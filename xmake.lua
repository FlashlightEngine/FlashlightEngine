-- Copyright (C) 2024 Jean "Pixfri" Letessier 
-- This file is part of Flashlight Engine.
-- For conditions of distribution and use, see copyright notice in LICENSE

set_xmakever("2.8.3")

set_project("FlashlightEngine")
set_license("Apache 2.0")

includes("xmake/**.lua")

option("static", {description = "Build the engine as a static library.", default = false})
option("override_runtime", {description = "Override vs runtime to MD in release and MDd in debug.", default = true})

local logger_options = {
    warn = "warnings",
    info = "info",
    debug = "debug",
    trace = "trace"
}

for opt, level in table.orderpairs(logger_options) do
    if level == "debug" or level == "trace" then
        option("enable_log_" .. opt, { description = "Enable " .. level .. " messages from the logger (Only in debug mode).", default = true})
        
        if has_config("enable_log_" .. opt) and is_mode("debug") then
            add_defines("FL_LOG_" .. opt:upper() .. "_ENABLED")
        end
    else
        option("enable_log_" .. opt, { description = "Enable " .. level .. " messages from the logger.", default = true})
       
        if has_config("enable_log_" .. opt) then
            add_defines("FL_LOG_" .. opt:upper() .. "_ENABLED")
        end
    end
end

includes("xmake/**.lua")

add_requires("vulkan-loader")

-- Don't link system-installed libraries on CI
if os.getenv("CI") then
    add_requireconfs("*", {system = false})
    add_requireconfs("freetype.*", {system = false})
end

-- Global configuration
add_rules("mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("natvis")

set_allowedplats("windows", "mingw", "linux")
set_allowedmodes("coverage", "debug", "releasedbg", "release")
set_defaultmode("debug")

add_includedirs("Include")

set_encodings("utf-8")
set_exceptions("cxx")
set_languages("c89", "cxx20")
set_rundir("./bin/$(plat)_$(arch)_$(mode)")
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")

if is_mode("debug") then
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

target("FlashlightEngine")
	-- handle shared/static kind
	if has_config("static") then
		set_kind("static")
		add_defines("FL_STATIC", { public = true })
	else
		set_kind("shared")
	end

	add_defines("FL_BUILD")

	-- Add header and source files
	for _, ext in ipairs({".h", ".hpp", ".inl"}) do
		add_headerfiles("Include/(Flashlight/**" .. ext .. ")")
		add_headerfiles("Source/Flashlight/**" .. ext, { prefixdir = "private", install = false })
	end

	-- Add extra files for projects
	for _, ext in ipairs({".natvis"}) do
		add_extrafiles("Include/Flashlight/**" .. ext)
		add_extrafiles("Source/Flashlight/**" .. ext)
	end

	add_files("Source/Flashlight/**.cpp")

	add_includedirs("Source", {private = true})
	add_packages()
	add_rpathdirs("$ORIGIN")

	if is_plat("windows", "mingw") then
		add_defines("FLUTILS_WINDOWS_NT6=1")
	end

	add_packages("vulkan-loader", {public = true})

includes("TestBed/**.lua")