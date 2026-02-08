target("AstWasm")
    set_kind("shared")
    if not is_plat("wasm") then
        set_enabled(false)
    end
    
