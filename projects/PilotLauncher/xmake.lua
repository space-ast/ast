target("PilotLauncher")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    add_deps("AstUiPilot", "AstGUI")
    set_default(false)
    if not has_package("qt") and not is_plat("wasm") then
        set_enabled(false)
    end
