-- VBScript 脚本执行器插件：依赖可选的 openvbs 包，未启用该包时不构建
if has_package("openvbs") then
    target("AstPluginOpenVBS")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstScript")
        add_packages("openvbs")
        add_defines("AST_BUILD_PLUGIN_OPENVBS")
        if is_plat("windows") then
            add_syslinks("oleaut32", "ole32", "uuid")
        elseif is_plat("linux", "macosx", "bsd") then
            add_syslinks("dl")
        end
        if not is_plat("windows", "mingw") then
            add_cxflags("-fPIC")
        end
end
