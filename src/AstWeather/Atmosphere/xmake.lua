target("AstWeather")
    if not has_package("libf2c") then
        remove_files("MSIS90/msise90_sub.c")
    end
