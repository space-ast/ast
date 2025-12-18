target("TestOsg")
    set_kind("binary")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_deps("AstGfx")
    add_packages("openscenegraph", "opengl")
    if is_plat("windows") then 
        add_defines("WIN32")
    end
