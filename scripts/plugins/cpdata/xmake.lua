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
        local srcpath = path.join(os.projectdir(), "data")
        local dstpath = path.join(os.projectdir(), format("build/%s/%s/%s/", plat, arch, mode))
        if not os.exists(dstpath) then
            os.mkdir(dstpath)
        end
        os.cp(srcpath, dstpath)
        print("dstpath:", dstpath)
    end)
task_end()