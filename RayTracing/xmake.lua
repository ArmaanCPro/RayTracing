add_requires("imgui docking", {configs = {sdl3 = true, sdl3_gpu = true}})
add_requires("libsdl3", "glm")

target ("RT")
    set_default(true)
    set_kind("binary")
    set_basename("RT")
    set_targetdir("$(projectdir)/bin/$(mode)")

    add_files("src/**.cpp")
    add_headerfiles("src/**.h") -- optional, for IDEs
    add_includedirs("src", {public = false})

    add_packages("imgui", "libsdl3", "glm")

    -- Sanitizers: enable only for debug mode
    if is_mode("debug") then
        set_policy("build.sanitizer.address", true)
        set_policy("build.sanitizer.undefined", true)
    end

    if is_mode("debug") then
        set_symbols("debug")
    else
        set_optimize("fast")
    end 
