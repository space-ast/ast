#!/bin/sh
# AstDataUpdate：数据更新（默认不参与构建，可用 `make AstDataUpdate` 单独编译）

target "AstDataUpdate"
    set_kind "shared"
    set_default false
    add_files "**.cpp"
    add_includedirs "../../include/AstDataUpdate" "{public}"
    add_defines "AST_BUILD_LIB_DATAUPDATE"
    add_deps "AstUtil" "AstCore"
    add_headerfiles "(**.hpp)" "AstDataUpdate"
    add_headerfiles "(**.h)" "AstDataUpdate"
target_end
