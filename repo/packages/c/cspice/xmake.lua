package("cspice")
    set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/cspice"))
    on_install(function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        elseif not package:is_plat("windows", "mingw") and package:config("pic") ~= false then
            configs.cxflags = "-fPIC"
        end
        import("package.tools.xmake").install(package, configs)
    end)