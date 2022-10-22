workspace "Hazel"                                  --解决方案名称
    architecture "x64"                             --架构为x64
    configurations {                               --配置有三种，最后一种是真正的发行版
                        "Debug",
                        "Release",
                        "Dist" 
                   }

    startproject "Sandbox"                              --启动项目为Sandbox

-- Include directories relative to root folder (solution directory)
IncludeDir = {}                                     --第三方库列表
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"    --定义GLFW项对应的目录
IncludeDir["Glad"] = "Hazel/vendor/GLAD/include"    --定义GLFW项对应的目录
IncludeDir["ImGui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["assimp"] = "Hazel/vendor/assimp/include"

group "Dependencies"
	include "Hazel/vendor/GLFW"                     --将该目录下的premake5.lua内容复制过来，那里存的就是GLFW项目的布局和配置
	include "Hazel/vendor/Glad"
	include "Hazel/vendor/imgui"



project "Hazel"                                     --项目名称Hazel
    location "Hazel"                                --项目地址，即lua文件所在目录中的文件夹Hazel
    kind "StaticLib"                                --项目输出类型，静态库
    language "C++"
    cppdialect "C++17"
    staticruntime "on"                              --运行库为多线程
    targetdir "x64/%{cfg.buildcfg}"                 --生成地址，x64/配置（cfg）
    objdir "%{prj.name}/x64/%{cfg.buildcfg}"        --obj中间文件生成地址，项目名称/x64/配置（cfg）
    pchheader "hzpch.h"                             --开启预编译头文件
    pchsource "%{prj.name}/src/hzpch.cpp"           --指定预编译头文件地址

    files {                                         --项目文件，即项目名称/src目录下及其根目录所有的.h和.cpp文件（其中/**表示当前路径及其所有子路径）        
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp", 
            "%{prj.name}/vendor/glm/glm/**.hpp",    --glm库的文件
		    "%{prj.name}/vendor/glm/glm/**.inl",
            "%{prj.name}/vendor/stb_image/stb_image.cpp",
            "%{prj.name}/vendor/stb_image/stb_image.h"
          }

    defines {
              "_CRT_SECURE_NO_WARNINGS"
            }

    includedirs {
                    "%{prj.name}/vendor/spdlog/include", --附加包含目录
                    "%{prj.name}/src",
                    "%{prj.name}/src/Hazel",
                    "%{IncludeDir.GLFW}",                 --列表的GLFW项的地址
                    "%{IncludeDir.Glad}",                 
                    "%{IncludeDir.ImGui}",
                    "%{IncludeDir.glm}",
                    "%{IncludeDir.assimp}",
                    "%{prj.name}/vendor/stb_image"
                }

    links { 
            "GLFW",
            "Opengl32.lib",                          --附加依赖项
            "Glad",
            "ImGui",
            "assimp-vc142-mtd.lib"
          }
    libdirs  "Hazel/vendor/assimp"           --附加库目录

    filter "system:windows"                         --filter可看作if，即如果系统是windows。
                                  
        
        systemversion "latest"                      --SDK版本为最新
        defines {                                   --预处理器定义
                    "HZ_PLATFORM_WINDOWS",
                    "HZ_BUILD_DLL",
                    "GLFW_INCLUDE_NONE"             --引用GLFW时不再引用opengl头文件（Glad已经引用了opengl头文件）
                }

    filter "configurations:Debug"                   --如果为Debug模式
        defines "HZ_DEBUG"                          --预处理器定义
        symbols "on"                                --开启debug调试
        runtime "Debug"                             --运行库为多线程调试DLL
        


    filter "configurations:Release"                 --如果为Release模式
        defines "HZ_RELEASE"                        --预处理器定义
        optimize "on"                               --开启优化
        runtime "Release"                           --运行库为多线程DLL

    filter "configurations:Dist"                    --如果为Dist模式
        defines "HZ_DIST"                           --预处理器定义
        optimize "on"                               --开启优化
        runtime "Release"                           --运行库为多线程DLL

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"                              --运行库为多线程
    
    targetdir "x64/%{cfg.buildcfg}"
    objdir "%{prj.name}/x64/%{cfg.buildcfg}"
    
    files { 
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp" 
          }

    includedirs { 
                    "Hazel/vendor/spdlog/include",
                    "Hazel/src",
                    "%{IncludeDir.glm}",
                    "Hazel/vendor",
                    "%{IncludeDir.GLFW}",
                    "%{IncludeDir.Glad}",
                    "%{IncludeDir.assimp}",
                    "Hazel/vendor/stb_image"
                }

    links{
            "Hazel",                                      --引用Hazel项目
            "assimp-vc142-mtd.lib"
         }
    libdirs  "Hazel/vendor/assimp"           --附加库目录
    filter "system:windows"

        systemversion "latest"
        defines {   
                    "HZ_PLATFORM_WINDOWS"
                }
    
    filter "configurations:Debug"
        defines "HZ_DEBUG"
        symbols "on"
        runtime "Debug"                             --运行库为多线程调试DLL
    
    filter "configurations:Release"
        defines "HZ_RELEASE"
        optimize "on"
        runtime "Release"                           --运行库为多线程DLL
    
    filter "configurations:Dist"
        defines "HZ_DIST"
        optimize "on"
        runtime "Release"                           --运行库为多线程DLL