target("AstCLI")
    set_kind("binary")
    add_files("**.cpp")
    add_deps("AstAI", "AstUtil")
