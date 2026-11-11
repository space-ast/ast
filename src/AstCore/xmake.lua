target("AstCore")
    set_kind("shared")
    add_files("**.cpp")
    add_files("**.c")
    add_headerfiles("**.hpp", {prefixdir="AstCore"})
    add_headerfiles("**.h", {prefixdir="AstCore"})
    add_defines("AST_BUILD_LIB_CORE")
    -- add_extrafiles("xmake.lua")    
    add_deps("AstUtil", "AstMath", "AstWeather", "AstScript")
    add_packages("cspice", {links = {}})
    
    -- Vallado/SGP4.cpp 单独处理：MSVC ARM64 在 /Od 下会把 sgp4init 里 dpper(...) 的 40 实参调用编译错
    -- 规避方式：该文件脱离 unity 单独编译并用 /O1，编译器修复后可删掉这段
    if is_plat("windows") and is_arch("arm64") and is_mode("debug") then
        add_files("Propagator/SGP4/Vallado/SGP4.cpp", {
            unity_ignored = true,
            cxflags = "/O1",
        })
    end