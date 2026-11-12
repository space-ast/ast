package("openvbs")
    local openvbs_dir = path.join(os.scriptdir(), "../../../../thirdparty/openvbs")
    if os.exists(path.join(openvbs_dir, "xmake.lua")) then
        set_sourcedir(openvbs_dir)
    else
        add_urls("https://gitcode.com/space-ast/openvbs.git")
        add_urls("https://github.com/space-ast/openvbs.git")
    end
    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})
    on_install(function (package)
        local configs = {}
        configs.kind = package:config("shared") and "shared" or "static"
        import("package.tools.xmake").install(package, configs)
    end)
