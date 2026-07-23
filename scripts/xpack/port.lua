-- examples 示例代码跨平台编译配置
-- 首先安装c++编译器和xmake(https://xmake.io/zh/guide/quick-start.html)
-- 然后执行以下命令:
-- xmake
-- xmake test

add_rules("mode.release", "mode.debug")         -- release 模式和 debug 模式
set_targetdir("bin")                            -- 输出目录
add_includedirs("include")                      -- 头文件目录
add_linkdirs("lib")                             -- 链接库目录

-- 链接库
if is_plat("windows") and is_mode("debug") then
    add_links(
        "AstAID", "AstAnalyzerD", "AstMockD",
        "AstSPICED", "AstSimD", "AstOptD",
        "AstCoreD", "AstMathD", "AstUtilD"
    )
else
    add_links(
        "AstAID", "AstAnalyzer", "AstMock",
        "AstSPICE", "AstSim", "AstOpt",
        "AstCore", "AstMath", "AstUtil"
    )
end


-- 遍历添加示例工程
local files = os.files("examples/**/*.c*")
for _, file in ipairs(files) do
    -- 提取文件名，作为目标名称
    local basename = path.basename(file)
    target(basename)
        -- 添加源文件
        add_files(file)
        -- 设置为二进制目标
        set_kind("binary")
        -- 添加到测试组
        add_tests("example")
        -- 如果是 Windows 平台，添加 /utf-8 编码选项
        if is_plat("windows") then
            add_cxflags("/utf-8")
        end
    target_end()
end

