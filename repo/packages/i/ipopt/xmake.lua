package("ipopt")
    set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/Ipopt"))
    add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})
    on_install(function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        elseif not package:is_plat("windows", "mingw") and package:config("pic") ~= false then
            configs.cxflags = "-fPIC"
        end
        import("package.tools.xmake").install(package, configs)
    end)