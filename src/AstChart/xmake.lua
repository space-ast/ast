target("AstChart")
    set_kind("shared")
    add_rules("ast.qt.shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstChart"})
    add_headerfiles("**.h", {prefixdir="AstChart"})
    add_defines("AST_BUILD_LIB_CHART")
    add_defines("NOMINMAX")
    add_deps("AstUtil", "AstSim", "AstCore", "AstMath")
    if has_package("matplotplusplus") and has_package("qwt") and has_package("opengl") then
        add_packages("matplotplusplus", "qwt", "opengl")
    else
        -- set_enabled(false)
        set_kind("phony")
    end
    if not has_package("qt") then
        set_enabled(false)
    end
