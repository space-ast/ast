target("AppMissionAnalysis")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    add_deps("AstUiCore", "AstUiDataUpdate")
    -- 必须用 set_enabled(false) 而不是 set_kind("phony")：phony 的目标仍会留在
    -- project.targets() 里，被 xpack 当成要安装的目标，最后包装出来少一个可执行文件
    -- 却不报错。放在 target 体内才能让它被过滤掉。
    if not has_package("qt") then
        set_enabled(false)
    end
