-- Qwt 绘图后端插件：Qt/Qwt 交互式后端，运行时由主程序扫描加载（不参与链接）
if not is_plat("wasm") and has_package("qt") and has_package("qwt") and has_package("matplotplusplus") then
    target("AstPluginQwt")
        set_kind("shared")
        -- qwt 后端包含 Q_OBJECT 类，需要 Qt 的 moc 规则
        add_rules("ast.qt.shared")
        add_files("**.cpp")
        add_files("**.hpp")
        add_deps("AstPlot", "AstUtil")
        add_packages("matplotplusplus", "qwt", "opengl")
        add_defines("AST_BUILD_PLUGIN_QWT", "NOMINMAX")
        -- 打包时不过滤没有依赖到的动态库，确保 qwt 等依赖进入安装包
        set_policy("install.strip_packagelibs", false)
end
