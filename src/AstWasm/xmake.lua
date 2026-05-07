target("AstWasm")
    set_kind("binary")
    if not is_plat("wasm") then
        set_enabled(false)
        return
    end
    
    add_files("**.cpp")
    add_includedirs(".")
    
    add_deps("AstUtil", "AstCore", "AstSim", "AstMath", "AstAI")
    add_ldflags("--bind")
    -- add_ldflags("-sEXPORT_ES6=1")        -- es6规范在一些浏览器不支持
    add_ldflags("--emit-tsd AstWasm.d.ts")  -- 需要安装 `npm install -g typescript`
    -- 设置为c++17标准，因为embind需要c++17标准的特性
    set_languages("c++17")

    after_build(function (target) 
        os.cp(path.join(os.scriptdir(), "AstWasmTest.html"), target:targetdir())
    end)
