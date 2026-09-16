#!/bin/sh
# AstSim：仿真（航天器运动、姿态、基础对象、访问者模式）

target "AstSim"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstSim" "{public}"
    add_defines "AST_BUILD_LIB_SIM"
    add_deps "AstCore" "AstUtil" "AstMath"
    add_headerfiles "(**.hpp)" "AstSim"
    add_headerfiles "(**.h)" "AstSim"
target_end
