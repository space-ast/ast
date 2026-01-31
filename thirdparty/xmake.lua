set_group("thirdparty")
includes("*/xmake.lua")

-- replace cminpack target with cminpack package in xmake-repo
-- target("cminpack")
-- 	add_files("cminpack/*.c")
--     set_kind("static")
--     add_defines("CMINPACK_NO_DLL")

-- make changes in libf2c xmake.lua
-- target("libf2c")
--     if is_plat("linux") then
--         add_cxflags("-fPIC")
--     end
