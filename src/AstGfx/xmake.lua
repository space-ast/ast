target("AstGfx")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_deps("AstUtil", "AstOpt", "AstCore")
    add_defines("AST_BUILD_LIB_GFX")
    add_packages("openscenegraph", "opengl")
    if is_plat("windows") then 
        add_defines("WIN32")
    end
    if not has_package("openscenegraph") then 
        set_enabled(false)
    end
    set_default(false)