-- cpp-httplib 网络实现插件：编译为独立动态库，运行时由主程序扫描加载（不参与链接）
if has_package("cpp-httplib") and not is_plat("wasm") then
    target("AstPluginCppHTTP")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstUtil")
        add_packages("cpp-httplib")
        -- 包的 exceptions=false 只作用于安装阶段，不会传递给使用方，这里显式与头文件对齐
        add_defines("AST_BUILD_PLUGIN_CPPHTTP", "CPPHTTPLIB_NO_EXCEPTIONS")
        if is_plat("windows", "mingw") then
            add_syslinks("ws2_32")
        elseif is_plat("linux") then
            add_syslinks("pthread", "dl")
        elseif is_plat("macosx", "bsd") then
            add_syslinks("pthread")
        end
end
