-- cpp-httplib 网络实现插件：编译为独立动态库，运行时由主程序扫描加载（不参与链接）
if has_package("cpp-httplib") and not is_plat("wasm") then
    target("AstPluginCppHTTP")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstUtil")
        add_packages("cpp-httplib")
        set_symbols("hidden")
        -- httplib 是头文件库：Client 所在的 TU 会顺带生成 ThreadPool、WebSocket 等用不到的 inline 代码，
        -- 它们内部构造 std::thread，把插件的 libstdc++ 需求从 GLIBCXX_3.4.21 抬到 3.4.22（老发行版上插件直接加载失败）
        -- hidden 让这些 inline 符号不再是 GC 根，再按函数分段并开启 gc-sections，
        -- 链接器就能把没人引用的部分连同其中的 thread 引用一起裁掉
        if not is_plat("windows") then
            add_cxflags("-ffunction-sections", "-fdata-sections")
            add_shflags("-Wl,--gc-sections")
        end
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
