set_group("test")
add_deps("AstCore", "AstUtil", "AstOpt", "AstMock", "AstMath", "AstScript")
add_packages("gtest")
add_packages("benchmark")


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
    local bm_files = os.files("**/bm*.cpp", "**/bm*.c")
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

local asc_files = os.files("**/*.asc")
for _, file in ipairs(asc_files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    local basename = path.basename(file)
    local filepath = path.join(os.scriptdir(), file)
    target(basename)
        add_files(file)
        set_kind("phony")
        add_deps("ascript")
        add_tests("script")
        local run = function(target)
            import("core.project.project")
            import("core.base.option")
            local target = project.target("ascript")
            if target then
                -- print(target:name())
                -- print(target:targetfile())
                local targetfile = target:targetfile()
                local runargs = {filepath}
                local ok, syserrors
                if option.get("verbose") then
                    ok, syserrors = os.execv(targetfile, runargs, {try = true})
                else
                    local testdir = "./build/testoutput/"
                    if not os.exists(testdir) then
                        os.mkdir(testdir)
                    end
                    ok, syserrors = os.execv(targetfile, runargs, {try = true, stdout = testdir .. basename})
                end
                -- print(ok, syserrors)
                local passed = ok == 0
                return passed
            end
            -- import("core.base.task")
            -- task.run("run", {target="ascript", arguments={filepath}})
        end
        on_test(run)
        on_run(run)
        -- add_extrafiles("xmake.lua")
end
