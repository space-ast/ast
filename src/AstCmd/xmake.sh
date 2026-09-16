#!/bin/sh
# AstCmd：CLI 命令系统（分发、路由）

target "AstCmd"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstCmd" "{public}"
    add_defines "AST_BUILD_LIB_CMD"
    add_deps "AstUtil" "AstCore" "AstSim"
    add_headerfiles "(**.hpp)" "AstCmd"
    add_headerfiles "(**.h)" "AstCmd"
target_end
