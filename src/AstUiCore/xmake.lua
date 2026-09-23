target("AstUiCore")
    if is_plat("wasm") then
        add_rules("ast.qt.static")
    else
        add_rules("ast.qt.shared")
    end
    add_files("**.cpp")
    add_files("**.hpp")
    -- add_files("**.ts")
    -- add_files("../../data/*.qrc")
    -- add_files("**.ui")
    add_headerfiles("**.hpp", {prefixdir="AstUiCore"})
    add_headerfiles("**.h", {prefixdir="AstUiCore"})
    add_deps(
        "AstUtil", "AstSim", "AstCore", "AstMath", "AstAnalyzer", "AstLoader", "AstEngine",
        "AstUiAI", "AstUiUtil", "AstUiDataUpdate"
    )
    add_defines("AST_BUILD_LIB_UICORE")
    set_default(false)
    if not has_package("qt") and not is_plat("wasm") then
        set_enabled(false)
    end
