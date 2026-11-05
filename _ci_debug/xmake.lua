-- 临时诊断工程：复现 manylinux 容器里 cpp-httplib / libcurl 的安装失败
set_project("ci-debug-packages")
set_version("0.0.1")

add_rules("mode.release")

add_requires("cpp-httplib", {optional = true, configs = {ssl = true, exceptions = false}})
add_requires("libcurl", {optional = true, configs = {debug = is_mode("debug")}})

-- 与根 xmake.lua 中相同的修复：系统没有 OpenSSL 3 时强制源码构建 openssl3
local function has_system_openssl3()
    local candidates = {
        "/usr/lib64/libssl.so.3",
        "/usr/lib/libssl.so.3",
        "/usr/lib/x86_64-linux-gnu/libssl.so.3",
        "/usr/lib/aarch64-linux-gnu/libssl.so.3",
        "/usr/local/lib64/libssl.so.3",
        "/usr/local/lib/libssl.so.3",
    }
    for _, filepath in ipairs(candidates) do
        if os.isfile(filepath) then
            return true
        end
    end
    return false
end

if is_plat("linux") and not has_system_openssl3() then
    add_requireconfs("**.openssl3", {system = false})
end

-- 同时验证两个包能以动态库形式链接（插件实际是 shared 目标）
target("probe-net")
    set_kind("shared")
    add_files("probe.cpp", "glibc_compat.c")
    add_packages("cpp-httplib", "libcurl")
    add_defines("CPPHTTPLIB_NO_EXCEPTIONS")
