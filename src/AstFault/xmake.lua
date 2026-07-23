target("AstFault")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp", {prefixdir="AstFault"})
    add_headerfiles("**.h", {prefixdir="AstFault"})
    add_deps("AstUtil")
    add_defines("AST_BUILD_LIB_FAULT")
    if is_plat("windows", "mingw") then
        add_links("user32")   -- MessageBoxA
        add_links("dbghelp")  -- SymInitialize, SymFromAddr, etc.
        add_links("psapi")    -- EnumProcessModules, GetModuleInformation, etc.
    end
