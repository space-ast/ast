target("AstUiDataUpdate")
    add_rules("ast.qt.shared")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstUiDataUpdate"})
    add_headerfiles("**.h", {prefixdir="AstUiDataUpdate"})
    add_deps("AstDataUpdate")
    add_defines("AST_BUILD_LIB_UIDATAUPDATE")
    set_default(false)
    if not has_package("qt") then
        set_enabled(false)
    end