rule("ast")
    on_load(function (target)
        if target:plat() == "wasm" then
            -- 添加wasm预加载文件
            -- 这个选项必须在on_load中设置，否则不起作用
            target:set("values", "wasm.preloadfiles", "build/wasm/data@data")
        end
    end)
    on_config(function (target)
        if target:plat() == "wasm" then
            -- import("core.cache.localcache")
            -- local preloadfiles = localcache.get("rule.ast", "wasm.preloadfiles")
            -- if not preloadfiles then
            --     local files = os.files("data/**|**plneph.405|**leDE1900.421|**leDE1941.405|**leDE18002100.424")
            --     preloadfiles = {}
            --     for _, file in ipairs(files) do
            --         table.insert(preloadfiles, file .. "@" .. file)
            --     end
            --     localcache.set("rule.ast", "wasm.preloadfiles", preloadfiles)
            --     localcache.save()
            -- end
            if is_mode("debug") or is_mode("releasedbg") then
                target:add("ldflags", "-s ASSERTIONS=1")
                target:add("cxflags", "-gsource-map")
            end
            target:add("ldflags", "-s ALLOW_MEMORY_GROWTH=1")
            target:add("shflags", "-s ALLOW_MEMORY_GROWTH=1")
            target:add("ldflags", "-s INITIAL_MEMORY=33554432")  -- This option was formerly called TOTAL_MEMORY
            target:add("ldflags", "-s TOTAL_MEMORY=33554432")    -- 为了兼容老版本的emscripten
        end
        local include_dir = path.join(os.scriptdir(), "include", target:name())
        if os.isdir(include_dir) then
            target:add("includedirs", include_dir)
        end
        if target:plat() == "windows" and target:kind() == "binary" then
            -- 添加图标资源文件
            target:add("files", path.join(os.scriptdir(), "data/icons/logo/*.rc"))
        end
    end)
    before_build(function (target)
        if target:plat() == "wasm" then
            if not os.exists("build/wasm/data") then
                os.cp("data/*|Test|Dev|.git|Config|README.md|.gitignore|.gitattributes", "build/wasm/data/")
            end
        end
    end)
    after_config(function (target)
        if target:plat() == "wasm" then
            if os.exists("build/wasm/data") then
                os.rmdir("build/wasm/data")
            end
        end
    end)
    before_clean(function (target)
        if target:plat() == "wasm" then
            if os.exists("build/wasm/data") then
                os.rmdir("build/wasm/data")
            end
        end
    end)
rule_end()

rule("ast.qt")
    on_config(function (target)
        target:add("frameworks", "QtWidgets", "QtGui", "QtCore", "QtSvg")
        target:add("qt.moc.flags", "-DAST_NAMESPACE_BEGIN=namespace ast{")
        target:add("qt.moc.flags", "-DAST_NAMESPACE_END=}")
    end)
rule_end()

rule("ast.qt.shared")
    add_deps("ast.qt", "qt.shared")
rule_end()


rule("ast.qt.widgetapp")
    add_deps("ast.qt")
    if is_plat("wasm") then
        add_deps("qt.widgetapp_static")
    else
        add_deps("qt.widgetapp")
    end
    on_load(function (target)
        -- 添加静态链接的Qt插件，必须在on_load中设置，否则不起作用
        if target:plat() == "wasm" then
            target:add("values", "qt.plugins", "QSvgPlugin")
            target:add("values", "qt.links", "qsvg")
            target:add("values", "qt.linkdirs", "plugins/imageformats")
        end
    end)
rule_end()


rule("ast.qt.ts")
    add_deps("qt.env")
    set_extensions(".ts")

    on_config(function (target)
        import("lib.detect.find_file")
        import("core.base.json")

        -- get source file
        local lupdate_argv = {"-no-obsolete", "-tr-function-alias", "tr+=_,QT_TR_NOOP+=N_,QT_TRANSLATE_NOOP+=NC_"}
        local sourcefile_ts
        local source_files = {}
        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename == "ast.qt.ts" then
                sourcefile_ts = sourcebatch.sourcefiles
            else
                if sourcebatch.sourcefiles then
                    for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                        table.insert(source_files, sourcefile)
                    end
                end
            end
        end
        if sourcefile_ts and #source_files > 0 then
            -- save source files
            source_files = table.unique(source_files)
            local json_data = {
                projectFile = "",
                sources = source_files
            }

            local json_path = path.join(target:autogendir(), "rules", "qt", "ts", "sources.json")
            json.savefile(json_path, json_data)

            table.join2(lupdate_argv, {"-project", path(json_path)})

            -- get lupdate and lrelease
            local qt = assert(target:data("qt"), "qt not found!")

            local search_dirs = {}
            if qt.bindir_host then table.insert(search_dirs, qt.bindir_host) end
            if qt.bindir then table.insert(search_dirs, qt.bindir) end
            if qt.libexecdir_host then table.insert(search_dirs, qt.libexecdir_host) end
            if qt.libexecdir then table.insert(search_dirs, qt.libexecdir) end

            local lupdate = find_file(is_host("windows") and "lupdate.exe" or "lupdate", search_dirs)
            assert(os.isexec(lupdate), "lupdate not found!")

            local lrelease = find_file(is_host("windows") and "lrelease.exe" or "lrelease", search_dirs)
            assert(os.isexec(lrelease), "lrelease not found!")

            local outputdir = target:targetdir()
            local fileconfig = target:fileconfig(sourcefile_ts)
            if fileconfig and fileconfig.prefixdir then
                if path.is_absolute(fileconfig.prefixdir) then
                    outputdir = fileconfig.prefixdir
                else
                    outputdir = path.join(target:targetdir(), fileconfig.prefixdir)
                end
            end
            for _, tsfile in ipairs(sourcefile_ts) do
                local tsargv = {}
                table.join2(tsargv, lupdate_argv)
                table.join2(tsargv, {"-ts", path(tsfile)})
                os.vrunv(lupdate, tsargv)
                local outfile = path.join(outputdir, path.basename(tsfile) .. ".qm")
                os.mkdir(outputdir)
                os.vrunv(lrelease, {path(tsfile), "-qm", path(outfile)})
            end
            -- save lrelease
            target:data_set("qt.ts.lrelease", lrelease)
        end
    end)

rule_end()
