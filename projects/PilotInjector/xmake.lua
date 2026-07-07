target("PilotInjector")
    set_kind("binary")
    add_files("**.cpp")
    set_default(false)
    if not is_plat("windows") then
        set_enabled(false)
    end
    add_deps("AstUtil")
    add_deps("AstUiPilotBoot", {inherit = false})