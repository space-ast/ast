-- JavaScript 脚本执行器插件：依赖可选的 Duktape，未启用该包时不构建
if has_package("duktape") then
    target("AstPluginJavaScript")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstScript")
        add_packages("duktape")
        add_defines("AST_BUILD_PLUGIN_JAVASCRIPT")
end
