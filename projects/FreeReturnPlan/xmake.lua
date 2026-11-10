target("FreeReturnPlan")
    set_kind("binary")
    add_files("**.cpp")
    add_includedirs(".")
    add_packages("agg")
    -- add_tests("project")
    if has_package("cminpack") and has_package("matplotplusplus") then
        add_packages("cminpack")
        add_packages("matplotplusplus")
    else
        set_enabled(false)
    end
    set_languages("c++14")
