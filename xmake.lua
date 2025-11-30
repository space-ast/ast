-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.0.1", {build="%Y%m%d"})

-- 编译选项：是否编译测试工程
option("with_test")
    set_default(false)
option_end()

-- 设置c代码标准：c99， c++代码标准：c++11
-- set_languages("cxx11", "c99")

-- 添加包含目录
add_includedirs("include")

-- 添加编译规则
add_rules("mode.debug", "mode.release", "mode.coverage")
add_rules("plugin.vsxmake.autoupdate")

-- 设置编译策略
set_policy("build.progress_style", "multirow")

-- linux平台添加rpath
if is_plat("linux") then
    add_rpathdirs("$ORIGIN")
end

add_requires("openscenegraph", {shared = true, optional = true})
add_requires("qt5base", "qt5widgets", "qt5gui", {optional = true})

-- 导入子目录配置
includes("thirdparty")
includes("src")
includes("projects")

-- 导入测试配置
if has_config("with_test") then
    add_requires("gtest v1.12.1", {optional = true})  -- gtest v1.12.1 for c++11
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
        if is_host("windows") then
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/windows/x64/debug/"))
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/windows/x64/release/"))
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/windows/x64/coverage/"))
        else
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/linux/x64/debug/"))
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/linux/x64/release/"))
            os.cp(path.join(os.projectdir(), "data"), path.join(os.projectdir(), "build/linux/x64/coverage/"))
        end
    end)
task_end()

-- 导入打包配置
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
