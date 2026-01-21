set_group("examples")
set_default(false)
add_deps("AstUtil", "AstCore", "AstMath")

local files = os.files("**/*.c*")
    for _, file in ipairs(files) do
        local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("example")
            if is_plat("windows") then
                add_cxflags("/utf-8")
            end
    end
