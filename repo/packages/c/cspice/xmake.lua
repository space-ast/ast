package("cspice")
    local cspice_dir
    local cspice_branch
    if is_plat("windows", "mingw") then
        cspice_dir = path.join(os.scriptdir(), "../../../../thirdparty/cspice-windows")
        cspice_branch = "windows"
    elseif is_plat("linux") then
        cspice_dir = path.join(os.scriptdir(), "../../../../thirdparty/cspice-linux")
        cspice_branch = "linux"
    else
        cspice_dir = path.join(os.scriptdir(), "../../../../thirdparty/cspice")
        cspice_branch = "master"
    end
    if os.exists(path.join(cspice_dir, "xmake.lua")) then
        set_sourcedir(cspice_dir)
    else
        add_urls("https://github.com/space-ast/cspice.git")
        add_versions("v1.0", cspice_branch)
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