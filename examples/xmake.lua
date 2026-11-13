set_group("examples")
set_default(false)
add_deps(
    "AstUtil", "AstCore", "AstMath", "AstSim", "AstTest",
    "AstSPICE", "AstPlot", "AstHarness", "AstAgent", "AstOpt",
    "AstLoader", "AstReport", "AstVisualization"
)

set_warnings("more")

add_packages("matplotplusplus")
add_packages("agg")
add_packages("cspice")
add_packages("openmp")
add_packages("gtest")

-- 按工程实际解析到的包自动添加 AST_WITH_* 宏
add_rules("ast.project_package_defines")

-- 添加 `examples` 和 `docs` 下面的所有cpp文件作为示例
local patterns = {
    "**/*.cpp",
    "**/*.c",
    -- "../docs/zh/**.cpp"
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

for _, file in ipairs(os.files("**/*.cxx")) do
    -- local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    local basename = path.basename(file)
    target(basename)
        add_files(file)
        set_kind("binary")
        -- add_tests("example")
        if is_plat("windows") then
            add_cxflags("/utf-8")
        end
    target_end()
end
