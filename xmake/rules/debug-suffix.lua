rule("debug.suffix")
    on_load(function (target)
        if target:kind() ~= "binary" then
            target:set("suffixname", "-d")
        end        
    end)