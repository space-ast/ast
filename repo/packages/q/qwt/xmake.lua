package("qwt")
    set_homepage("https://czyt1988.github.io/QWT")
    set_description("a protocol-friendly LGPL-compliant plotting widget.")
    set_license("LGPL-2.1")

    local qwt_dir = path.join(os.scriptdir(), "../../../../thirdparty/QWT")
    if os.exists(path.join(qwt_dir, "src")) then
        set_sourcedir(qwt_dir)
    else
        add_urls("https://github.com/space-ast/QWT.git")
    end


    -- add_deps("cmake")
    add_deps("qt")
    add_includedirs("include/qwt", "include/qwt/plot", "include/qwt/plot3d", "include/qwt/classincludes")
    set_policy("package.librarydeps.strict_compatibility", true)
    -- set_policy("package.install_always", true)

    add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})

    on_load(function (package)
        -- [OLD CONFIG - dynamic Qt dep] --
        -- local qt_sdkver = package:config("qt_sdkver")
        -- local system_qt = package:config("system_qt")
        -- local qt_config = {}
        -- if qt_sdkver ~= "auto" then
        --     qt_config.version = qt_sdkver
        -- end
        -- qt_config.system = system_qt
        -- package:add("deps", "qt", qt_config)
        -- [END OLD CONFIG - dynamic Qt dep] --
        if package:config("shared") then
            package:add("defines", "QWT_DLL", "QWT3D_DLL")
        end
    end)

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
        end
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("qwt_version_info.h"))
    end)
