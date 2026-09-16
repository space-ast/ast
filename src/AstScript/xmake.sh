#!/bin/sh
# AstScript：嵌入式脚本语言（解释器、解析器、调试器）

target "AstScript"
    set_kind "shared"
    add_files "**.cpp"
    add_files "**.cxx"
    add_includedirs "../../include/AstScript" "{public}"
    add_defines "AST_BUILD_LIB_SCRIPT"
    add_deps "AstUtil"
    add_headerfiles "(**.hpp)" "AstScript"
    add_headerfiles "(**.h)" "AstScript"
    if is_plat "windows" "mingw"; then
        add_syslinks "oleaut32" "ole32" "uuid"
    fi
target_end
