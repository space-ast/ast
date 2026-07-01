target("AppDataUpdate")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    if is_plat("windows", "mingw") then
        add_files("AppDataUpdate.rc")
    end
    add_deps("AstUiDataUpdate", "AstCore")
