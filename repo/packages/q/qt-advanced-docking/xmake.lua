package("qt-advanced-docking")
    set_homepage("https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System")
    set_description("Advanced Docking System for Qt")
    set_license("LGPL-2.1")

    set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/Qt-Advanced-Docking-System"))

    add_deps("qt5base", "qt5widgets")
    add_includedirs("include/qtadvanceddocking-qt5")
    
    on_load(function (package)
        if package:config("shared") then
            package:add("defines", "ADS_SHARED_EXPORT")
        end
    end)

    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_EXAMPLES=" .. "OFF")
        table.insert(configs, "-DBUILD_TESTING=" .. "OFF")
        table.insert(configs, "-DADS_VERSION=" .. "4.5.0")
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("DockManager.h"))
    end)
