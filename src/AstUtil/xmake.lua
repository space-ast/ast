target("AstUtil")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.cxx")
    add_files("Platform/IO.cpp", {unity_group="IO"})
    add_headerfiles("**.hpp", {prefixdir="AstUtil"})
    add_headerfiles("**.h", {prefixdir="AstUtil"})
    -- 下面的全局头文件由AstUtil工程在发布时复制到include目录下 
    add_headerfiles("../../include/ast/*", {prefixdir="ast"})
    add_headerfiles("../../include/AstGlobal.h")
    add_headerfiles("../../include/AstCompiler.h")
    add_defines("AST_BUILD_LIB_UTIL")
    -- add_extrafiles("xmake.lua")   
    if is_plat("linux") then
        add_syslinks("dl")
    end
    if is_plat("windows", "mingw") then
        add_syslinks("ole32", "oleaut32", "uuid", "shell32", "user32", "shlwapi")
    end

