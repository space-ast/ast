add_defines("AST_BUILD_LIB")
includes("*/xmake.lua")

if is_plat("windows") and is_mode("debug") then
    set_suffixname("D") -- windows的调试库使用D后缀
end