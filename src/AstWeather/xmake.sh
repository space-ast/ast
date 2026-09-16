#!/bin/sh
# AstWeather：空间天气模型（大气、磁场、辐射等）

target "AstWeather"
    set_kind "shared"
    add_files "**.cpp"
    add_files "Atmosphere/nrlmsise-00/*.c"
    add_includedirs "../../include/AstWeather" "{public}"
    add_defines "AST_BUILD_LIB_WEATHER"
    add_deps "AstUtil" "AstMath"
    add_headerfiles "(**.hpp)" "AstWeather"
    add_headerfiles "(**.h)" "AstWeather"
    # f2c 转换出来的 Fortran 代码需要 libf2c 运行库
    if has_config "libf2c"; then
        add_options "libf2c"
        add_defines "AST_WITH_LIBF2C"
    fi
target_end
