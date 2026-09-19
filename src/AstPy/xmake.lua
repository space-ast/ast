

target("AstPy")
    -- 目标平台/架构与宿主机不一致时（如 windows x86、windows arm64），xmake 会把 python 包
    -- 当成宿主机工具包（package kind = "binary"，见 xmake-repo packages/p/python/xmake.lua），
    -- 这类包不会把 includedirs/links 传给目标，且解析到的是宿主机架构的 python，
    -- 结果就是编译 AstPy.cpp 时报 "Cannot open include file: 'Python.h'"。
    if not is_plat(os.host()) or not is_arch(os.arch()) then
        set_enabled(false)
    elseif has_package("python") and has_package("swig") and not is_plat("mingw") then
        add_packages("swig")
        add_packages("python")
    else
        set_enabled(false)
    end
    add_files("AstPy.i")
    add_rules("swig.cpp", {moduletype = "python"})
    add_deps("AstUtil", "AstCore", "AstSim")
    add_defines("SWIG_PYTHON_INTERPRETER_NO_DEBUG")
    add_defines("SWIG", "AST_BUILD_LIB_PY")
    set_warnings("none")  -- swig 自动封装代码，忽略警告
    add_cxxflags("/bigobj")
    after_build(function(target)
        local autogendir = vformat(path.join("$(projectdir)", target:autogendir(), "rules", "swig"))
        local dest = target:targetdir()
        local src = path.join(autogendir, "**.py")
        pyfiles = os.files(src)
        print("pyfiles: ", pyfiles)
        print("dest: ", dest)
        os.cp(src, dest, {rootdir = autogendir})
    end)
    if is_plat("windows") and is_mode("debug") then
        set_suffixname("_d")
    end
