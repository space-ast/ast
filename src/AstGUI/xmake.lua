target("AstGUI")
    add_rules("ast.qt.shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_files("**.ts")
    -- add_files("../../data/*.qrc")
    -- add_files("**.ui")
    add_headerfiles("**.hpp", {prefixdir="AstGUI"})
    add_deps(
        "AstUtil", "AstSim", "AstCore", "AstMath", "AstAnalyzer", "AstLoader",
        "AstUiAI", "AstUiUtil", "AstUiDataUpdate"
    )
    add_defines("AST_BUILD_LIB_GUI")
    set_default(false)
