target("AstChart")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.hpp", {prefixdir="AstChart"})
    add_headerfiles("**.h", {prefixdir="AstChart"})
    add_defines("AST_BUILD_LIB_CHART", "NOMINMAX")
    -- Qwt 后端已迁移到 AstPluginQwt 插件，本模块只保留兼容入口
    add_deps("AstPlot")
