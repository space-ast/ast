package("libf2c")
    local libf2c_dir = path.join(os.scriptdir(), "../../../../thirdparty/libf2c")
    if os.exists(path.join(libf2c_dir, "xmake.lua")) then
        set_sourcedir(libf2c_dir)
    else
        add_urls("https://github.com/space-ast/libf2c.git")
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