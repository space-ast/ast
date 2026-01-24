set_group("thirdparty")
includes("*/xmake.lua")

target("cminpack")
	add_files("cminpack/*.c")
    set_kind("static")
    add_defines("CMINPACK_NO_DLL")

target("libf2c")
    if is_plat("linux") then
        add_cxflags("-fPIC")
    end
