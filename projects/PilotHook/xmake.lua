target("PilotHook")
    set_kind("binary")
    add_files("**.cpp")
    add_links("user32")
    set_default(false)
    if not is_plat("windows") then
        set_enabled(false)
    end
