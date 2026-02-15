target("AstWeather")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.c")
    add_headerfiles("**.hpp")
    add_defines("AST_BUILD_LIB_WEATHER")
    if has_package("libf2c") then
        add_defines("AST_WITH_LIBF2C")
        add_packages("libf2c")
    end