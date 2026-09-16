#!/bin/sh
# AstTest：内置测试框架（兼容 GoogleTest 的头文件式框架）

target "AstTest"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstTest" "{public}"
    add_defines "AST_BUILD_LIB_TEST"
    add_deps "AstUtil" "AstMath" "AstCore" "AstSim" "AstScript" "AstLoader"
    add_headerfiles "(**.hpp)" "AstTest"
    add_headerfiles "(**.h)" "AstTest"
target_end
