-- 界面工具插件示例：编译为独立动态库，运行时由主程序扫描；
-- 只有用户点击菜单项时主程序才会加载本库
if not is_plat("wasm") and has_package("qt") then
    target("AstPluginUiToolDemo")
        set_kind("shared")
        add_rules("ast.qt.shared")
        add_files("**.cpp")
        add_deps("AstUiUtil")
end
