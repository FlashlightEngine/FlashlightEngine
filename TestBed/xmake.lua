target("TestBed")
  add_files("Source/TestBed/**.c")

  add_includedirs("Include")

  for _, ext in ipairs({".h"}) do
    add_headerfiles("Include/(TestBed/**" .. ext .. ")")
  end

  add_deps("FlashlightEngine")
