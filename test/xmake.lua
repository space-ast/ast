set_group("test")

add_deps("AstCore", "AstUtil", "AstOpt")
add_packages("gtest")
add_packages("benchmark")

local test_files = os.files("**/test*.c*")

for _, file in ipairs(test_files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    target(targetname)
        add_files(file)
        set_kind("binary")
        add_tests("test")
        -- add_extrafiles("xmake.lua")
end

local bm_files = os.files("**/bm*.c*")
for _, file in ipairs(bm_files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    target(targetname)
        add_files(file)
        set_kind("binary")
        add_tests("benchmark")
        -- add_extrafiles("xmake.lua")
end

local ex_files = os.files("**/ex*.c*")
for _, file in ipairs(ex_files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    target(targetname)
        add_files(file)
        set_kind("binary")
        add_tests("example")
        -- add_extrafiles("xmake.lua")
end