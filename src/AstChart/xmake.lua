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
    -- Qt 是硬依赖，没有 Qt 时整个目标禁用；
    -- 其余绘图后端是可选的，对应的源码本有 AST_WITH_* 宏保护
    if has_package("qt") then
        add_packages("matplotplusplus")
        add_packages("qwt")
        add_packages("opengl")
    else
        set_enabled(false)
    end
