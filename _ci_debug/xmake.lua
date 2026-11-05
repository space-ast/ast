-- 临时诊断工程：复现 manylinux 容器里 cpp-httplib / libcurl 的安装失败
set_project("ci-debug-packages")
set_version("0.0.1")

add_rules("mode.release")

add_requires("cpp-httplib", {optional = true, configs = {ssl = true, exceptions = false}})
add_requires("libcurl", {optional = true, configs = {debug = is_mode("debug")}})

-- 与根 xmake.lua 中相同的修复：系统 OpenSSL < 3.0 时强制源码构建 openssl3
import("lib.detect.find_package")
import("core.base.semver")
local system_openssl = find_package("openssl")
local system_openssl_ver = system_openssl and system_openssl.version and semver.match(system_openssl.version)
if not system_openssl_ver or system_openssl_ver:lt("3.0.0") then
    add_requireconfs("**.openssl3", {system = false})
end

-- 同时验证两个包能以动态库形式链接（插件实际是 shared 目标）
target("probe-net")
    set_kind("shared")
    add_files("probe.cpp")
    add_packages("cpp-httplib", "libcurl")
    add_defines("CPPHTTPLIB_NO_EXCEPTIONS")
