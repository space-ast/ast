target("AstUiUtil")
    if is_plat("wasm") then
        add_rules("ast.qt.static")
    else
        add_rules("ast.qt.shared")
    end
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstUiUtil"})
    add_headerfiles("**.h", {prefixdir="AstUiUtil"})
    add_deps("AstUtil")
    add_defines("AST_BUILD_LIB_UIUTIL")
    set_default(false)
    if not has_package("qt") and not is_plat("wasm") then
        set_enabled(false)
    end
