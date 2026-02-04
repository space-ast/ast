-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.0.1", {build="%Y%m%d"})

-- 编译选项：是否编译测试工程
option("with_test")
    set_default(true)
option_end()

-- 设置 c++代码标准：c++11，c代码标准：c99
if not is_plat("windows") then  -- fixme: msvc下添加c++11后生成vs工程有问题，所以这里不设置c++11标准
    set_languages("c++11")
end

-- 添加包含目录
add_includedirs("include")

-- 添加编译规则
add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

-- 设置警告等级，并且将警告作为编译错误处理
set_warnings("more", "error")

-- 设置编译策略
set_policy("build.progress_style", "multirow")
-- set_policy("package.precompiled", false)     // 禁止从远程下载预编译的第三方库，而是在本地从源代码编译

-- linux平台添加rpath
if is_plat("linux") then
    add_rpathdirs("$ORIGIN")
elseif is_plat("windows") then
    add_cxflags(
        "/wd4819"           -- 忽略警告：代码页
        ,"/wd4251"          -- 忽略警告：需要有 dll 接口
        ,"/wd4996"          -- 忽略警告：已否决的函数或参数
        -- ,"/wd4244"          -- 忽略警告：转换可能丢失数据
        -- ,"/wd4090"           -- 忽略警告：不同的“const”限定符
    )  
    if is_mode("debug") then
        set_runtimes("MDd") -- 调试模式下使用MDd动态链接库
    else
        set_runtimes("MD")  -- 其他模式下使用MD动态链接库
    end
end

-- 生成汇编文件的编译选项
-- add_cxflags("-save-temps")
-- add_cxflags("/FA")

-- 添加自定义第三方库描述文件仓库
add_repositories("ast-repo repo", {rootdir = os.scriptdir()})

-- 下载并安装第三方库（可选）
add_requires("openscenegraph", {optional = true, configs = {shared = true}})
add_requires("qt5base", "qt5widgets", "qt5gui", {optional = true})
add_requires("eigen", {optional = true, configs = {headeronly = true}})
add_requires("opengl", {optional = true})
add_requires("fmt", {optional = true})
add_requires("sofa", {optional = true})
add_requires("matplotplusplus", {optional = true}) 
add_requires("libf2c", {optional = true})
add_requires("cminpack", {optional = true, configs = {long_double = true}})

-- add_requires("llvm", {optional = true})
-- set_toolchains("@llvm")

-- 添加abseil库（可选）
-- add_requires("abseil", {optional = true})
-- if has_package("abseil") then
--     add_packages("abseil")
--     add_defines("AST_WITH_ABSEIL")
-- end

-- 添加fmt库依赖（可选）
if has_package("fmt") then
    add_packages("fmt")
    add_defines("AST_WITH_FMT")
end

if has_package("eigen") then
    add_defines("AST_WITH_EIGEN")
    add_packages("eigen")
end

-- 添加matplot++库依赖（可选）
if has_package("matplotplusplus") then
    add_packages("matplotplusplus")
    add_defines("AST_WITH_MATPLOT")
else 
    add_defines("AST_NO_MATPLOT")
end

-- 导入子目录配置
includes("thirdparty")
includes("src")
includes("projects")
includes("examples")

-- 导入测试配置
if has_config("with_test") then
    add_requires("gtest <=1.12.1", {optional = true})  -- gtest v1.12.1 for c++11
    add_requires("benchmark", {optional = true})
    includes("test")
end

-- 自定义任务：复制数据目录到构建目录
task("cpdata")
    set_menu{
        usage = "xmake cpdata",
        description = "Copy data directory to build directory"
    }
    on_run(function ()
        local srcpath = path.join(os.projectdir(), "data")
        local modes = {"release", "debug", "coverage"}
        local plats = {os.host(), "mingw"}
        for _, plat in ipairs(plats) do
            local arch = os.arch()
            if plat == "mingw" then
                arch = "x86_64"
            end
            for _, mode in ipairs(modes) do
                local dstpath = path.join(os.projectdir(), format("build/%s/%s/%s/", plat, arch, mode))
                if not os.exists(dstpath) then
                    os.mkdir(dstpath)
                end
                os.cp(srcpath, dstpath)
                print("dstpath:", dstpath)
            end
        end
    end)
task_end()


task("gitpush")
    set_menu{
        usage = "xmake gitpush",
        description = "Push git repository"
    }
    on_run(function ()
        os.exec("python " .. path.join(os.scriptdir(), "scripts/git_push_retry.py"))
    end)
task_end()


task("genheader")
    set_menu{
        usage = "xmake genheader",
        description = "Generate header file"
    }
    on_run(function ()
        os.exec("python " .. path.join(os.scriptdir(), "scripts/gen_redirect_header.py"))
    end)
task_end()



-- 导入打包配置
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
