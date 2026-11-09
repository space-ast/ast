
target("ascript")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("AstScript", "AstUtil")
    add_packages("replxx")
    if is_plat("windows") then 
        add_syslinks("shell32")
    end