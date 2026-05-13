set_group("examples")
set_default(false)
add_deps("AstUtil", "AstCore", "AstMath", "AstSim", "AstTest", "AstSPICE")
set_warnings("more")

-- 添加 `examples` 和 `docs` 下面的所有cpp文件作为示例
local patterns = {
        "**/*.c*",
        "../docs/guide/**.cpp",  
        "../docs/examples/**.cpp",  
        "../docs/dev/**.cpp",
        "../docs/technical/**.cpp"
}

for _, pattern in ipairs(patterns) do
    local files = os.files(pattern)
    for _, file in ipairs(files) do
        -- local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("example")
            if is_plat("windows") then
                add_cxflags("/utf-8")
            end
        target_end()
    end
end

