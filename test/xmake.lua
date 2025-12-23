set_group("test")
add_deps("AstCore", "AstUtil", "AstOpt", "AstMock", "AstMath", "AstScript")
add_packages("gtest")
add_packages("benchmark")
add_packages("eigen")

if has_package("gtest") then
    local test_files = os.files("**/test*.c*")
    for _, file in ipairs(test_files) do
        local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("unittest")
            -- add_extrafiles("xmake.lua")
    end
end

if has_package("benchmark") then
    local bm_files = os.files("**/bm*.c*")
    for _, file in ipairs(bm_files) do
        local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("benchmark")
            -- add_extrafiles("xmake.lua")
    end
end
