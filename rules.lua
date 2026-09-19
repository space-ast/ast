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
        elseif target:plat() == "windows" then
            -- 去除 __FILE__ 宏中的项目目录前缀
            -- 但是这个编译选项容易导致部分调试器无法正常工作
            -- target:add("cxflags", "/d1trimfile:" .. os.projectdir() .. "\\")
        end
        local include_dir = path.join(os.scriptdir(), "include", target:name())
        if os.isdir(include_dir) then
            target:add("includedirs", include_dir)
        end
        if target:kind() == "binary" then
            if target:plat() == "windows" then
                -- 添加图标资源文件
                target:add("files", path.join(os.scriptdir(), "data/icons/logo/*.rc"))
            elseif target:plat() == "linux" then
                target:add("syslinks", "dl", "pthread")
            end
        end
    end)
    before_build(function (target)
        if target:plat() == "wasm" then
            -- 在after_config里会删除原来的data目录
            if not os.exists("build/wasm/data") then
                os.cp("data/*|Test|Dev|.git|Config|README.md|.gitignore|.gitattributes", "build/wasm/data/")
            end
        end
    end)
    if after_config then
        after_config(function (target)
            -- 在 after_config 里显式设置 runpath，覆盖 qt rules 里隐式添加的 rpathdirs
            target:set("rpathdirs", "$ORIGIN", "$ORIGIN/../lib")

            if target:plat() == "wasm" then
                import("core.project.config")
                if os.exists("build/wasm/data") then
                    os.rmdir("build/wasm/data")
                end
                -- Qt5-wasm SDK 只有一个 wasm 资源 `qmake_wasmfonts`；xmake 内置规则会在
                -- 生成的 static_import.cpp 里硬编码 Qt6 的资源名 `wasmwindow`/`wasmfonts`，
                -- 造成链接时未定义符号 qInitResources_wasmfonts / qInitResources_wasmwindow。
                -- 这里改写成 Qt5.15 实际的资源名（插件自身会引用 qInitResources_qmake_wasmfonts）。
                local importfile = path.join(config.builddir(), ".qt", "plugin", target:name(), "static_import.cpp")
                if os.isfile(importfile) then
                    local qt = target:data("qt")
                    local sdkver = qt and qt.sdkver
                    if sdkver and sdkver:startswith("5.") then
                        local content = io.readfile(importfile)
                        if content then
                            content = content:gsub("Q_INIT_RESOURCE%(%s*wasmwindow%s*%)%s*;", "")
                            content = content:gsub("Q_INIT_RESOURCE%(%s*wasmfonts%s*%)%s*;", "Q_INIT_RESOURCE(qmake_wasmfonts);")
                            io.writefile(importfile, content)
                        end
                    end
                end
            end
        end)
    end
    before_clean(function (target)
        if target:plat() == "wasm" then
            if os.exists("build/wasm/data") then
                os.rmdir("build/wasm/data")
            end
        end
    end)
rule_end()

rule("ast.qt")
    add_deps("qt.env")
    add_deps("qt.ui", "qt.moc", "qt.qrc", "qt.ts")
    on_config(function (target)
        -- 注意：Qt 缺失不能在这里处理。on_config 只对已经启用的目标执行，此时目标早已
        -- 进了 project.targets()，在这里把它改成 phony 只会让目标留在 xpack 的打包列表
        -- 里、贡献头文件却不产出库（os.cp 找不到产物是静默失败），既不报错也装不出东西。
        -- 必须在 target 体内 `if not has_package("qt") then set_enabled(false) end`，
        -- 只有这样才能让它被 project.targets() 过滤掉。
        -- 参见 src/AstUi*/xmake.lua 与 projects/App*/xmake.lua。
        target:add(
            "frameworks", 
            "QtWidgets", "QtGui", "QtCore", "QtSvg", "QtTest", 
            "QtOpenGL", "QtPrintSupport"
        )
        if target:plat() ~= "wasm" then
            target:add("frameworks", "QtConcurrent")
        end
        target:add("qt.moc.flags", "-DAST_NAMESPACE_BEGIN=namespace ast{")
        target:add("qt.moc.flags", "-DAST_NAMESPACE_END=}")
    end)
rule_end()

rule("ast.qt.shared")
    add_deps("ast.qt", "qt.shared")
rule_end()

rule("ast.qt.static")
    add_deps("ast.qt", "qt.static")
rule_end()

rule("ast.qt.widgetapp")
    add_deps("ast.qt")
    on_load(function (target)
        -- 与内置 qt.widgetapp 一致：桌面平台是 GUI 可执行程序，android 上是共享库。
        -- 必须在 on_load 中设置，因为规则要早于 target 的其它配置确定 kind。
        target:set("kind", target:is_plat("android") and "shared" or "binary")
        -- 添加静态链接的Qt插件，必须在on_load中设置，否则不起作用
        if target:plat() == "wasm" then
            target:add("values", "qt.plugins", "QSvgPlugin")
            target:add("values", "qt.links", "qsvg")
            target:add("values", "qt.linkdirs", "plugins/imageformats")
        end
    end)
    if is_plat("wasm") then
        -- wasm 是静态链接，仍然复用内置规则（它的 installcmd 在非 windows 平台本来就装到 bin）
        add_deps("qt.widgetapp_static")
    else
        on_config(function (target)
            -- 与内置 qt.widgetapp 的 on_config 一致：
            import("rules.qt.load", {rootdir = os.programdir()})(target, {gui = true})
            -- 这两项 windeployqt 默认都会部署，但对本项目是多余的：
            --   --compiler-runtime      MSVC 工具链下会从 %VCToolsRedistDir% 拷一份 18MB 的
            --                           vc_redist.x64.exe，改由使用方自行确保目标机装过运行库
            --   --system-d3d-compiler   系统自带 D3Dcompiler_47.dll，Windows 7 之后无需分发
            -- xmake 会把这里的值原样透传给 windeployqt。
            target:add("values", "qt.deploy.flags", "--no-compiler-runtime", "--no-system-d3d-compiler")
        end)

        -- 安装到安装包：windows/mingw 下用 windeployqt 部署并装到 bin；其余平台复制构建输出目录到 bin、共享库到 lib。
        -- 与内置 qt.widgetapp 的 installcmd 的唯一区别就是 windows 下不装到包根目录。
        --
        -- 注意这里只是"生成"安装命令，windeployqt 必须在此时同步跑完（deploydir 位于target:autogendir() 下），
        -- 随后的 cp/rm 由 pack 统一执行。
        on_installcmd(function (target, batchcmds, opt)
            local package = opt.package
            if not package then
                return
            end
            import("plugins.pack.batchcmds", {alias = "pack_batchcmds", rootdir = os.programdir()})
            local bindir = package:bindir()

            if target:is_plat("windows", "mingw") then
                import("rules.qt.install.windeployqt", {rootdir = os.programdir()})

                local deploydir = path.join(target:autogendir(), "qt", "deploy", target:name())
                os.mkdir(deploydir)

                -- 先把可执行文件和 qt.shared 依赖拷进来，再交给 windeployqt 补齐 Qt 依赖
                local targetfile = path.join(deploydir, target:filename())
                os.cp(target:targetfile(), targetfile)
                local installfiles = {targetfile}
                for _, dep in ipairs(target:orderdeps()) do
                    if dep:rule("qt.shared") then
                        local depfile = path.join(deploydir, path.filename(dep:targetfile()))
                        os.cp(dep:targetfile(), depfile)
                        table.insert(installfiles, depfile)
                    end
                end
                windeployqt.run_deploy(target, deploydir, installfiles)

                batchcmds:mkdir(bindir)
                batchcmds:cp(path.join(deploydir, "*"), bindir, {rootdir = deploydir})
                pack_batchcmds.install_target_shared_libraries(target, batchcmds, {bindir = bindir, package = package})
            else
                -- 注意：这里装的是可执行文件自身，不能用 target:bindir() 去找构建产物。
                -- target:bindir() 返回的是**安装**目录，linux 平台默认 installdir 是 /usr/local，
                -- 于是它等于 /usr/local/bin，os.isdir 又为真，下面就会把系统目录当成产物目录拷进包里。
                batchcmds:mkdir(bindir)
                batchcmds:cp(target:targetfile(), path.join(bindir, target:filename()))
                pack_batchcmds.install_target_shared_libraries(target, batchcmds,
                    {bindir = package:installdir("lib"), package = package})
            end

            -- 安装目标自身的文件（资源等）
            pack_batchcmds.install_target_files(target, batchcmds, opt)
            pack_batchcmds.update_target_install_rpath(target, batchcmds, opt)
        end)

        on_uninstallcmd(function (target, batchcmds, opt)
            local package = opt.package
            if not package then
                return
            end
            if target:is_plat("windows", "mingw") then
                local deploydir = path.join(target:autogendir(), "qt", "deploy", target:name())
                if not os.isdir(deploydir) then
                    return
                end
                local bindir = package:bindir()
                for _, item in ipairs(os.filedirs(path.join(deploydir, "*"))) do
                    local dstpath = path.join(bindir, path.relative(item, deploydir))
                    if os.isdir(item) then
                        batchcmds:rmdir(dstpath, {emptydirs = true})
                    else
                        batchcmds:rm(dstpath, {emptydirs = true})
                    end
                end
            else
                -- 与内置 qt.widgetapp 的 uninstallcmd 的 linux 分支一致：清空包 bin 目录
                local package_bindir = package:installdir("bin")
                for _, item in ipairs(os.filedirs(path.join(package_bindir, "*"))) do
                    if os.isfile(item) then
                        batchcmds:rm(item, {emptydirs = true})
                    elseif os.isdir(item) then
                        batchcmds:rmdir(item, {emptydirs = true})
                    end
                end
            end
        end)
    end
rule_end()


rule("ast.qt.ts")
    add_deps("qt.env")
    set_extensions(".ts")

    on_config(function (target)
        import("lib.detect.find_file")
        import("core.base.json")
        import("core.base.semver")

        -- get source file
        local lupdate_argv = {"-no-obsolete", "-tr-function-alias", "qtTrId+=_,QT_TRID_NOOP+=N_,QT_TRANSLATE_NOOP+=NC_,AST_TRANSLATE_NOOP,QT_TR_NOOP+=AST_TR_NOOP"}
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
            if semver.compare(qt.sdkver, "5.15.0") < 0 then
                target:set("enabled", false)
                return
            end

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
