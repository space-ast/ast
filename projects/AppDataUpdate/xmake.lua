target("AppDataUpdate")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    if is_plat("windows") then
        add_files("AppDataUpdate.rc")
    end
    add_deps("AstUiDataUpdate", "AstCore")
    if not has_package("qt") then
        set_enabled(false)
    end