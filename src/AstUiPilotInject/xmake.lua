target("AstUiPilotInject")
    add_rules("ast.qt.shared")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_deps("AstUiPilot")
    set_default(false)
    if not has_package("qt5widgets") and not is_plat("wasm") then
        set_enabled(false)
    end
