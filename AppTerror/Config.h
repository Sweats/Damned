#ifndef TERROR_CONFIG_H
#define TERROR_CONFIG_H

#include <Core/Core.h>
#include <Scenes/Scenes.h>
#include <TerrorDeferredShading/TerrorDeferredShading.h>
#include <GenericGame/GenericGame.h>
#include <GenericCharacter/GenericCharacter.h>
#include <TerrorNetwork/TerrorNetwork.h>
#include <Entities/Entities.h>

#pragma comment(lib, "Algebra.lib")
#pragma comment(lib, "Animations.lib")
#pragma comment(lib, "Audio.lib")
#pragma comment(lib, "BoostWrapper.lib")
#pragma comment(lib, "Compression.lib")
#pragma comment(lib, "Core.lib")
#pragma comment(lib, "CrashReport.lib")
#pragma comment(lib, "Crypto.lib")
#pragma comment(lib, "Database.lib")
#pragma comment(lib, "DeferredShading.lib")
#pragma comment(lib, "Entities.lib")
#pragma comment(lib, "Events.lib")
#pragma comment(lib, "GameNetwork.lib")
#pragma comment(lib, "GenericCharacter.lib")
#pragma comment(lib, "GenericGame.lib")
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "GUI.lib")
#pragma comment(lib, "Input.lib")
#pragma comment(lib, "Network.lib")
#pragma comment(lib, "OldEntities.lib")
#pragma comment(lib, "Parser.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "PythonScripting.lib")
#pragma comment(lib, "Scenes.lib")
#pragma comment(lib, "Steam.lib")
#pragma comment(lib, "Utils.lib")

#define SAVE_VERSION 8

namespace terror {
	using namespace core;
	using namespace scenes;
	using namespace terror_deferred_shading;	
	using namespace generic_game;
	using namespace generic_character;
	using namespace terror_network;

	typedef std::list<int> IntList;
}

#endif