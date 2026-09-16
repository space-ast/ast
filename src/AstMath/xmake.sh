#!/bin/sh
# AstMath：数学库（线性代数、ODE 积分器、插值、姿态变换、非线性方程等）

target "AstMath"
    set_kind "shared"
    add_files "**.c"
    add_files "**.cpp"
    add_includedirs "../../include/AstMath" "{public}"
    add_defines "AST_BUILD_LIB_MATH"
    add_deps "AstUtil"
    add_headerfiles "(**.hpp)" "AstMath"
    add_headerfiles "(**.h)" "AstMath"
target_end
