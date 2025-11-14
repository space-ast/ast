target("AstCore")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_headerfiles("**.h")
    add_headerfiles("../../(include/AstGlobal.hpp)")
    add_defines("AST_BUILD_LIB_CORE")
    
