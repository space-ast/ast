#!/bin/sh
# AstReport：报告输出

target "AstReport"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstReport" "{public}"
    add_defines "AST_BUILD_LIB_REPORT"
    add_deps "AstUtil" "AstCore"
    add_headerfiles "(**.hpp)" "AstReport"
    add_headerfiles "(**.h)" "AstReport"
target_end
