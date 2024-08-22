rule("embed.resources")
    on_config(function (target, opt)
        import("core.base.option")
        import("utils.progress")

        local function generate_embed_header(filepath, targetpath)
            local bufferSize = 1024 * 1024
            
            cprint("${color.build.object}embedding %s", filepath)

            local resource = assert(io.open(filepath, "rb"))
            local targetFile = assert(io.open(targetpath, "w+"));

            local resourceSize = resource:size()

            local remainingSize = resourceSize
            local headerSize = 0

            while remainingSize > 0 do
                local readSize = math.min(remainingSize, bufferSize)
                local data = resource:read(readSize)
                remainingSize = remainingSize - readSize

                local headerContentTable = {}
                for i = 1, #data do
                    table.insert(headerContentTable, string.format("%d,", data:byte(i)))
                end
                local content = table.concat(headerContentTable)

                headerSize = headerSize + #content

                targetFile:write(content)
            end

            resource:close()
            targetFile:close()
        end

        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename == "embed.resources" then
                for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                    local targetPath = sourcefile .. ".h"
                    if option.get("rebuild") or os.mtime(sourcefile) >= os.mtime(targetPath) then
                        generate_embed_header(sourcefile, targetPath)
                    end
                end
            end
        end
    end)