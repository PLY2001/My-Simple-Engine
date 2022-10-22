project "Glad"
    kind "StaticLib"--输出类型为静态库，即lib
    language "C"
    staticruntime "on"
    
    targetdir "x64/%{cfg.buildcfg}"
	objdir "%{prj.name}/x64/%{cfg.buildcfg}" 

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

	includedirs
	{
		"include"
	}
    
    filter "system:windows"
        systemversion "latest"
        
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"