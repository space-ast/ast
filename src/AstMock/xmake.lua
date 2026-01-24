target("AstMock")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.c")
    add_headerfiles("**.hpp")
    add_headerfiles("**.h")
    add_headerfiles("../../(include/AstGlobal.h)")
    add_defines("AST_BUILD_LIB_MOCK")
    add_deps("AstUtil", "AstMath", "AstCore")
    
    if is_plat("windows") then
        add_rules("utils.symbols.export_list", {
            symbols = {
                "snopta_", "sninit_",  "snset_", 
                "sngeti_", "sngetr_", "sngetc_", 
                "snseti_", "snsetr_",
                "snspec_", "snmema_", "snjac_"
            },
        })
    end
    -- add_extrafiles("xmake.lua")    
