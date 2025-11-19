-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.0.1", {build="%Y%m%d"})

-- 编译选项：是否编译测试工程
option("with_test")
    set_default(false)
option_end()

-- 设置c代码标准：c99， c++代码标准：c++11
set_languages("cxx11", "c99")

-- 添加包含目录
add_includedirs("include")

-- 添加编译规则
add_rules("mode.debug", "mode.release")

if is_plat("linux") then
    add_rpathdirs("$ORIGIN")
end

-- 导入子目录配置
includes("3rdparty")
includes("src")
includes("projects")

-- 导入测试配置
if has_config("with_test") then
    includes("test")
end

-- 导入打包配置
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
