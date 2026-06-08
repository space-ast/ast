add_defines("AST_BUILD_LIB")
includes("*/xmake.lua")

if is_plat("windows") and is_mode("debug") then
    set_suffixname("D") -- windows的调试库使用D后缀
end

if is_plat("windows") then
    add_cxflags("/utf-8")
end


target("AstConfig")
    set_kind("phony")
    add_files("**.cpp|AstGUI/**|AstChart/**")
    add_rules("ast.qt.ts")
    add_files("*.ts")
    add_rules("c++.unity_build", {batchsize=0})
target_end()
