-- 临时诊断工程：复现 manylinux 容器里 cpp-httplib / libcurl 的安装失败
set_project("ci-debug-packages")
set_version("0.0.1")

add_rules("mode.release")

add_requires("cpp-httplib", {optional = true, configs = {ssl = true, exceptions = false}})
add_requires("libcurl", {optional = true, configs = {debug = is_mode("debug")}})
