-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.4.0", {build="%Y%m%d", soname = false})

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
    set_default(false)
option_end()

-- 工程配置选项：是否编译插件工程
option("with_plugins")
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

-- 工程配置选项：第三方依赖策略
--   all（默认）：按需下载/构建/使用第三方库
--   none：不下载、不链接任何第三方库
--         此时不会执行任何 add_requires，has_package() 恒为 false，AST_WITH_* 宏均不会被定义
--         依赖第三方库的代码走内置回退，依赖第三方库的目标（Qt/绘图/可视化/Python 绑定等）自动禁用
--   用法：xmake f --packages=none -c -m release
option("packages")
    set_default("all")
    set_values("all", "none")
option_end()


-- 设置库文件后缀名
-- set_suffixname("_")
-- add_defines([[_AST_LIB_SUFFIX="_"]])

-- 设置 c++代码标准：c++11，c代码标准：c99
if not is_plat("windows") then
    set_languages("c++11")
else
    -- fixme: msvc下添加c++11后生成vs工程有问题，所以这里不设置c++11标准
    -- windows下统一显式设置为c++14：
    --   msvc最低只支持c++14（xmake会把c++11映射成msvc的-std:c++11，等价于默认的c++14）；
    --   clang下vs2022+的stl也要求c++14起步。
    -- 另外，xmake内置的qt规则在target未设置c++标准时，会按Qt5往target注入一个c++11：
    --   msvc会忽略它，但clang会真的降到-std=c++11，导致vs2022+的stl无法编译。显式设置可一并避免。
    set_languages("c++14")
end


-- 添加包含目录
add_includedirs("include")

-- 添加编译规则
-- 内置规则
add_rules(
    "mode.debug", "mode.release", "mode.releasedbg", 
    "mode.coverage", 
    "mode.asan", "mode.msan", "mode.ubsan", "mode.valgrind"
)                                                           -- 调试模式、发布模式、代码覆盖率模式
-- add_rules("plugin.vsxmake.autoupdate")                      -- 自动更新vsxmake工程
add_rules("c++.unity_build", {batchsize=20})                -- 开启unity build，提高编译效率
add_rules("c.unity_build", {batchsize=0})                   -- 不启用C语言的unity build

-- 自定义规则
includes("rules.lua")                                       -- 导入ast项目自定义规则
add_rules("ast")                                            -- 添加ast项目自定义规则
add_rules("ast.package_defines")                            -- 按目标的 add_packages() 自动添加 AST_WITH_* 宏

-- 设置警告等级，并且将警告作为编译错误处理
if has_config("check_warnings") then
    set_warnings("more", "error")
end

-- 设置编译策略
set_policy("run.autobuild", true)                           -- 自动编译，当运行目标时自动编译
set_policy("build.progress_style", "multirow")              -- 编译进度条显示为多行
set_policy("package.precompiled", false)                    -- 禁止从远程下载预编译的第三方库，而是在本地从源代码编译(osg使用预编译库流水线会报错)
set_policy("build.rpath", false)                            -- 禁止xmake隐式添加runpath
set_policy("install.rpath", false)                          -- 禁止xmake隐式添加runpath
-- 关闭打包时的包内共享库裁剪：pack 会用 ldd 解析插件的 DT_NEEDED，manylinux 里系统 libcurl(7.61)
-- 与包内 libcurl(8.21) 的 SONAME 都是 libcurl.so.4，ldd 命中 /usr/lib64/libcurl.so.4 后，
-- 包内 libcurl 在过滤时被判为“没用到”而剔除，nightly 的 libAstPluginCURL.so 就会缺 NEEDED 库。
-- 详见 .agents/notes/2026-11-11-libcurl-missing-from-linux-nightly-package.md
set_policy("install.strip_packagelibs", false)

-- MSVC（VS2022 17.10 起）的 STL 会给 basic_string/vector 打 ASan 容器标注，并用
--   #pragma detect_mismatch("annotate_string"/"annotate_vector", ...)
-- 记录编译时有没有开 /fsanitize=address：开了写 1，没开写 0。
-- 同一个二进制里出现两个不同的值就是 LNK2038、 LNK1319
-- 本工程目标都带 /fsanitize=address（值 1），
-- 而 fmt/gtest/agg/qwt 这些第三方静态库是按非 ASan 编出来的（值 0）
-- 关掉 STL 容器标注后本工程目标也写 0，两边一致；
-- ASan 本身照常生效（堆/栈/全局越界、use-after-free 都还在），
-- 只是不再额外标记「容器capacity 之内、size 之外」的越界访问。
-- 想连容器标注一起要，得让所有依赖也用 ASan 编（工程级set_policy("build.sanitizer.address", true)），
-- 但 xmake-repo 的 gtest 在on_test 里会链自己刚编出来的 asan 版库并因同样原因装不上，暂时走不通
if is_mode("asan") then
    if is_plat("windows") then
        add_defines("_DISABLE_STL_ANNOTATION")
    else
        set_policy("build.sanitizer.address", true)
    end
end

if is_mode("msan") then
    -- MSan 只有 clang 编译器支持，且只在 Linux/FreeBSD 这类平台提供
    assert(not is_plat("windows"), "msan 不支持 Windows, 请在 Linux 上构建")
    set_toolchains("clang")
    set_policy("build.sanitizer.memory", true)
end

if is_mode("ubsan") then
    -- ubsan 在 windows 上只有 clang 编译器支持
    -- Windows 上用 clang-cl 而不是 GNU MinGW 驱动的 clang++
    -- "clang-cl@llvm" 是 xmake 的 toolchain@packages 语法：
    -- 工具链用内置的 clang-cl, 二进制从 llvm 包获取
    if is_plat("windows") then
        add_requires("llvm", {optional = true})
        set_toolchains("clang-cl@llvm")
    else
        -- linux 下 gcc 和 clang 都支持，默认使用 gcc 编译器即可
        -- set_toolchains("clang")
    end
    set_policy("build.sanitizer.undefined", true)
end

if is_plat("windows") then
    if is_mode("debug") then
        set_values("windows.subsystem", "console")
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


if is_config("packages", "none") then
    add_rules("ast.no_packages")
else
    -- 第三方依赖在此统一声明
    -- --packages=none 时跳过，不下载、不注册任何包
    
    -- 系统内置的库（一般不需要下载和安装）
    add_requires("openmp", {optional = true})                                       -- 可选的OpenMP库，用于并行计算
    add_requires("opengl", {optional = true})                                       -- 可选的OpenGL库，用于图形渲染
    add_requires("glu", {optional = true})                                          -- 可选的GLU库，用于3D模型渲染

    -- 下载并安装第三方库（可选）
    add_requires("python 3.x", {optional = true, configs = {headeronly = is_plat("linux")}})
    add_requires("swig >=4.4.1", {optional = true, asan=false})                     -- 可选的SWIG库，用于生成Python绑定代码，必须 >=4.4.1，见下方说明
    -- SWIG 版本要求说明：
    --   >=4.2 才支持 `enum class : type` 语法；
    --   >=4.3 才能求值本项目枚举中的初值表达式（enum class EDimension : dimension_t 的初值为constexpr 常量，
    --         enum EColor : color_t 的初值为 rgb()/rgba() constexpr 函数调用），
    --         4.2.0 会对这些初值报 "Error: Type error. Expecting an integral type" 并以非 0 退出；
    --   >=4.4.1 修复了 4.4.0 的问题：AstPy.i 限定了 Py_LIMITED_API=3.5，而 4.4.0 只按 Python 头文件
    --         版本判断能否直接调用 3.13 新增 API（PyImport_AddModuleRef、PyType_GetFullyQualifiedName 等），
    --         这些 API 在 Py_LIMITED_API=3.5 下不会被声明，导致生成的 AstPy.cpp 编译失败。
    add_requires("gtest <=1.12.1", {optional = true, configs = {cmake = false}})    -- 可选的gtest库，用于单元测试，gtest v1.12.1 for c++11
    add_requires("benchmark", {optional = true})                                    -- 可选的benchmark库，用于性能测试
    add_requires("replxx", {optional = true})                                       -- 可选的replxx库，用于命令行交互
    -- add_requires("openscenegraph", {optional = true, configs = {shared = true}})    -- 可选的OpenSceneGraph库，共享库版本，用于图形渲染
    -- add_requires("openframes", {optional = true})                                   -- 可选的OpenFrames库，用于三维可视化
    -- add_requires("vulkanscenegraph", {optional = true, configs = {shared = true}})  -- 可选的VulkanSceneGraph库，共享库版本，用于图形渲染
    -- add_requires("vtk", {optional = true, configs = {shared = true}})
    -- add_requireconfs("vtk.seacas", {version = "2025.08.28"})
    add_requires("eigen", {optional = true, configs = {headeronly = true}})         -- 可选的Eigen库，头文件版本，用于线性代数计算
    add_requires("fmt", {optional = true})                                          -- 可选的fmt库，用于格式化输出
    add_requires("sofa", {optional = true})                                         -- 可选的iau-sofa库，用于天文计算
    add_requires("agg", {optional = true, configs = {shared = true}})               -- 可选的agg库，用于绘图
    add_requires("matplotplusplus", {optional = true})                              -- 可选的matplot++库，用于绘图
    add_requires("libf2c", {optional = true})                                       -- 可选的libf2c库，用于f2c转换
    add_requires("duktape", {optional = true})                                      -- 可选的Duktape嵌入式JavaScript引擎
    add_requires("openvbs", {optional = true})                                      -- 可选的OpenVBS脚本引擎，用于跨平台执行VBScript
    add_requires("libcurl", {optional = true})                                      -- 可选的libcurl库，用于HTTP请求
    -- 可选的cpp-httplib（header-only），ssl=true 会带入 openssl3；
    -- cpp-httplib >=0.23.0 不再支持 32 位，x86 构建跳过该包
    if not is_plat("wasm") and not is_arch("x86") then
        add_requires("cpp-httplib", {optional = true, configs = {ssl = true, exceptions = false}})
    end

    -- windows 系统:
    -- libcurl 可以不依赖 openssl 库(直接使用系统自带的Schannel)，但是 cpp-httplib 仍然需要依赖 openssl 库
    -- 
    -- Linux 系统:
    -- libcurl 8.21 与 cpp-httplib 都要求 OpenSSL >= 3.0，
    -- 而 xmake-repo 的 openssl3 包在探测系统库时只看能否链接、不校验版本：
    -- AlmaLinux 8（manylinux_2_28）上的 OpenSSL 1.1.1k 会被当成 openssl3 直接满足依赖，
    -- 导致 libcurl 编译报 #error、cpp-httplib 的 CMake 找不到 OpenSSL 3，两个网络插件被静默跳过。
    -- 
    -- 因此不使用系统中的 openssl 库，直接从源码编译
    -- 同时将 openssl3 和 libcurl 都编译为共享库：两个插件共享一份，不必各自静态嵌入；
    add_requireconfs("**.openssl3", {system = false, configs = {shared = true}})
    add_requireconfs("libcurl", {configs = {shared = true}})

    add_requires("cminpack", {optional = true, configs = {long_double = true}})     -- 可选的cminpack库，用于求解非线性方程组
    add_requires("cspice", {optional = true})                                       -- 可选的cspice库，用于天文计算
    add_requires("ipopt", {optional = true, configs = {shared = true, debug = is_mode("debug")}}) -- 可选的ipopt库，用于求解非线性优化问题

    local qt_sdkver = get_config("qt_sdkver")
    local system_qt = get_config("system_qt")
    -- 触发Qt库的自动下载
    local qt_config = {
        override = true,
        optional = true,
        system = system_qt
    }
    if qt_sdkver ~= "auto" then
        qt_config.version = qt_sdkver
    end

    add_requires("qt", {optional = true})                                           -- 可选的Qt库，包含基础、窗口部件和GUI模块
    add_requires("qwt", {optional = true, configs = {shared = true, debug = is_mode("debug")}}) -- 可选的Qwtplot库，用于Qt绘图，共享库版本
    add_requires("qt-advanced-docking", {optional = true, configs = {shared = true, debug = is_mode("debug")}})
    add_requireconfs("qt", qt_config)
    add_requireconfs("qwt.qt", qt_config)
    add_requireconfs("qt-advanced-docking.qt", qt_config)
end

-- add_requires("libintl", {optional = true})                                      -- 可选的libintl库，用于国际化
-- add_requires("nlohmann_json", {optional = true})                                -- 可选的nlohmann_json库，用于JSON解析
-- add_requires("jsoncpp", {optional = true})                                      -- 可选的jsoncpp库，用于JSON解析
-- add_requires("nodeeditor", {optional = true})                                   -- 可选的nodeeditor库，用于节点编辑器

-- 使用llvm工具链编译（可选）
-- add_requires("llvm", {optional = true})
-- set_toolchains("@llvm")


-- 添加Eigen库依赖（可选）
if has_package("eigen") then
    -- header-only 库不会引入链接库，可以直接添加依赖
    add_packages("eigen")
end


-- 添加插件
add_plugindirs("scripts/plugins")

includes("src")

-- 导入插件工程配置
if has_config("with_plugins") then
    includes("plugins")
end

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