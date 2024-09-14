target("TestBed")
    add_files("Source/TestBed/**.cpp")

    for _, ext in ipairs({".h", ".hpp", ".inl"}) do
		add_headerfiles("Include/(TestBed/**" .. ext .. ")")
		add_headerfiles("Source/TestBed/**" .. ext, { prefixdir = "private", install = false })
	end

    for _, ext in ipairs({".natvis"}) do
		add_extrafiles("Include/Flashlight/**" .. ext)
		add_extrafiles("Source/Flashlight/**" .. ext)
	end

    add_deps("FlashlightEngine")