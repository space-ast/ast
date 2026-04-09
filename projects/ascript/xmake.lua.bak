add_requires("replxx", {optional = true})

target("ascript")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("AstScript", "AstUtil")
    if has_package("replxx") then
        add_packages("replxx")
        add_defines("AST_WITH_REPLXX")
    end
    if is_plat("windows") then 
        add_syslinks("shell32")
    end