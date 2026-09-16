#!/bin/sh
# AstVisualization：可视化基础层

target "AstVisualization"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstVisualization" "{public}"
    add_defines "AST_BUILD_LIB_VISUALIZATION"
    add_deps "AstUtil" "AstCore" "AstSim"
    add_headerfiles "(**.hpp)" "AstVisualization"
    add_headerfiles "(**.h)" "AstVisualization"
target_end
