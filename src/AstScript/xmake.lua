target("AstScript")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.cxx")
    add_headerfiles("**.hpp", {prefixdir="AstScript"})
    add_defines("AST_BUILD_LIB_SCRIPT")
    -- add_extrafiles("xmake.lua")    
    add_deps("AstUtil")
    if is_plat("windows", "mingw") then
        add_syslinks("oleaut32", "ole32", "uuid")
    end
