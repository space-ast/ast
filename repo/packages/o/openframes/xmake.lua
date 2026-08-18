package("openframes")
    set_homepage("https://github.com/ravidavi/OpenFrames")
    set_description("Realtime interactive scientific visualization API")
    set_license("Apache 2.0")


    local of_dir = path.join(os.scriptdir(), "../../../../thirdparty/OpenFrames")
    if os.exists(path.join(of_dir, "CMakeLists.txt")) then
        set_sourcedir(of_dir)
    else
        add_urls("https://github.com/ravidavi/OpenFrames.git")
    end


    add_deps("cmake")
    add_deps("openscenegraph", {configs = {shared = true}})

    on_load(function (package)
    end)

    on_install(function (package)
        package:addenv("PATH", "bin")
        local configs = {}
        local cxflags
        import("package.tools.cmake").install(package, configs, {cxflags = cxflags})
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("OpenFrames/Export.h"))
    end)
