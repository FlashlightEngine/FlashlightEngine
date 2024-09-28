local headerTemplate, sourceTemplateWithHeader, sourceTemplateWithoutHeader

task("create-file")

set_menu({
    usage = "xmake create-file [options] name",
    description = "Task to help file creation.",
    options = {
        {nil, "noc", "k", nil, "Set this to only create a header file." },
        {nil, "noh", "k", nil, "Set this to only create a source file."},
        {nil, "name", "v", nil, "File name" }
    } 
})

on_run(function()
    import("core.base.option")

    local filePath = option.get("name")
    if not filePath then
        os.raise("Missing file name")
    end

    local className = path.basename(filePath)

    local files = {}

    if not option.get("noh") then
        table.insert(files, { TargetPath = path.join("Include/FlashlightEngine", filePath) .. ".h", Template = headerTemplate })
    end

    if not option.get("noc") then
        if not option.get("noh") then
            table.insert(files, { TargetPath = path.join("Source", "FlashlightEngine", filePath) .. ".c", Template = sourceTemplateWithHeader })
        else
            table.insert(files, { TargetPath = path.join("Source", "FlashlightEngine", filePath) .. ".c", Template = sourceTemplateWithoutHeader })
        end
    end

    local replacements = {
        CLASS_NAME = className,
        CLASS_PATH = filePath,
        COPYRIGHT = os.date("%Y") .. [[ Jean "Pixfri" Letessier ]],
        HEADER_GUARD = "FL_" .. filePath:gsub("[/\\]", "_"):upper() .. "_H"
    }

    for _, file in pairs(files) do
        local content = file.Template:gsub("%%([%u_]+)%%", function (kw)
            local r = replacements[kw]
            if not r then
                os.raise("Missing replacement for " .. kw)
            end

            return r
        end)

        io.writefile(file.TargetPath, content)
    end
end)

headerTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef %HEADER_GUARD%
#define %HEADER_GUARD%

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#endif // %HEADER_GUARD%
]]

sourceTemplateWithHeader = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/%CLASS_PATH%.h"
]]

sourceTemplateWithoutHeader = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h
]]