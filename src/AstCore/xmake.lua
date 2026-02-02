target("AstCore")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.c")
    add_headerfiles("**.hpp")
    add_headerfiles("**.h")
    add_headerfiles("../../(include/AstGlobal.h)")
    add_defines("AST_BUILD_LIB_CORE")
    -- add_extrafiles("xmake.lua")    
    add_deps("AstUtil", "AstMath")
    if has_package("libf2c") then
        add_defines("AST_WITH_LIBF2C")
        add_packages("libf2c")
    end

includes("Atmosphere")