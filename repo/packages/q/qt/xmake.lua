package("qt")
    set_kind("phony")
    set_base("qtbase")
    
    add_versions("5.15.2", "dummy")
    add_versions("5.12.5", "dummy")
    add_versions("5.7.1", "dummy")
    add_versions("5.7.0", "dummy")

    add_versions("6.3.0", "dummy")
    add_versions("6.3.1", "dummy")
    add_versions("6.3.2", "dummy")
    add_versions("6.4.0", "dummy")
    add_versions("6.4.1", "dummy")
    add_versions("6.4.2", "dummy")
    add_versions("6.4.3", "dummy")
    add_versions("6.5.0", "dummy")
    add_versions("6.5.1", "dummy")
    add_versions("6.5.2", "dummy")
    add_versions("6.5.3", "dummy")
    add_versions("6.6.0", "dummy")
    add_versions("6.6.1", "dummy")
    add_versions("6.6.2", "dummy")
    add_versions("6.6.3", "dummy")
    add_versions("6.7.0", "dummy")
    add_versions("6.7.1", "dummy")
    add_versions("6.7.2", "dummy")
    add_versions("6.8.0", "dummy")
    add_versions("6.8.1", "dummy")
    add_versions("6.8.2", "dummy")
    add_versions("6.8.3", "dummy")
    add_versions("6.9.0", "dummy")
    add_versions("6.9.1", "dummy")


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

        local qtversion = semver.new(qt.sdkver)
        if version and not qtversion:eq(version) then
            -- 这里将版本检测条件放松了
            if package:config("exact_version") then
                return
            end
        end
        qt.version = qt.sdkver
        package:data_set("qt", qt)
        return qt
    end)