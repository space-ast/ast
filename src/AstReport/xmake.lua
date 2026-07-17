target("AstReport")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp", {prefixdir="AstReport"})
    add_headerfiles("**.h", {prefixdir="AstReport"})
    add_deps("AstUtil", "AstCore")
    -- @todo 最好别依赖 AstCore 模块
    add_defines("AST_BUILD_LIB_REPORT")
