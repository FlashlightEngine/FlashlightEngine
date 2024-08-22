local headerTemplate, inlineTemplate, sourceTemplate

local modules = FLModules
local rendererBackends = FLRendererBackends

task("create-class")

set_menu({
    usage = "xmake create-class [options] name",
    description = "Task to help class creation.",
    options = {
        {nil, "nocpp", "k", nil, "Set this to create a header-only class."},
        {nil, "module", "v", nil, "Module where the class should be created."},
        {nil, "name", "v", nil, "Class name" }
    } 
})

on_run(function()
    import("core.base.option")

    local moduleName = option.get("module")
    if not moduleName then
        os.raise("Missing module name")
    end

    local classPath = option.get("name")
    if not classPath then
        os.raise("Missing class name")
    end

    local module = modules[moduleName]
    if not module then
        -- Try using the renderer backends instead
        --module = rendererBackends[moduleName]
        --if not module then
            os.raise("Unknown module" .. moduleName)
        --end
    end

    local className = path.basename(classPath)

    local files = {
        { TargetPath = path.join("Include/Flashlight", moduleName, classPath) .. ".hpp", Template = headerTemplate },
        { TargetPath = path.join("Include/Flashlight", moduleName, classPath) .. ".inl", Template = inlineTemplate }
    }

    if not option.get("nocpp") then
        table.insert(files, { TargetPath = path.join("Source", "Flashlight", moduleName, classPath) .. ".cpp", Template = sourceTemplate })
    end

    local replacements = {
        CLASS_NAME = className,
        CLASS_PATH = classPath,
        COPYRIGHT = os.date("%Y") .. [[ Jean "Pixfri" Letessier ]],
        HEADER_GUARD = "FL_" .. moduleName:upper() .. "_" .. classPath:gsub("[/\\]", "_"):upper() .. "_HPP",
        MODULE_API = "FL_" .. moduleName:upper() .. "_API",
        MODULE_NAME = moduleName
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
// This file is part of Flashlight Engine - %MODULE_NAME% module.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef %HEADER_GUARD%
#define %HEADER_GUARD%

#include <FLUtils/Defines.hpp>
#include <Flashlight/%MODULE_NAME%/Export.hpp>

namespace Flashlight {
    class %MODULE_API% %CLASS_NAME% {
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

#include <Flashlight/%MODULE_NAME%/%CLASS_PATH%.inl>

#endif // %HEADER_GUARD%
]]

inlineTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine - %MODULE_NAME% module.
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Flashlight {
}
]]

sourceTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of Flashlight Engine - %MODULE_NAME% module.
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Flashlight/%MODULE_NAME%/%CLASS_PATH%.hpp>

namespace Flashlight {
}
]]