-- gnuplot 绘图后端插件：包装 matplot++ 自带的 gnuplot 管道后端。
-- gnuplot 只是运行时外部程序，不提供 xmake package，这里仅依赖 matplot++。
if has_package("matplotplusplus") and not is_plat("wasm") then
    target("AstPluginGnuplot")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstPlot")
        add_packages("matplotplusplus")
        add_defines("AST_BUILD_PLUGIN_GNUPLOT", "NOMINMAX")
end
