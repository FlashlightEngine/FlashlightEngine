rule("natvis")
    on_config(function (target)
        for name, pkg in table.orderpairs(target:pkgs()) do
            local includedir = path.join(pkg:installdir(), "Include")
            local natvis = os.files(path.join(includedir, "**.natvis"))
            if #natvis > 0 then
                local groups = table.wrap(target:get("filegroups"))
                if not table.find(groups, name) then
                    target:add("extrafiles", natvis)
                    target:add("filegroups", path.join("natvis", name), {rootdir = includedir})
                end
            end
        end
    end)