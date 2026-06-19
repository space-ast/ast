package("qwt")
    set_homepage("https://czyt1988.github.io/QWT")
    set_description("a protocol-friendly LGPL-compliant plotting widget.")
    set_license("LGPL-2.1")

    -- add_urls("https://gitee.com/czyt1988/QWT.git", "https://github.com/czyt1988/QWT.git")
    -- add_urls("https://github.com/space-ast/qwt.git")
    -- add_versions("v7.2.2-dev", "master")
    -- add_versions("v7.2.1", "v7.2.1")
    -- add_configs("qt_sdkver", {description = "Qt SDK version to use.", default = "auto"})
    -- add_configs("system_qt", {description = "Use system Qt library.", default = false})

    set_sourcedir(path.join(os.scriptdir(), "../../../../thirdparty/QWT"))


    add_deps("cmake")
    add_deps("qt5base")
    add_includedirs("include/qwt", "include/qwt/plot", "include/qwt/plot3d", "include/qwt/classincludes")
    set_policy("package.librarydeps.strict_compatibility", true)
    -- set_policy("package.install_always", true)

    on_load(function (package)
        -- local qt_sdkver = package:config("qt_sdkver")
        -- local system_qt = package:config("system_qt")
        -- local qt_config = {}
        -- if qt_sdkver ~= "auto" then
        --     qt_config.version = qt_sdkver
        -- end
        -- qt_config.system = system_qt
        -- package:add("deps", "qt", qt_config)
        if package:config("shared") then
            package:add("defines", "QWT_DLL")
        end
    end)

    on_install(function (package)
        package:addenv("PATH", "bin")
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DQWT_CONFIG_BUILD_EXAMPLE=" .. "OFF")
        table.insert(configs, "-DQWT_CONFIG_BUILD_PLAYGROUND=" .. "OFF")
        table.insert(configs, "-DQWT_CONFIG_BUILD_STATIC_EXAMPLE=" .. "OFF")
        table.insert(configs, "-DCMAKE_C_FLAGS=$CMAKE_C_FLAGS -DGL2PSDLL_EXPORTS -DGL2PSDLL")
        -- 修复qwt在msvc debug模式编译时pdb文件的并行写入问题
        local cxflags
        if package:debug() and package:is_plat("windows") then
            print("add /FS flag for debug build")
            cxflags = "/FS"
        end
        import("package.tools.cmake").install(package, configs, {cxflags = cxflags})
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("qwt_version_info.h"))
    end)
