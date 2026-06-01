target("AppAnalyzer")
    add_rules("ast.qt.widgetapp")
    add_files("AppMain.cpp", "AppMain.hpp")
    add_deps("AstGUI", "AstAnalyzer")

    if not has_package("qt5widgets") and not is_plat("wasm") then
        set_enabled(false)
    end
