-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.0.1", {build="%Y%m%d"})

-- 工程配置选项：是否编译测试工程
option("with_test")
    set_default(true)
option_end()

-- 工程配置选项：是否编译示例工程
option("with_examples")
    set_default(true)
option_end()

-- 工程配置选项：是否编译项目工程
option("with_projects")
    set_default(true)
option_end()

-- 工程配置选项：是否编译第三方库
option("with_thirdparty")
    set_default(true)
option_end()


-- 工程配置选项：是否检查警告（将警告作为编译错误处理）
option("check_warnings")
    set_default(true)
option_end()

-- 工程配置选项：是否使用系统Qt库
option("system_qt")
    set_default(false)
option_end()

-- 设置库文件后缀名
-- set_suffixname("_")
-- add_defines([[_AST_LIB_SUFFIX="_"]])

-- 设置 c++代码标准：c++11，c代码标准：c99
if not is_plat("windows") then  -- fixme: msvc下添加c++11后生成vs工程有问题，所以这里不设置c++11标准
    set_languages("c++11")
end


-- 添加包含目录
add_includedirs("include")

-- 添加编译规则
-- 内置规则
add_rules(
    "mode.debug", "mode.release", "mode.releasedbg", 
    "mode.coverage"
)                                                           -- 调试模式、发布模式、代码覆盖率模式
-- add_rules("plugin.vsxmake.autoupdate")                      -- 自动更新vsxmake工程
add_rules("c++.unity_build", {batchsize=20})                -- 开启unity build，提高编译效率
add_rules("c.unity_build", {batchsize=0})                   -- 不启用C语言的unity build

-- 自定义规则
includes("rules.lua")                                       -- 导入ast项目自定义规则
add_rules("ast")                                            -- 添加ast项目自定义规则

-- 设置警告等级，并且将警告作为编译错误处理
if has_config("check_warnings") then
    set_warnings("more", "error")
end

-- 设置编译策略
set_policy("run.autobuild", true)                           -- 自动编译，当运行目标时自动编译
set_policy("build.progress_style", "multirow")              -- 编译进度条显示为多行
set_policy("package.precompiled", false)                    -- 禁止从远程下载预编译的第三方库，而是在本地从源代码编译(osg使用预编译库流水线会报错)

-- linux平台添加rpath
if is_plat("linux") then
    add_rpathdirs("$ORIGIN")                                -- 添加运行时库搜索路径，指向可执行文件所在目录
elseif is_plat("windows") then
    if is_mode("debug") then
        set_values("windows.subsystem", "console")
        -- 为了让AI生成的代码能正常编译
        add_includedirs("src")
    end
    add_defines("_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS")
    -- for msvc
    add_cxflags(
        "/wd4819"               -- 忽略警告：代码页
        ,"/wd4251"              -- 忽略警告：需要有 dll 接口
        ,"/wd4996"              -- 忽略警告：已否决的函数或参数
        -- ,"/wd4244"           -- 忽略警告：转换可能丢失数据
        -- ,"/wd4090"           -- 忽略警告：不同的“const”限定符
    )
    add_cxxflags(
        "/we4703"               -- 视为错误：未初始化的本地变量
        -- "/sdl"
    )

    -- add_cxxflags(
    --     "/analyze"               -- 开启代码分析
    -- )
    
    -- for clang
    add_cxflags(
        "-Wno-missing-braces"       -- 忽略警告：缺少大括号
    )
    if is_mode("debug") then
        set_runtimes("MDd")         -- 调试模式下使用MDd动态链接库
    else
        set_runtimes("MD")          -- 其他模式下使用MD动态链接库
    end
elseif is_plat("wasm") then         -- 编译为wasm
    -- for clang
    add_cxflags(
        "-Wno-missing-braces"       -- 忽略警告：缺少大括号
    )
end

-- 生成汇编文件的编译选项
-- add_cxflags("-save-temps")
-- add_cxflags("/FA")

-- 添加自定义第三方库描述文件仓库
add_repositories("ast-repo repo", {rootdir = os.scriptdir()})

-- 设置与Qt版本对应的emscripten工具链
if is_plat("wasm") then
    if os.getenv("QT6") then
        add_requires("emscripten 3.1.25")   -- 对应 qt 6.5.2
    else
        add_requires("emscripten 1.39.8")   -- 对应 qt 5.15.x  -- 见 https://wiki.qt.io/Qt_for_WebAssembly#Install_the_Emscripten_SDK
    end
    set_toolchains("emcc@emscripten")
end

-- 下载并安装第三方库（可选）
add_requires("python 3.x", {optional = true})                                   -- 可选的Python库，用于编译python库
add_requires("swig >=4.2", {optional = true})                                   -- 可选的SWIG库，用于生成Python绑定代码，必须 >=4.2 才支持 enum class : type
add_requires("gtest <=1.12.1", {optional = true, configs = {cmake = false}})    -- 可选的gtest库，用于单元测试，gtest v1.12.1 for c++11
add_requires("benchmark", {optional = true})                                    -- 可选的benchmark库，用于性能测试
add_requires("replxx", {optional = true})                                       -- 可选的replxx库，用于命令行交互
add_requires("openscenegraph", {optional = true, configs = {shared = true}})    -- 可选的OpenSceneGraph库，共享库版本，用于图形渲染
add_requires("openframes", {optional = true})                                   -- 可选的OpenFrames库，用于三维可视化
add_requires("opengl", {optional = true})                                       -- 可选的OpenGL库，用于图形渲染
add_requires("glu", {optional = true})                                          -- 可选的GLU库，用于3D模型渲染
add_requires("eigen", {optional = true, configs = {headeronly = true}})         -- 可选的Eigen库，头文件版本，用于线性代数计算
add_requires("fmt", {optional = true})                                          -- 可选的fmt库，用于格式化输出
add_requires("sofa", {optional = true})                                         -- 可选的iau-sofa库，用于天文计算
add_requires("agg", {optional = true, configs = {shared = true}})               -- 可选的agg库，用于绘图
add_requires("matplotplusplus", {optional = true})                              -- 可选的matplot++库，用于绘图
add_requires("libf2c", {optional = true})                                       -- 可选的libf2c库，用于f2c转换
add_requires("cminpack", {optional = true, configs = {long_double = true}})     -- 可选的cminpack库，用于求解非线性方程组
add_requires("cspice", {optional = true})                                       -- 可选的cspice库，用于天文计算
add_requires("qt5base", {optional = true})                                           -- 可选的Qt库，包含基础、窗口部件和GUI模块

local qt_sdkver = get_config("qt_sdkver")
local system_qt = get_config("system_qt")
-- 可选的Qwtplot库，用于Qt绘图，共享库版本
add_requires("qwt", {optional = true, 
    configs = {
        shared = true, debug = is_mode("debug")
        -- , qt_sdkver = qt_sdkver, system_qt = system_qt
    }
})

-- 触发Qt库的自动下载
local qt_config = {
    override = true,
    optional = true,
    system = system_qt
}
if qt_sdkver ~= "auto" then
    qt_config.version = qt_sdkver
end

add_requireconfs("qt5base", qt_config)
add_requireconfs("qwt.qt5base", qt_config)

-- add_requires("libintl", {optional = true})                                      -- 可选的libintl库，用于国际化
-- add_requires("nlohmann_json", {optional = true})                                -- 可选的nlohmann_json库，用于JSON解析
-- add_requires("jsoncpp", {optional = true})                                      -- 可选的jsoncpp库，用于JSON解析
-- add_requires("curl", {optional = true})                                         -- 可选的curl库，用于HTTP请求
-- add_requires("nodeeditor", {optional = true})                                   -- 可选的nodeeditor库，用于节点编辑器

-- 使用llvm工具链编译（可选）
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
else
    add_defines("AST_NO_FMT")
end

-- 添加Eigen库依赖（可选）
if has_package("eigen") then
    add_defines("AST_WITH_EIGEN")
    add_packages("eigen")
else
    add_defines("AST_NO_EIGEN")
end

-- 添加cspice库依赖（可选）
if has_package("cspice") then
    add_packages("cspice")
    add_defines("AST_WITH_CSPICE")
else
    add_defines("AST_NO_CSPICE")
end


if has_package("agg") then
    add_defines("AST_WITH_AGG")
    add_packages("agg")
end

-- 添加matplot++库依赖（可选）
if has_package("matplotplusplus") then
    add_packages("matplotplusplus")
    add_defines("AST_WITH_MATPLOT")
else 
    add_defines("AST_NO_MATPLOT")
end

-- 添加libintl库依赖（可选）
-- if has_package("libintl") then
--     add_packages("libintl")
--     add_defines("AST_WITH_LIBINTL")
-- else 
--     add_defines("AST_NO_LIBINTL")
-- end

-- 添加nlohmann_json库依赖（可选）
-- if has_package("nlohmann_json") then
--     add_packages("nlohmann_json")
--     add_defines("AST_WITH_NLOHMANN_JSON")
-- else
--     add_defines("AST_NO_NLOHMANN_JSON")
-- end


-- 添加jsoncpp库依赖（可选）
-- if has_package("jsoncpp") then
--     add_packages("jsoncpp")
--     add_defines("AST_WITH_JSONCPP")
-- else
--     add_defines("AST_NO_JSONCPP")
-- end

-- -- 添加curl库依赖（可选）
-- if has_package("curl") then
--     add_packages("curl")
--     add_defines("AST_WITH_CURL")
-- else
--     add_defines("AST_NO_CURL")
-- end

-- 添加nodeeditor库依赖（可选）
-- if has_package("nodeeditor") then
--     add_packages("nodeeditor")
--     add_defines("AST_WITH_NODEEDITOR")
-- else
--     add_defines("AST_NO_NODEEDITOR")
-- end


-- 添加插件
add_plugindirs("scripts/plugins")

includes("src")


-- 导入项目工程配置
if has_config("with_projects") then
    includes("projects")
end

-- 导入示例工程配置
if has_config("with_examples") then
    includes("examples")
end

-- 导入测试配置
if has_config("with_test") then
    includes("test")
end

-- 导入第三方库工程配置
if has_config("with_thirdparty") then
    includes("thirdparty")
end

-- 导入打包配置
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end