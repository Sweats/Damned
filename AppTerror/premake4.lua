project "AppTerror"

kind "ConsoleApp"
targetdir "../output"
includedirs {
	"./",
}
links {
	"TerrorNetwork",
	"TerrorDeferredShading",
}
files {
	"**.cpp",
	"**.h"
}
