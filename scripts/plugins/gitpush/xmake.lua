task("gitpush")
    set_menu{
        usage = "xmake gitpush",
        description = "推送Git仓库"
    }
    on_run(function ()
        os.exec("python " .. path.join(os.projectdir(), "scripts/git_push_retry.py"))
    end)
task_end()