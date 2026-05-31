

target("AstPy")
    if has_package("python") and has_package("swig") and not is_plat("mingw") then
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
