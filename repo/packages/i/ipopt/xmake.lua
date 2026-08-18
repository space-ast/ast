package("ipopt")

    add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})
    add_includedirs("include", "include/coin-or")

    local ipopt_dir = path.join(os.scriptdir(), "../../../../thirdparty/Ipopt")
    if os.exists(path.join(ipopt_dir, "xmake.lua")) then
        set_sourcedir(ipopt_dir)
    else
        add_urls("https://github.com/space-ast/Ipopt.git")
        add_versions("v3.14", "stable/3.14")
    end

    on_install(function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        elseif not package:is_plat("windows", "mingw") and package:config("pic") ~= false then
            configs.cxflags = "-fPIC"
        end
        import("package.tools.xmake").install(package, configs)
    end)