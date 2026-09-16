#!/bin/sh
# AstAI：AI 代理、对话服务

target "AstAI"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstAI" "{public}"
    add_defines "AST_BUILD_LIB_AI"
    add_deps "AstUtil"
    add_headerfiles "(**.hpp)" "AstAI"
    add_headerfiles "(**.h)" "AstAI"
target_end
