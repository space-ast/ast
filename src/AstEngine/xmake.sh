#!/bin/sh
# AstEngine：仿真引擎

target "AstEngine"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstEngine" "{public}"
    add_defines "AST_BUILD_LIB_ENGINE"
    add_deps "AstCore" "AstUtil" "AstMath" "AstSim"
    add_headerfiles "(**.hpp)" "AstEngine"
    add_headerfiles "(**.h)" "AstEngine"
target_end
