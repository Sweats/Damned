solution "Engine"

configurations {"Release", "Debug"}
configuration "Release"
	flags {"Symbols", "Unicode", "NoFramePointer", "OptimizeSpeed", "FloatFast"}
configuration "Debug"
	flags {"Symbols", "Unicode", "NoEditAndContinue" ,"FloatFast"}

platforms {"x32"}

configuration {"x32", "debug"}
	targetsuffix "_d"

configuration {"Windows"}
	defines {"WIN32", "NDEBUG", "_CONSOLE", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS"}
	linkoptions {"/ignore:4006", "/ignore:4193", "/ignore:4221", "/ignore:4251", "/ignore:4275", "/nodefaultlib:libcmt.lib", "/nodefaultlib:libcpmt.lib", "/nodefaultlib:libcmtd.lib", "/nodefaultlib:libcpmtd.lib"}
	buildoptions {"/bigobj"}

configuration{}

language "C++"

location ("./Solution")

depsdir = os.getenv("USERPROFILE") .. "/Dropbox (9 Heads)/9gears_dependencies_2014/"

includedirs {
	"./",    	    
	"C:/Python27/include",
    depsdir .. "9gears/include",    
    depsdir .. "boost_1_55_0",
    depsdir .. "bullet-2.82/src",
    depsdir .. "Crypto++_5_6_2",
    depsdir .. "google-breakpad/src",
    depsdir .. "Ogre/build/include",
    depsdir .. "Ogre/OgreMain/**",
    depsdir .. "Ogre/Components/**",
    depsdir .. "Ogre/PlugIns/**",
    depsdir .. "Ogre/Dependencies/include",
    depsdir .. "miniupnpc-1.9",
    depsdir .. "MyGUI/MyGUIEngine/include",
    depsdir .. "MyGUI/Platforms/Ogre/OgrePlatform/include",
    depsdir .. "OTL_v4_h2",
    depsdir .. "PhysX 3.3.1/Include",
    depsdir .. "SFML-2.1/include",
    depsdir .. "speex-1.2rc1/include",
    depsdir .. "Steamworks-1.31/public",
    depsdir .. "zlib128/include",
}
libdirs {    
	"C:/Python27/libs",
    depsdir .. "9gears/lib",    
    depsdir .. "boost_1_55_0/stage/lib",
    depsdir .. "bullet-2.82/lib",
    depsdir .. "Crypto++_5_6_2/Crypto++/Win32/Output/Release",
    depsdir .. "google-breakpad/src/client/windows/Release/lib",
    depsdir .. "Ogre/build/lib/Release",
    depsdir .. "Ogre/Dependencies/lib/Release",
    depsdir .. "miniupnpc-1.9/msvc/Release",
    depsdir .. "MyGUI/build/lib/Release",
    depsdir .. "PhysX 3.3.1/Lib/win32",
    depsdir .. "SFML-2.1/lib",
    depsdir .. "speex-1.2rc1/lib",
    depsdir .. "Steamworks-1.31/redistributable_bin",
    depsdir .. "zlib128/lib",
}

include "./AppTerror"
include "./TerrorDeferredShading"
include "./TerrorNetwork"
