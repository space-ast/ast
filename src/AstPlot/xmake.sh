#!/bin/sh
# AstPlot：绘图（matplot++ / agg 后端）

target "AstPlot"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstPlot" "{public}"
    add_defines "AST_BUILD_LIB_PLOT" "NOMINMAX"
    add_headerfiles "(**.hpp)" "AstPlot"
    add_headerfiles "(**.h)" "AstPlot"
target_end
