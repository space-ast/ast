rule("ast")
    on_load(function (target)
        if target:plat() == "wasm" then
            -- 添加wasm预加载文件
            -- @todo: 这里执行太慢了，需要优化
            local files = os.files("data/**|**plneph.405|**leDE1900.421|**leDE1941.405|**leDE18002100.424")
            for _, file in ipairs(files) do
                target:add("values", "wasm.preloadfiles", file .. "@" .. file)
            end
        end
    end)