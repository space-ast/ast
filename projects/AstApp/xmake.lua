target("AstApp")
    add_rules("qt.widgetapp")
    add_files("**.cpp")
    add_files("**.hpp")
    add_files("**.ui")
    add_deps("AstGui")
    if not has_package("qt5widgets") then
        set_enabled(false)
    end
