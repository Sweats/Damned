project "TerrorDeferredShading"

kind "StaticLib"
targetdir "../output"
includedirs {
	"./",
}
links {}
files {
	"**.cpp",
	"**.h"
}
