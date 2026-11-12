-- VTK 可视化后端插件：编译为独立动态库，运行时由主程序扫描加载（不参与链接）
if has_package("vtk") and not is_plat("wasm") then
    target("AstPluginVTK")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstVisualization", "AstUtil", "AstCore", "AstSim")
        add_packages("vtk")
        add_defines("AST_BUILD_PLUGIN_VTK", "NOMINMAX")
end
