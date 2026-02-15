target("AstWeather")
    if not has_package("libf2c") then
        remove_files("IGRF13/igrf13.c")
    end
