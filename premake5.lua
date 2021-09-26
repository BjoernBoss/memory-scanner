newaction {
	trigger     = "clean",
	description = "Remove all build artifacts",
	execute     = function ()
		os.rmdir("./bin")
		os.rmdir("./build")
		print("All artifacts have been removed.")
	end
}

function IncludeExtern(name, location)
	-- define the root paths and directories to be used by the libraries
	rootLocationPath = path.join(_MAIN_SCRIPT_DIR, 'build', name)
	rootTargetDir = path.join(_MAIN_SCRIPT_DIR, 'bin', name)
	rootObjectDir = path.join(_MAIN_SCRIPT_DIR, 'build', name)

	-- include the actual library
	include (path.join(location, name))
end

local ProjectName = "Memory-Reader"

workspace(ProjectName)
	language "C++"
	configurations { "debug", "release" }
	location "build"
	systemversion "latest"
	architecture "x86_64"
	
project(ProjectName)
	kind "ConsoleApp"
	location "build"
	cppdialect "C++17"
	staticruntime "On"
	
	-- include the dependencies
	links { "Console-Menu" }
	includedirs { "repos" }
	
	targetdir "bin/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "build/%{cfg.buildcfg}-%{cfg.architecture}"
	
	includedirs { "source" }
	files { "source/**.h", "source/**.cpp", "source/**.txt", "source/**.lua", "*.txt", "*.lua" }
	
	filter "configurations:debug"
		defines { "BIN_DEBUG" }
		debugdir "bin"
		symbols "On"
		runtime "Debug"
	
	filter "configurations:release"
		defines { "BIN_RELEASE" }
		optimize "Full"
		symbols "On"
		runtime "Release"

-- define the root directories to be used by the libraries
group "repos"
	IncludeExtern("console-menu", "repos")
group ""