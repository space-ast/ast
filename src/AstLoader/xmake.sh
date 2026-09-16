#!/bin/sh
# AstLoader：动态库加载器

target "AstLoader"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstLoader" "{public}"
    add_defines "AST_BUILD_LIB_LOADER"
    add_deps "AstUtil" "AstSim" "AstCore" "AstMath" "AstReport"
    add_headerfiles "(**.hpp)" "AstLoader"
    add_headerfiles "(**.h)" "AstLoader"
target_end
