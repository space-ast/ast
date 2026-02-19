target("AstWeather")
    if has_package("libf2c") then
        -- 规避fortran程序的STOP命令，防止程序直接退出
        -- 将STOP命令替换为函数返回
        add_files("**.c", {defines="s_stop=i_len"})
    else
        remove_files("**.c")
    end
