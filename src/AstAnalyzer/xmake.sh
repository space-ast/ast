#!/bin/sh
# AstAnalyzer：任务分析（约束、变量、打靶法、灵敏度、权衡研究等）

target "AstAnalyzer"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstAnalyzer" "{public}"
    add_defines "AST_BUILD_LIB_ANALYZER"
    add_deps "AstCore"
    add_headerfiles "(**.hpp)" "AstAnalyzer"
    add_headerfiles "(**.h)" "AstAnalyzer"
target_end
