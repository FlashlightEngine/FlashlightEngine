target("TestBed")
  add_files("Source/TestBed/**.c")

  add_includedirs("Include")

  for _, ext in ipairs({".h"}) do
    add_headerfiles("Include/(TestBed/**" .. ext .. ")")
    add_headerfiles("Source/TestBed/**" .. ext, {prefixdir = "private", install = false})
  end

  add_deps("FlashlightEngine")
