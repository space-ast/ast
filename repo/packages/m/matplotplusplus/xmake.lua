package("matplotplusplus")
    local mpp_dir = path.join(os.scriptdir(), "../../../../thirdparty/matplotplusplus")
    if os.exists(path.join(mpp_dir, "xmake.lua")) then
        set_sourcedir(mpp_dir)
    else
        add_urls("https://github.com/space-ast/matplotplusplus.git")
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