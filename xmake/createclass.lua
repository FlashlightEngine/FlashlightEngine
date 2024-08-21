local headerTemplate, inlineTemplate, sourceTemplate

task("create-class")

set_menu({
    usage = "xmake create-class [options] name",
    description = "Task to help class creation.",
    options = {
        {nil, "nocpp", "k", nil, "Set this to create a header-only class."},
        {nil, "name", "v", nil, "Class name" }
    } 
})

on_run(function()
    import("core.base.option")

    local classPath = option.get("name")
    if not classPath then
        os.raise("Missing class name")
    end

    local className = path.basename(classPath)

    local files = {
        { TargetPath = path.join("Include/FlashlightEngine", classPath) .. ".hpp", Template = headerTemplate },
        { TargetPath = path.join("Include/FlashlightEngine", classPath) .. ".inl", Template = inlineTemplate }
    }

    if not option.get("nocpp") then
        table.insert(files, { TargetPath = path.join("Source", classPath) .. ".cpp", Template = sourceTemplate })
    end

    local replacements = {
        CLASS_NAME = className,
        CLASS_PATH = classPath,
        COPYRIGHT = os.date("%Y") .. [[ Jean "Pixfri" Letessier ]],
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
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#include <FlashlightEngine/flpch.hpp>
#include <FlashlightEngine/Export.hpp>

namespace Flashlight {
    class %CLASS_NAME% {
    public:
        %CLASS_NAME%() = default;
        ~%CLASS_NAME%() = default;
        
        %CLASS_NAME%(const %CLASS_NAME%&) = delete;
        %CLASS_NAME%(%CLASS_NAME%&&) = delete;
        
        %CLASS_NAME%& operator=(const %CLASS_NAME%&) = delete;
        %CLASS_NAME%& operator=(%CLASS_NAME%&&) = delete;

    private:
    };
}

#include <FlashlightEngine/%CLASS_PATH%.inl>
]]

inlineTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

namespace Flashlight {
}
]]

sourceTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#include <FlashlightEngine/%CLASS_PATH%.hpp>

namespace Flashlight {
}
]]