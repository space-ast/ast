
task("genheader")
    set_menu{
        usage = "xmake genheader",
        description = "生成头文件和SWIG接口文件"
    }
    on_run(function ()
        os.exec("python " .. path.join(os.scriptdir(), "../gen_redirect_header.py"))
        os.exec("python " .. path.join(os.scriptdir(), "../generate_aggregate_headers.py"))
        os.exec("python " .. path.join(os.scriptdir(), "../gen_swig_interface.py"))
    end)
task_end()
