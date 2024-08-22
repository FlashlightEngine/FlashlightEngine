local modules = FLRendererBackends

-- Build renderer plugins if linked to FLRenderer
rule("build.rendererplugins")
	on_load(function (target)
		if has_config("embed_rendererbackends", "static") then
			return
		end

		local deps = table.wrap(target:get("deps"))

		if target:kind() == "binary" and (table.contains(deps, "FLRenderer") or table.contains(deps, "FLGraphics")) then
			for name, _ in pairs(modules) do
				local depName = "FL" .. name
				if name:match("^.+Renderer$") and not table.contains(deps, depName) then -- don't overwrite dependency
					target:add("deps", depName, {inherit = false})
				end
			end
		end
	end)