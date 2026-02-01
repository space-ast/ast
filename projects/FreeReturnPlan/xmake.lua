target("FreeReturnPlan")
    set_kind("binary")
    add_files("**.cpp")
    add_includedirs(".")
    add_tests("project")
    add_packages("cminpack")
    add_packages("matplotplusplus")
    if not has_package("cminpack") or not has_package("matplotplusplus") then
        set_enabled(false)
    end

