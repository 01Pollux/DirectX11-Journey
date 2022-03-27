project "ImGui"
	kind "StaticLib"

	language "C++"
	cppdialect "C++17"

    staticruntime "off"
	systemversion "latest"

	targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")

	files
	{
		"imconfig.h",
        "stb_sprintf.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",

        "imcxx/**.cpp",
        "imcxx/**.hpp"
	}

	includedirs
	{
		DefaultIncludeDir
	}

    filter "system:windows"
		defines 
		{
			"_CRT_SECURE_NO_WARNINGS",
            "IMCXX_MISC_NO_NOTIFICATION_DEMO",
            "IMCXX_MISC_NO_SHORTCUT_DEMO"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
