add_defines("AST_BUILD_LIB")
includes("*/xmake.lua")

if is_plat("windows") and is_mode("debug") then
    set_suffixname("D") -- windows的调试库使用D后缀
end

-- for gcc: 将一些警告转换为错误，以发现潜在的问题
if is_plat("linux") or is_plat("mingw") then
    add_cxflags("-Werror=uninitialized")
    add_cxflags("-Werror=init-self")
    add_cxflags("-Werror=maybe-uninitialized")
    add_cxflags("-Werror=missing-field-initializers")
    -- C++ 专用警告
    add_cxxflags("-Werror=effc++")
    add_cxxflags("-Werror=reorder")
end

-- if is_plat("windows") then
--     add_cxflags("/utf-8")
-- end


-- target("AstConfig")
--     set_kind("phony")
--     add_files("**.cpp|AstGUI/**|AstChart/**")
--     add_rules("ast.qt.ts")
--     add_files("*.ts")
--     add_rules("c++.unity_build", {batchsize=0})
-- target_end()
