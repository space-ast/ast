#!/bin/sh
# AstFault：故障诊断与崩溃报告

target "AstFault"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstFault" "{public}"
    add_defines "AST_BUILD_LIB_FAULT"
    add_deps "AstUtil"
    add_headerfiles "(**.hpp)" "AstFault"
    add_headerfiles "(**.h)" "AstFault"
    if is_plat "windows" "mingw"; then
        add_links "user32"                 # MessageBoxA
        add_links "dbghelp"                # SymInitialize, SymFromAddr, ...
        add_links "psapi"                  # EnumProcessModules, GetModuleInformation, ...
    fi
target_end
