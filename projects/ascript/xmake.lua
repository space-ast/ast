add_requires("replxx", {optional = true})

target("ascript")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("AstScript")
    if has_package("replxx") then
        add_packages("replxx")
        add_defines("AST_WITH_REPLXX")
    end