target("AstWeather")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.c")
    add_deps("AstUtil", "AstMath")
    add_headerfiles("**.hpp", {prefixdir="AstWeather"})
    add_headerfiles("**.h", {prefixdir="AstWeather"})
    add_defines("AST_BUILD_LIB_WEATHER")
    if has_package("libf2c") then
        add_defines("AST_WITH_LIBF2C")
        add_packages("libf2c")
    end
    add_rules("c++.unity_build", {batchsize=0}) -- 关闭unity build，防止宏冲突

includes("*/xmake.lua")