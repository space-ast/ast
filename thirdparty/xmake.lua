set_group("thirdparty")
set_warnings("more")

includes("*/xmake.lua|cspice-*/xmake.lua")

if is_plat("windows", "mingw") then
    includes("cspice-windows/xmake.lua")
elseif is_plat("linux") then
    includes("cspice-linux/xmake.lua")
end



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
