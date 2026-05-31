target("AppAnalyzer")
    if is_plat("wasm") then
        add_rules("qt.widgetapp_static")
    else
        add_rules("qt.widgetapp")
    end
    add_files("AppMain.cpp", "AppMain.hpp")
    add_deps("AstGUI", "AstAnalyzer")
    if not has_package("qt5widgets") and not is_plat("wasm") then
        set_enabled(false)
    end
