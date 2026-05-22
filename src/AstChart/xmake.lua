target("AstChart")
    set_kind("shared")
    add_rules("qt.shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstChart"})
    add_defines("AST_BUILD_LIB_CHART")
    add_defines("NOMINMAX")
    add_frameworks("QtWidgets", "QtGui", "QtCore")
    add_deps("AstUtil", "AstSim", "AstCore", "AstMath")
    on_config(function (target)
        target:add("qt.moc.flags", "-DAST_NAMESPACE_BEGIN=namespace ast{")
    end)
    if has_package("matplotplusplus") and has_package("qwt") then
        add_packages("matplotplusplus", "qwt")
    else
        -- set_enabled(false)
        set_kind("phony")
    end

