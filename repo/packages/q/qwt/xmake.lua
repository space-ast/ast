package("qwt")
    set_homepage("https://czyt1988.github.io/QWT")
    set_description("a protocol-friendly LGPL-compliant plotting widget.")
    set_license("LGPL-2.1")

    add_urls("https://gitee.com/czyt1988/QWT.git", "https://github.com/czyt1988/QWT.git")

    add_versions("v7.2.1", "v7.2.1")


    add_deps("cmake")
    add_deps("qt5base")
    add_includedirs("include/qwt", "include/qwt/plot", "include/qwt/plot3d", "include/qwt/classincludes")

    on_install(function (package)
        package:addenv("PATH", "bin")
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DQWT_CONFIG_BUILD_EXAMPLE=" .. "OFF")
        table.insert(configs, "-DQWT_CONFIG_BUILD_PLAYGROUND=" .. "OFF")
        table.insert(configs, "-DQWT_CONFIG_BUILD_STATIC_EXAMPLE=" .. "OFF")
        table.insert(configs, "-DCMAKE_C_FLAGS=$CMAKE_C_FLAGS -DGL2PSDLL_EXPORTS -DGL2PSDLL")
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("qwt_version_info.h"))
    end)
