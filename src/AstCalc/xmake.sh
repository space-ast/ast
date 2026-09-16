#!/bin/sh
# AstCalc：天文计算器与几何搜索

target "AstCalc"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstCalc" "{public}"
    add_defines "AST_BUILD_LIB_CALC"
    add_headerfiles "(**.hpp)" "AstCalc"
    add_headerfiles "(**.h)" "AstCalc"
target_end
