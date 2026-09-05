target("AstUiPilot")
    add_rules("ast.qt.shared")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstUiPilot"})
    add_headerfiles("**.h", {prefixdir="AstUiPilot"})
    add_deps("AstAI", "AstUtil", "AstUiUtil")
    add_defines("AST_BUILD_LIB_UIPILOT")
    set_default(false)
    if not has_package("qt") and not is_plat("wasm") then
        set_enabled(false)
    end
