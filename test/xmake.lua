set_group("test")

add_deps("AstCore", "AstUtil", "AstOpt")
add_packages("gtest")

local files = os.files("**/test*.c*")

for _, file in ipairs(files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    target(targetname)
        add_files(file)
        set_kind("binary")
        add_tests("test")
        -- add_extrafiles("xmake.lua")
end
