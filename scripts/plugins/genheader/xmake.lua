
task("genheader")
    set_menu{
        usage = "xmake genheader",
        description = "生成头文件和SWIG接口文件"
    }
    on_run(function ()
        os.exec("python " .. path.join(os.projectdir(), "scripts/gen_redirect_header.py"))
        os.exec("python " .. path.join(os.projectdir(), "scripts/generate_aggregate_headers.py"))
        os.exec("python " .. path.join(os.projectdir(), "scripts/gen_swig_interface.py"))
        os.exec("python " .. path.join(os.projectdir(), "scripts/gen_ast_forward.py"))
    end)
task_end()
