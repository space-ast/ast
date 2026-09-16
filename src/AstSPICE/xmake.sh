#!/bin/sh
# AstSPICE：CSPICE 工具箱集成

target "AstSPICE"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstSPICE" "{public}"
    add_defines "AST_BUILD_LIB_SPICE"
    add_deps "AstMath" "AstCore"
    add_headerfiles "(**.hpp)" "AstSPICE"
    add_headerfiles "(**.h)" "AstSPICE"
target_end
