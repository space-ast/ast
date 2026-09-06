target("AstVisVTK")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp", {prefixdir="AstVisVTK"})
    add_defines("AST_BUILD_LIB_VISVTK")
    add_deps("AstUtil", "AstCore", "AstSim", "AstVisualization")
    -- VTK 为可选后端依赖：无 VTK 时模块自动禁用，不参与默认构建
    if not has_package("vtk") then
        set_enabled(false)
    else
        add_packages("vtk")
    end
