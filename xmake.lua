-- Copyright (C) 2024 Jean "Pixfri" Letessier 
-- This file is part of Flashlight Engine.
-- For conditions of distribution and use, see copyright notice in LICENSE

-- Modules
local rendererBackends = {
    VulkanRenderer = {
        Option = "vulkan",
        Deps = {"FLRenderer"},
        Custom = function()
        Packages = {"vulkan-headers", "vulkan-memory-allocator"}
            add_defines("VK_NO_PROTOTYPES")
            if is_plat("windows", "mingw") then
                add_defines("VK_USE_PLATFORM_WIN32_KHR")
                add_syslinks("User32")
            elseif is_plat("linux") then
                add_defines("VK_USE_PLATFORM_XLIB_KHR")
                add_defines("VK_USE_PLATFORM_WAYLAND_KHR")
            end
        end
    }
}
FLRendererBackends = rendererBackends

local modules = {
    Core = {
        Custom = function()
            add_headerfiles("Include/(FlashlightEngine/*.hpp)")
            
            if is_plat("windows", "mingw") then
                add_syslinks("Ole32")
            elseif is_plat("linux") then
                add_packages("libuuid")
                add_syslinks("dl", "pthread")
            end
        end,
        Packages = {"stb", "entt", "frozen", "utfcpp"},
        PublicPackages = {"flutils"}
    },
    Graphics = {
        Option = "graphics",
        Deps = {"FLRenderer"},
        Packages = {"entt"}
    },
    Platform = {
        Option = "platform",
        Deps = {"FLCore"},
        Packages = {"utfcpp"},
        Custom = function()
            add_packages("libsdl", {components = {"lib"}})
            if is_plat("windows", "mingw") then
                add_defines("SDL_VIDEO_DRIVER_WINDOWS=1")
            elseif is_plat("linux") then
                add_defines("SDL_VIDEO_DRIVER_X11=1")
                add_defines("SDL_VIDEO_DRIVER_WAYLAND=1")
            end
        end
    },
    Renderer = {
        Option = "renderer",
        Deps = {"FLPlatform"},
        Packages = {"frozen"},
        PublicPackages = {"flutils"},
        Custom = function()
            if has_config("embed_rendererbackends", "static") then
                add_defines("FL_EMBED_RENDERERBACKEND")
                for name, module in table.orderpairs(rendererBackends) do
                    if not module.Option or has_config(module.Option) then
                        if module.Deps then
                            module = table.clone(module, 1)
                            module.Deps = table.remove_if(table.clone(module.Deps), function (idx, dep) return dep == "FLRenderer" end)
                            if #module.Deps == 0 then
                                module.Deps = nil
                            end
                        end
                    
                        ModuleTargetConfig(name, module)
                    end
                end
            end
        end
    }
}

if not has_config("embed_rendererbackends", "static") then
    -- Register renderer backends as their own modules
    for name, module in pairs(rendererBackends) do
        if (module[name] ~= nil) then
            os.raise("overriding module " .. name)
        end
        
        modules[name] = module
    end
end

FLModules = modules

set_xmakever("2.9.3")

set_project("FlashlightEngine")
set_license("MIT")

includes("xmake/**.lua")

option("embed_rendererbackends", {description = "Embed the renderer backends in FLRenderer instead of loading them dynamically.", default = false})
option("static", {description = "Build the engine as static libraries.", default = false})
option("override_runtime", {description = "Override vs runtime to MD in release and MDd in debug.", default = true})
option("usepch", {description = "Use precompiled headers (speeds up compilation.)", default = false})

includes("xmake/**.lua")

-- Sanitizers
local sanitizers = {
    asan = "address",
    lsan = "leak",
    tsan = "thread"
}

for opt, policy in table.orderpairs(sanitizers) do
    option(opt, {description = "Enable " .. opt, default = false})
    
    if has_config(opt) then
        add_defines("FL_WITH_" .. opt:upper())
        set_policy("build.sanitizer." .. policy, true)
    end
end

-- Allow to disable some modules
for name, module in table.orderpairs(modules) do
	if module.Option then
		option(module.Option, { description = "Enables the " .. name .. " module", default = true, category = "Modules" })
	end
end

-- Dependencies
add_repositories("FlashlightEngineRepo https://github.com/FlashlightEngine/xmake-repo")

add_requires(
    "entt 3.13.2",
    "spdlog v1.9.0",
    "frozen", 
    "flutils >=2024.08.22", 
    "stb", 
    "utfcpp"
)

-- Don't link system-installed libraries on CI
if os.getenv("CI") then
    add_requireconfs("*", {system = false})
    add_requireconfs("freetype.*", {system = false})
end

if has_config("platform") then
    add_requires("libsdl >=2.26.0")
    if is_plat("linux") then
        add_requires("libxext", "wayland", {configs = { asan = false }})
    end
end

if has_config("renderer") then    
    local function is_cross_compiling()
        if os.host() == "windows" then
            local host_arch = os.arch()
            if is_plat("windows") then
                -- maybe cross-compilation for arm64 on x86/x64
                if (host_arch == "x86" or host_arch == "x64") and is_arch("arm64") then
                    return true
                -- maybe cross-compilation for x86/x64 on arm64
                elseif host_arch == "arm64" and not is_arch("arm64") then
                    return true
                end
                return false
            elseif is_plat("mingw") then
                return false
            end
        end
        if not is_plat(os.host()) and not is_plat(os.subhost()) then
            return true
        end
        return false
    end
end

if has_config("vulkan") then
    add_requires("vulkan-headers", "vulkan-memory-allocator")
end

if is_plat("linux") then
    add_requires("libuuid")
end

-- Global configuration
add_rules("mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build.rendererplugins")
add_rules("natvis")

set_allowedplats("windows", "mingw", "linux")
set_allowedmodes("coverage", "debug", "releasedbg", "release")
set_defaultmode("debug")

add_includedirs("Include")

set_encodings("utf-8")
set_exceptions("cxx")
set_languages("c89", "cxx20")
set_rundir("./build/$(mode)-$(plat)-$(arch)/bin")
set_targetdir("./build/$(mode)-$(plat)-$(arch)/bin")
set_warnings("allextra")

if is_mode("debug") then
    add_rules("debug.suffix")
    add_defines("FL_DEBUG")
elseif is_mode("coverage") then 
    if not is_plat("windows") then 
        add_links("gcov")
    end 
elseif is_mode("releasedbg", "release") then
    set_fpmodels("fast")
    add_vectorexts("sse", "sse2", "sse3", "ssse3")
end 

if not is_mode("release") then 
    set_symbols("debug", "hidden")
end

-- Compiler flags
add_rules("@flutils/compiler_setup")
add_cxflags("-Wno-missing-field-initializers", { tools = { "clang", "gcc" } })

-- Platform specific options
if is_plat("windows") then
	if has_config("override_runtime") then
		set_runtimes(is_mode("debug") and "MDd" or "MD")
	end
elseif is_plat("mingw") then
	-- Use some optimizations even in debug for MinGW to reduce object size
	if is_mode("debug") then
		add_cxflags("-Og")
	end
	add_cxflags("-Wa,-mbig-obj")
end

-- Targets
function ModuleTargetConfig(name, module)
    add_defines("FL_" .. name:upper() .. "_BUILD")
    if is_mode("debug") then
        add_defines("FL_" .. name:upper() .. "_DEBUG") 
    end

    -- Add header and source files
    for _, ext in ipairs({".h", ".hpp", ".inl"}) do
        add_headerfiles("Include/(Flashlight/" .. name .. "/**" .. ext .. ")")
        add_headerfiles("Source/Flashlight/" .. name .. "/**" .. ext, {prefixdir = "private", install = false})
    end

    add_files("Source/Flashlight/" .. name .. "/**.cpp")

    if has_config("embed_resources") then
        local embedResourceRule = false
        for _, filepath in pairs(os.files("Source/Flashlight/" .. name .. "/Resources/**|**.h")) do
            if not embedResourceRule then
                add_rules("embed.resources")
                embedResourceRule = true
            end

            add_files(filepath, {rule = "embed.resources"})
        end
    end

    -- Remove platform-specific files
	if not is_plat("windows", "mingw") then
		remove_headerfiles("Source/Flashlight/" .. name .. "/Win32/**")
		remove_files("Source/Flashlight/" .. name .. "/Win32/**")
	end

	if not is_plat("linux") then
		remove_headerfiles("Source/Flashlight/" .. name .. "/Linux/**")
		remove_files("Source/Flashlight/" .. name .. "/Linux/**")
	end

	if not is_plat("linux") then
		remove_headerfiles("Source/Flashlight/" .. name .. "/Posix/**")
		remove_files("Source/Flashlight/" .. name .. "/Posix/**")
	end

    if module.Deps then
		add_deps(table.unpack(module.Deps))
	end

	if module.Packages then
		add_packages(table.unpack(module.Packages))
	end

	if module.PublicPackages then
		for _, pkg in ipairs(module.PublicPackages) do
			add_packages(pkg, { public = true })
		end
	end

	if module.Custom then
		module.Custom()
	end
end

for name, module in pairs(modules) do
    if module.Option and not has_config(module.Option) then
        goto continue
    end

    target("FL" .. name, function ()
        set_group("Modules")

        if has_config("static") then
            set_kind("static")
            add_defines("FL_STATIC", {public = true})
        else
            set_kind("shared")
        end

        add_defines("FL_BUILD")
        add_includedirs("Source")
        add_packages("spdlog")
        add_rpathdirs("$ORIGIN")

        if has_config("usepch") then
            set_pcxxheader("Include/Flashlight/" .. name .. ".hpp")
        end

        if is_plat("windows", "mingw") then
            add_defines("FLUTILS_WINDOWS_NT6=1")
        end

        ModuleTargetConfig(name, module)
    end)

    ::continue::
end