#!/bin/sh
# AstCore：动力学核心（轨道、传播器、坐标系、时间系统、力模型、太阳系）

target "AstCore"
    set_kind "shared"
    add_files "**.cpp"
    add_files "**.c"
    add_includedirs "../../include/AstCore" "{public}"
    add_defines "AST_BUILD_LIB_CORE"
    add_deps "AstUtil" "AstMath" "AstWeather" "AstScript"
    add_headerfiles "(**.hpp)" "AstCore"
    add_headerfiles "(**.h)" "AstCore"
target_end
