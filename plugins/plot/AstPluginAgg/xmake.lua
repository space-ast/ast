-- Agg 绘图后端插件：编译为独立动态库，运行时由主程序扫描加载（不参与链接）
if has_package("agg") and has_package("matplotplusplus") and not is_plat("wasm") then
    target("AstPluginAgg")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstPlot")
        add_packages("agg", "matplotplusplus")
        add_defines("AST_BUILD_PLUGIN_AGG", "NOMINMAX")
end
