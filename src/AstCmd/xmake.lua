target("AstCmd")
    set_kind("shared")
    add_headerfiles("**.hpp", {prefixdir="AstCmd"})
    add_headerfiles("**.h", {prefixdir="AstCmd"})
    add_files("**.cpp")
    add_deps("AstUtil", "AstCore", "AstSim")
    add_defines("AST_BUILD_LIB_CMD")

