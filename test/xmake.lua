set_group("test")

add_deps("AstCore")

local files = os.files("**/test*.cpp")

for _, file in ipairs(files) do
    local targetname = file:gsub("\\", "-"):gsub("%.[^.]*$", "")
    target(targetname)
        add_files(file)
        set_kind("binary")
        add_tests("test")
end
