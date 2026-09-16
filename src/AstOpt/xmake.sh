#!/bin/sh
# AstOpt：优化（NLP 问题接口、Ipopt/Snopt/Minpack 求解器封装）

target "AstOpt"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstOpt" "{public}"
    add_defines "AST_BUILD_LIB_OPT"
    add_deps "AstCore" "AstUtil"
    add_headerfiles "(**.hpp)" "AstOpt"
    add_headerfiles "(**.h)" "AstOpt"
target_end
