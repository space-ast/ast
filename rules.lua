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
            target:add("ldflags", "-s INITIAL_MEMORY=33554432")  -- This option was formerly calledTOTAL_MEMORY
            target:add("ldflags", "-s TOTAL_MEMORY=33554432")
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
    
    after_config(function (target)
        if target:plat() == "wasm" then
            if os.exists("build/wasm/data") then
                os.rmdir("build/wasm/data")
            end
        end
    end)

    before_build(function (target)
        if target:plat() == "wasm" then
            if not os.exists("build/wasm/data") then
                os.cp("data/*|Test|Dev|.git|Config|README.md|.gitignore|.gitattributes", "build/wasm/data/")
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