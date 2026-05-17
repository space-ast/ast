target("AppAnalyzer")
    add_rules("qt.widgetapp")
    add_files("AppMain.cpp", "AppMain.hpp")
    add_deps("AstGUI", "AstAnalyzer")
    if not has_package("qt5widgets") then
        set_enabled(false)
    end
