target("AstUtil")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_headerfiles("**.h")
    add_headerfiles("../../(include/AstGlobal.h)")
    add_defines("AST_BUILD_LIB_UTIL")
    -- add_extrafiles("xmake.lua")    
    if has_package("fmt") then
        add_packages("fmt")
        add_defines("AST_WITH_FMT")
    end