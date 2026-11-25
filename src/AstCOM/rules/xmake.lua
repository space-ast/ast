rule("platform.windows.idl.check")
    set_extensions(".idl")
    on_config("windows", "mingw", function (target)
        -- 如果存在内置的idl规则，就禁用这个patch规则
        local rule = target:rule("platform.windows.idl")
        if rule then
            target:rule_enable("platform.windows.idl.check", false)
        else
            target:set("enabled", false)
        end
    end)

