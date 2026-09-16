#!/bin/sh
# AstVisVTK：基于 VTK 的三维可视化（需要 vtk，未探测到时该模块不参与构建）

target "AstVisVTK"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstVisVTK" "{public}"
    add_defines "AST_BUILD_LIB_VISVTK"
    add_deps "AstUtil" "AstCore" "AstSim" "AstVisualization"
    add_headerfiles "(**.hpp)" "AstVisVTK"
    add_headerfiles "(**.h)" "AstVisVTK"
target_end
