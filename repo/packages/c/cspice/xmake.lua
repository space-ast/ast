package("cspice")
    if is_plat("windows", "mingw") then
        set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/cspice-windows"))
    elseif is_plat("linux") then
        set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/cspice-linux"))
    else
        set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/cspice"))
    end
    add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})

    on_install(function (package)
        package:addenv("PATH", "bin")
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        elseif not package:is_plat("windows", "mingw") and package:config("pic") ~= false then
            configs.cxflags = "-fPIC"
        end
        import("package.tools.xmake").install(package, configs)
    end)