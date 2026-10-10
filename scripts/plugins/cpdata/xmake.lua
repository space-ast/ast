-- 自定义任务：复制数据目录到构建目录
task("cpdata")
    set_menu{
        usage = "xmake cpdata",
        description = "复制数据目录到构建目录"
    }
    on_run(function ()
        import("core.project.config")
        config.load()
        local plat = config.plat()
        local arch = config.arch()
        local mode = config.mode()
        local datadir = path.join(os.projectdir(), "data")
        local testdatadir = path.join(os.projectdir(), "test-data")
        local dstpath = path.join(os.projectdir(), format("build/%s/%s/%s/", plat, arch, mode))

        if not os.exists(dstpath) then
            os.mkdir(dstpath)
        end
        
        if os.islink(path.join(dstpath, "data")) then
            print("data dest dir is link, skip copy data")
        else
            os.cp(datadir, dstpath)
            print("data is copied to:", dstpath)
        end
        if os.islink(path.join(dstpath, "test-data")) then
            print("test-data dest dir is link, skip copy test-data")
        else
            os.cp(testdatadir, dstpath)
            print("test-data is copied to:", dstpath)
        end
    end)
task_end()