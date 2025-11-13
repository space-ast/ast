-- 设置工程名称
set_project("ast")

-- 设置版本号
set_version("0.0.1", {build="%Y%m%d"})

-- 编译选项：是否编译测试工程
option("with_test")
    set_default(false)
option_end()

-- 包含目录
add_includedirs("include")

-- 编译规则
add_rules("mode.debug", "mode.release")

-- 包含子目录
includes("3rdparty")
includes("src")
includes("projects")

-- 测试
if has_config("with_test") then
    includes("test")
end

-- 打包
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
