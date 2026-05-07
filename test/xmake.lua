set_group("test")
add_deps(
    "AstCore", 
    "AstUtil", 
    "AstOpt", 
    "AstMock", 
    "AstMath", 
    "AstScript", 
    "AstWeather", 
    "AstSPICE", 
    "AstSim",
    "AstLoader",
    "AstAI",
    "AstTest"
)
add_packages("gtest")
add_packages("benchmark")

set_warnings("more")

-- 单元测试代码
if has_package("gtest") then
    local test_files1 = os.files("**/test*.cpp|Archive/**|GUI/**")
    for _, file in ipairs(test_files1) do
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("unittest")
    end
    local test_files2 = os.files("**/test*.cxx")
    for _, file in ipairs(test_files2) do
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
    end
end


-- GUI测试代码
if has_package("qt5widgets") then
    local test_files = os.files("GUI/**.cpp")
    for _, file in ipairs(test_files) do
        -- local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_rules("qt.widgetapp")
            set_values("windows.subsystem", "console")
            add_files(file)
            add_deps("AstGUI")
            add_frameworks("QtWidgets", "QtGui", "QtCore")
        target_end()
    end
end

-- 性能测试代码
if has_package("benchmark") then
    local bm_files = os.files("**/bm*.cpp|Archive/**", "**/bm*.c")
    for _, file in ipairs(bm_files) do
        -- local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_files(file)
            set_kind("binary")
            add_tests("benchmark")
            -- add_extrafiles("xmake.lua")
    end
end

-- 脚本测试代码
local asc_files = os.files("**/*.asc")
for _, file in ipairs(asc_files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    local basename = path.basename(file)
    local filepath = path.join(os.scriptdir(), file)
    target(basename)
        -- add_files(file)
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
