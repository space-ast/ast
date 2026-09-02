target("AstUiAI")
    if is_plat("wasm") then
        add_rules("ast.qt.static")
    else
        add_rules("ast.qt.shared")
    end
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.hpp")
    add_headerfiles("**.hpp", {prefixdir="AstUiAI"})
    add_headerfiles("**.h", {prefixdir="AstUiAI"})
    add_deps("AstAI", "AstUtil", "AstSim")
    add_defines("AST_BUILD_LIB_UIAI")
    -- set_default(false)
    -- if not has_package("qt") and not is_plat("wasm") then
    --     set_enabled(false)
    -- end
    set_default(false)
    if not has_package("qt") and not is_plat("wasm") then
        set_enabled(false)
    end
