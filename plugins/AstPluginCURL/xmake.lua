-- libcurl 网络实现插件：编译为独立动态库，运行时由主程序扫描加载（不参与链接）
if has_package("libcurl") and not is_plat("wasm") then
    target("AstPluginCURL")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstUtil")
        add_packages("libcurl")
        add_defines("AST_BUILD_PLUGIN_CURL")
end
