add_defines("AST_BUILD_LIB")
if has_package("fmt") then
    add_packages("fmt")
    add_defines("AST_WITH_FMT")
end
includes("*/xmake.lua")
