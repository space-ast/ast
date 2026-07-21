target("AstUiPilotBoot")
    add_rules("ast.qt.shared")
    set_kind("shared")
    add_headerfiles("**.hpp", {prefixdir="AstUiPilotBoot"})
    add_headerfiles("**.h", {prefixdir="AstUiPilotBoot"})
    add_files("**.cpp")
    add_files("**.hpp")
    add_deps("AstUiPilot")
    set_default(false)
    -- if not has_package("qt) and not is_plat("wasm") then
    --     set_enabled(false)
    -- end
