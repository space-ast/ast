package("qt-advanced-docking")
    set_homepage("https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System")
    set_description("Advanced Docking System for Qt")
    set_license("LGPL-2.1")

    local ads_dir = path.join(os.scriptdir(), "../../../../thirdparty/Qt-Advanced-Docking-System")
    if os.exists(path.join(ads_dir, "src")) then
        set_sourcedir(ads_dir)
    else
        add_urls("https://github.com/space-ast/Qt-Advanced-Docking-System.git")
    end

    add_deps("qt")
    add_includedirs("include/qtadvanceddocking-qt5")
    add_includedirs("include/qt-advanced-docking")
    

    on_install(function (package)
        package:addenv("PATH", "bin")
        if os.exists(path.join(package:sourcedir(), "xmake.lua")) then
            local configs = {}
            configs.mode = package:is_debug() and "debug" or "release"
            import("package.tools.xmake").install(package, configs)
        else
            local configs = {}
            table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
            table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
            table.insert(configs, "-DBUILD_EXAMPLES=" .. "OFF")
            table.insert(configs, "-DBUILD_TESTING=" .. "OFF")
            table.insert(configs, "-DADS_VERSION=" .. "4.5.0")
            import("package.tools.cmake").install(package, configs)
        end
    end)

    on_test(function (package)
        -- assert(package:has_cxxincludes("DockManager.h"))
    end)
