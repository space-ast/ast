package("qt")
    set_kind("phony")
    set_base("qtbase")

    add_versions("5.15.2", "dummy")
    add_versions("5.12.5", "dummy")
    add_versions("5.7.1", "dummy")

    on_fetch(function (package, opt)
        import("core.base.semver")
        import("detect.sdks.find_qt")

        local qt = package:data("qt")
        if qt then
            return qt
        end

        local sdkdir
        if not opt.system then
            sdkdir = package:installdir()
        end

        local qt
        local version = package:version()
        local find_opt = {force = opt.force}
        if package:config("exact_version") and version then
            find_opt.version = version:shortstr()
        end
        if not package:is_cross() then
            qt = find_qt(sdkdir, find_opt)
        else
            local host_qt = package:dep("qt-tools")
            if not host_qt then
                return
            end
            local host_qt_data = host_qt:data("qt") or find_qt(nil, {version = host_qt:version() and host_qt:version():shortstr()})
            if not host_qt_data then
                return
            end
            qt = find_qt(sdkdir, table.join(find_opt, {sdkdir_host = host_qt_data.sdkdir}))
            if qt then
                -- Avoid mistakenly identifying "host_qt" as the target SDK
                if host_qt_data.sdkdir and host_qt_data.sdkdir == qt.sdkdir then
                    return
                end
            end
        end
        if not qt then
            return
        end
        if not opt.system then
            local qtversion = semver.new(qt.sdkver)
            if version and not qtversion:eq(version) then
                if package:config("exact_version") or not qtversion:ge(version) then
                    return
                end
            end
        end
        qt.version = qt.sdkver
        package:data_set("qt", qt)
        return qt
    end)
    on_install("windows|x86", "windows|x64", "linux", "macosx", "mingw", "android", "iphoneos", function (package, opt)
        package:base():script("install")(package, opt)
    end)
