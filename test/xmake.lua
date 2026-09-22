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
    "AstCmd",
    "AstTest",
    "AstFault",
    "AstEngine"
)

if is_plat("windows") and has_config("with_com") then
    add_defines("AST_WITH_COM")
    add_deps("AstCOM")
end

if is_plat("windows") then
    add_cxflags("/utf-8")
end

add_packages("gtest")
add_packages("benchmark")

set_warnings("more")

-- 单元测试代码（如果没有gtest库，则使用内置测试框架 AstTest/TestFramework.hpp）
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


-- GUI测试代码
if has_package("qt") then
    local test_files = os.files("GUI/**.cpp")
    for _, file in ipairs(test_files) do
        -- local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
        local basename = path.basename(file)
        target(basename)
            add_rules("qt.widgetapp")
            set_values("windows.subsystem", "console")
            add_files(file)
            add_deps("AstUiCore", "AstChart")
            add_frameworks("QtWidgets", "QtGui", "QtCore")
            add_packages("qwt")
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
        local run = function(target, opt)
            import("core.project.project")
            import("private.action.run.runenvs")

            local target = project.target("ascript")
            if target then
                local outfile
                local errfile
                -- opt 为 nil 说明是 run，否则是 test，需要捕获输出和错误
                if opt then
                    outfile = os.tmpfile()
                    errfile = os.tmpfile()
                end
                local targetfile = target:targetfile()
                local runargs = {filepath}
                local addenvs, setenvs = runenvs.make(target)
                local ok, syserrors = os.execv(targetfile, runargs, {
                    try = opt ~= nil, curdir = target:rundir(),
                    envs = runenvs.join(addenvs, setenvs),
                    stdout = outfile, stderr = errfile
                })
                if opt then
                    local outdata = os.isfile(outfile) and io.readfile(outfile) or ""
                    local errdata = os.isfile(errfile) and io.readfile(errfile) or ""
                    os.tryrm(outfile)
                    os.tryrm(errfile)
                    if #outdata > 0 then
                        opt.stdout = outdata
                    end
                    if #errdata > 0 then
                        opt.stderr = errdata
                    end
                end
                return ok == 0
            end
        end
        on_test(run)
        on_run(run)
end
