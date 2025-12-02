set_project("RayTracing")
set_version("1.0.0")
set_xmakever("3.0.0")

set_languages("cxx23")

add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    -- MSVC flags
    add_cxxflags("/MP", "/utf-8", "/W4", "/wd4201")
    add_cxxflags("/WX", {force = true})
    add_defines("NOMINMAX", "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN")
else
    -- UNIX
    add_cxxflags("-Wall", "-Werror", "-Wno-unused-parameter")
end

includes("RayTracing")
