target("MoonLandAnalysis")
    set_kind("binary")
    add_files("**.cpp")
    add_deps("AstCore", "AstAnalyzer")