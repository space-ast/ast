target("AppGUI")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    -- add_files("**.ui")
    add_deps("AstGUI", "AstUiDataUpdate")
    -- if not has_package("qt5widgets") and not is_plat("wasm") then
    --     set_enabled(false)
    -- end
