target("AppMissionAnalysis")
    add_files("**.cpp")
    if not has_package("qt") then
        set_kind("phony")
    else
        add_rules("ast.qt.widgetapp")
        add_deps("AstUiCore", "AstUiDataUpdate")
    end
