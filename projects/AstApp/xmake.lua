target("AstApp")
    add_rules("qt.widgetapp")
    add_files("**.cpp")
    add_files("**.hpp")
    add_files("**.ui")
    add_deps("AstGui")
    
