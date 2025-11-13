set_project("ast")

-- 是否编译测试工程
option("with_test")
    set_default(false)
option_end()

add_includedirs("include")
add_rules("mode.debug", "mode.release")

includes("3rdparty")
includes("src")
includes("projects")

-- 测试
if has_config("with_test") then
    includes("test")
end

-- add xpack
includes("@builtin/xpack")
if xpack then
    includes("xpack.lua")
end
