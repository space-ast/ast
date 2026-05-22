includes("rules")
add_rules("platform.windows.idl.patch")

target("AstCOM")
    add_files("**.cpp")
    add_files("**.idl")
    add_files("**.rc")
    set_kind("shared")
    add_headerfiles("**.hpp", {prefixdir="AstCOM"})
    add_defines("AST_BUILD_LIB_COM")  -- 编译AstCOM库
    add_defines("_USRDLL")            -- 定义CAtlDllModuleT变量时需要
    add_defines("UNICODE", "_UNICODE", "_ATL_UNICODE") -- 编译为使用UNICODE宽字符
    add_deps("AstUtil", "AstMath", "AstWeather", "AstScript", "AstSim", "AstCmd")
    if is_plat("windows") then
        add_syslinks("oleaut32", "ole32", "uuid")
    else
        -- set_enabled(false)
        set_kind("phony")
    end
    -- on_load(function(target) 
    --     local msvc = target:toolchain("msvc") or target:toolchain("clang-cl") or target:toolchain("clang")
    --     if not msvc then
    --         target:set("enabled", false)
    --         return
    --     end
    -- end)