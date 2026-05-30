target("AppGUI")
    if is_plat("wasm") then
        add_rules("qt.widgetapp_static")
    else
        add_rules("qt.widgetapp")
    end
    add_files("**.cpp")
    add_files("**.hpp")
    -- add_files("**.ui")
    add_deps("AstGUI")
    if not has_package("qt5widgets") and not is_plat("wasm") then
        set_enabled(false)
    end
