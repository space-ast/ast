target("AppMissionAnalysis")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    add_deps("AstGUI", "AstUiDataUpdate")
    if not has_package("qt") then
        set_enabled(false)
    end
