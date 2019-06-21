#include "GUI/GameSessionListboxHandler/GameSessionListboxHandler.h"
#include "GUI/TitleWindowHandler/TitleWindowHandler.h"
#include "GUI/OptionsWindowHandler/OptionsWindowHandler.h"
#include "GUI/GraphicsWindowHandler/GraphicsWindowHandler.h"
#include "GUI/GammaWindowHandler/GammaWindowHandler.h"
#include "GUI/SoundWindowHandler/SoundWindowHandler.h"
#include "GUI/ControlsWindowHandler/ControlsWindowHandler.h"
#include "GUI/ProfileWindowHandler/ProfileWindowHandler.h"
#include "GUI/ConnectionWindowHandler/ConnectionWindowHandler.h"
#include "GUI/JoinGameWindowHandler/JoinGameWindowHandler.h"
#include "GUI/CreateLobbyButtonHandler/CreateLobbyButtonHandler.h"
#include "GUI/LobbyWindowHandler/LobbyWindowHandler.h"
#include "GUI/AllowedMonstersWindowHandler/AllowedMonstersWindowHandler.h"
#include "GUI/StageSelectionWindowHandler/StageSelectionWindowHandler.h"
#include "GUI/ChatWindowHandler/ChatWindowHandler.h"
#include "GUI/LoginWindowHandler/LoginWindowHandler.h"
#include "GUI/ResetPasswordWindowHandler/ResetPasswordWindowHandler.h"
#include "GUI/RegisterWindowHandler/RegisterWindowHandler.h"
#include "GUI/ActivationWindowHandler/ActivationWindowHandler.h"
#include "GUI/CharacterSelectionWindowHandler/CharacterSelectionWindowHandler.h"
#include "GUI/PauseWindowHandler/PauseWindowHandler.h"
#include "GUI/ServerDownWindowHandler/ServerDownWindowHandler.h"
#include "GUI/CreditsWindowHandler/CreditsWindowHandler.h"

#include "GameEndingRoomSystem/GameEndingRoomSystem.h"
#include "GameNetworkHandler/GameNetworkHandler.h"
#include "PlayersStatsHandler/PlayersStatsHandler.h"
#include "VoiceChatHandler/VoiceChatHandler.h"
#include "StartGameHandler/StartGameHandler.h"
#include "ScreenshotHandler/ScreenshotHandler.h"
#include "DebugInputHandler/DebugInputHandler.h"
#include "ProgramDataFolderHandler/ProgramDataFolderHandler.h"
#include "InsanityEffect/SoundTurnedDownEffect/SoundTurnedDownEffect.h"
#include "InsanityEffect/GammaEffect/GammaEffect.h"
#include "InsanityEffect/FlashlightFlashingEffect/FlashlightFlashingEffect.h"
#include "InsanityEffect/ButtonDisabledEffect/ButtonDisabledEffect.h"
#include "InsanityEffect/BlackAndWhiteEffect/BlackAndWhiteEffect.h"
#include "InsanityEffect/SoundSourceEffect/SoundSourceEffect.h"
#include "InsanityEffect/PossessedCharacterEffect/PossessedCharacterEffect.h"
#include "InsanityEffect/MonsterArmsEffect/MonsterArmsEffect.h"
#include "BerserkerEnergyRegenerationSystem/BerserkerEnergyRegenerationSystem.h"
#include "BerserkerTeleportSystem/BerserkerTeleportSystem.h"
#include "AutomaticLoginHandler/AutomaticLoginHandler.h"
#include "SteamStatsAndAchievements/SteamStatsAndAchievements.h"
#include "SteamTransformationAchievementSystem/SteamTransformationAchievementSystem.h"

using namespace terror;

void main() {
	SharedPointer<Ogre::LogManager> logManager(new Ogre::LogManager);
	logManager->createLog("Damned.log", true, false, false);
	try{
		{ // hack to make windows ask for permission to transfer data via web;
			IoService ioService;
			UdpSocket permissionRequester(ioService, UdpEndpoint(Udp::v4(), 44555));
		}
		SteamSystemPtr steamSystem = SteamSystem::getInstance();
		initializeSteamStatsAndAchievements();
		ProgramDataFolderHandlerPtr programDataFolderHandler(new ProgramDataFolderHandler);
		string baseDirectory = "./DamnedData";
		CorePtr core = Core::getInstance();
		core->initialize("Damned", programDataFolderHandler->getConfigDir(), baseDirectory);			
		core->addResourceDirectoryRecursively(baseDirectory + "/Resources/");
		core->parseScripts();

		core->createEventSystem("Menu");
		core->createEventSystem("Ingame");

		core->createInputSystem("Menu");
		core->createInputSystem("Pause");
		core->createInputSystem("Ingame");
		
		core->createSoundSystem("VoiceChat");
		core->createSoundSystem("AmbientMusic");
		
		CoreStatePtr menuState(new CoreState("Menu"));
		menuState->addActiveEventSystem("Menu");
		menuState->addActiveInputSystem("Menu");
		menuState->addActiveSoundSystem("VoiceChat");
		menuState->addActiveSoundSystem("AmbientMusic");
		core->addState(menuState);

		CoreStatePtr pauseState(new CoreState("Pause"));
		pauseState->addActiveEventSystem("Ingame");
		pauseState->addActiveInputSystem("Pause");
		pauseState->addActiveSoundSystem("VoiceChat");
		pauseState->addActiveSoundSystem("AmbientMusic");
		core->addState(pauseState);

		CoreStatePtr ingameState(new CoreState("Ingame"));
		ingameState->addActiveEventSystem("Ingame");
		ingameState->addActiveInputSystem("Ingame");
		ingameState->addActiveSoundSystem("VoiceChat");
		ingameState->addActiveSoundSystem("AmbientMusic");
		core->addState(ingameState);

		CoreStatePtr pregameState(new CoreState("Pregame"));
		pregameState->addActiveInputSystem("Pause");
		pregameState->addActiveSoundSystem("VoiceChat");
		pregameState->addActiveSoundSystem("AmbientMusic");
		core->addState(pregameState);

		CoreStatePtr posgameState(new CoreState("Posgame"));
		posgameState->addActiveInputSystem("Pause");
		posgameState->addActiveSoundSystem("VoiceChat");
		posgameState->addActiveSoundSystem("AmbientMusic");
		core->addState(posgameState);

		core->switchState("Menu");

		SoundAttenuatorPtr barrierSoundAttenuator(new BarrierSoundAttenuator(core->getPhysicsSystem()->getCollisionTester(), 0.5f));
		core->addSoundAttenuator(barrierSoundAttenuator, "Default");

		SceneManagerPtr sceneManager = core->getSceneManager();
		Ogre::AxisAlignedBox* box = new Ogre::AxisAlignedBox(-100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f);
		sceneManager->setOption("Size", box);
		int* depth = new int(1);
		sceneManager->setOption("Depth", depth);

		sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		sceneManager->setShadowTextureCasterMaterial("ShadowCaster");
		sceneManager->setShadowTextureCount(1);
		sceneManager->setShadowFarDistance(20.0f);			

		RenderCameraPtr camera = core->getGraphicsSystem()->getCamera();
		camera->setNearClipDistance(0.01f);
		camera->setFarClipDistance(25.0f);
		camera->setUseRenderingDistance(true);

		TerrorMaterialGeneratorImplPtr terrorMaterialGeneratorImpl(new TerrorMaterialGeneratorImpl);
		TerrorPassPropertiesFactoryPtr terrorPassPropertiesFactory(new TerrorPassPropertiesFactory);
		DeferredShadingSystemPtr deferredShadingSystem(new DeferredShadingSystem(camera->getViewport(), terrorMaterialGeneratorImpl, terrorPassPropertiesFactory));

		ScreenshotHandlerPtr screenshotHandler(new ScreenshotHandler(core->getGraphicsSystem()->getRenderWindow(), programDataFolderHandler->getScreenshotsDir()));
		core->addInputHandler(screenshotHandler, "Default");

		SceneBuilderPtr sceneBuilder(new SceneBuilder(sceneManager, "Default", "AmbientMusic"));
		SceneFactoryPtr sceneFactory(new SceneFactory(sceneManager));

		NodePtr menuSceneNode = core->getParseTreeFactory()->create("menu_background.scene")->getChildByType("scene", true);
		SceneDataPtr menuSceneData = sceneBuilder->build(menuSceneNode);
		MenuSceneLoaderPtr menuSceneLoader(new MenuSceneLoader(core, sceneFactory, menuSceneData));	
		core->addEventHandler(menuSceneLoader, "Menu");
		menuSceneLoader->loadMenuScene();

		TerrorSavePtr terrorSave(new TerrorSave(programDataFolderHandler->getSaveDir() + "/terror.save"));
		terrorSave->load();		

		LoginSaverPtr loginSaver(new LoginSaver(programDataFolderHandler->getSaveDir() + "/data.save"));

		PostProcessingSystemPtr postProcessingSystem = core->getGraphicsSystem()->getPostProcessingSystem();
		GraphicsOptionsPtr graphicsOptions = core->getGraphicsSystem()->getGraphicsOptions();

		ExitTriggerPtr exitTrigger(new ExitTrigger);
		core->addEventHandler(exitTrigger, "Default");

		PlayerProfilePtr playerProfile(new PlayerProfile);

		PauseHandlerPtr pauseHandler(new PauseHandler);

		ParticleFxHandlerPtr particleFxHandler(new ParticleFxHandler(sceneManager, terrorSave));

		GuiSystemPtr guiSystem = core->getGuiSystem();

		GuiLayoutPtr mainLayout = loadLayout("Title.layout");
		mainLayout->show();
		mainLayout->disable();
		GuiLayoutPtr loginLayout = loadLayout("Login.layout");
		loginLayout->show();
		GuiLayoutPtr crosshairLayout = loadLayout("Crosshair.layout");
		crosshairLayout->show();
		GuiLayoutPtr hostLayout = loadLayout("Host.layout");
		GuiLayoutPtr joinLayout = loadLayout("Join.layout");
		GuiLayoutPtr gamePasswordLayout = loadLayout("GamePassword.layout");
		GuiLayoutPtr pauseLayout = loadLayout("Pause.layout");
		GuiLayoutPtr optionsLayout = loadLayout("Options.layout");
		GuiLayoutPtr optionsIngameLayout = loadLayout("OptionsIngame.layout");
		GuiLayoutPtr graphicsLayout = loadLayout("Graphics.layout");
		GuiLayoutPtr gammaLayout = loadLayout("Gamma.layout");
		GuiLayoutPtr soundLayout = loadLayout("Sound.layout");
		GuiLayoutPtr controlsLayout = loadLayout("Controls.layout");
		GuiLayoutPtr profileLayout = loadLayout("_Profile.layout");
		GuiLayoutPtr connectionLayout = loadLayout("Connection.layout");		
		GuiLayoutPtr resetPasswordLayout = loadLayout("Forgot.layout");
		GuiLayoutPtr warningLayout = loadLayout("Warning.layout");
		GuiLayoutPtr activationLayout = loadLayout("Activation.layout");
		GuiLayoutPtr registerLayout = loadLayout("Register.layout");
		GuiLayoutPtr lobbyLayout = loadLayout("Lobby.layout");
		lobbyLayout->setParent(mainLayout);
		GuiLayoutPtr loadingLayout = loadLayout("Loading.layout");
		GuiLayoutPtr creditsLayout = loadLayout("Credits.layout");
		GuiLayoutPtr characterSelectionLayout = loadLayout("_CharacterSelection.layout");
		GuiLayoutPtr allowedMonstersLayout = loadLayout("AllowedMonsters.layout");
		GuiLayoutPtr stageSelectionLayout = loadLayout("StageSelection.layout");
		GuiLayoutPtr chatLayout = loadLayout("Chat.layout");
		GuiLayoutPtr startGameLayout = loadLayout("StartGame.layout");
		GuiLayoutPtr endGameLayout = loadLayout("EndGame.layout");
		loadingLayout->connectShowEvent(Bind(&GuiLayout::hide, characterSelectionLayout, false));

		//Debug Drawer!!
		//DebugDrawerPtr debugDrawer = core->createDebugDrawer();		
		//DebugInputHandlerPtr debugInputHandler(new DebugInputHandler(debugDrawer, deferredShadingSystem));
		//core->addInputHandler(debugInputHandler, "Default");

		CrashReportSystemPtr crashReportSystem = CrashReportSystem::getInstance();

		TcpClientPtr tcpClient(new TcpClient);		
		UdpPeerPtr udpPeer(new UdpPeer);				

		UdpAddressPtr serverUdpAddress;
		try {	
			serverUdpAddress = UdpAddressPtr(new AsioUdpAddress("9heads.ddns.net", 55444));
		}catch (std::exception&) {
			ServerDownWindowHandlerPtr serverDownWindowHandler(new ServerDownWindowHandler(loginLayout, exitTrigger));
			core->run();
			return;
		}	

		KeepAliveClientPtr keepAliveClient(new KeepAliveClient(serverUdpAddress, 20.0f, 5.0f));
		tcpClient->addTcpPacketObserver(keepAliveClient);
		udpPeer->addUdpPacketObserver(keepAliveClient);

		ConnectionClientPtr connectionClient(new ConnectionClient(serverUdpAddress));
		tcpClient->addTcpPacketObserver(connectionClient);
		udpPeer->addUdpPacketObserver(connectionClient);

		PingClientPtr pingClient(new PingClient);
		tcpClient->addTcpPacketObserver(pingClient);
		udpPeer->addUdpPacketObserver(pingClient);

		UdpPunchThroughClientPtr udpPunchThroughClient(new UdpPunchThroughClient);
		tcpClient->addTcpPacketObserver(udpPunchThroughClient);
		udpPeer->addUdpPacketObserver(udpPunchThroughClient);		

		CrashReportClientPtr crashReportClient(new CrashReportClient(TERROR_VERSION));
		tcpClient->addTcpPacketObserver(crashReportClient);
		crashReportSystem->addCrashReportObserver(crashReportClient);

		TerrorClientPtr terrorClient(new TerrorClient(udpPunchThroughClient, serverUdpAddress));
		tcpClient->addTcpPacketObserver(terrorClient);
		udpPeer->addUdpPacketObserver(terrorClient);

		if (!terrorSave->useFixedPort) {
			udpPeer->initialize();
		}else {
			udpPeer->initialize(terrorSave->fixedPort);
		}

		try {	
			tcpClient->initialize("9heads.ddns.net", 55444);
		}catch (std::exception&) {
			ServerDownWindowHandlerPtr serverDownWindowHandler(new ServerDownWindowHandler(loginLayout, exitTrigger));
			core->run();
			return;
		}		

		core->addEventHandler(terrorClient, "Default");

		StageHandlerPtr stageHandler(new StageHandler(core, sceneBuilder, sceneFactory, menuSceneLoader, terrorClient));
		terrorClient->addNetworkObserver(stageHandler);
		core->addEventHandler(stageHandler, "Default");

		CharacterHandlerPtr characterHandler(new CharacterHandler);

		VoiceChatHandlerPtr voiceChatHandler(new VoiceChatHandler(characterHandler, terrorSave->controls, terrorClient));
		SoundAttenuatorPtr voiceChatDefaultSoundAttenuator(new DefaultSoundAttenuator(2.0f, 100.0f, 0.1f));
		voiceChatHandler->addSoundAttenuator(voiceChatDefaultSoundAttenuator);
		SoundAttenuatorPtr voiceChatBarrierSoundAttenuator(new BarrierSoundAttenuator(core->getPhysicsSystem()->getCollisionTester(), 0.5f));
		voiceChatHandler->addSoundAttenuator(voiceChatBarrierSoundAttenuator);		
		stageHandler->connectStageLoadedEvent(Bind(&VoiceChatHandler::notifyStageLoaded, voiceChatHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&VoiceChatHandler::notifyStageUnloaded, voiceChatHandler));
		core->addEventHandler(voiceChatHandler, "Default");
		core->addInputHandler(voiceChatHandler, "Default");
		terrorClient->addNetworkObserver(voiceChatHandler);

		KickHandlerPtr kickHandler(new KickHandler(mainLayout, pauseLayout, loadingLayout, lobbyLayout, stageHandler, terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&KickHandler::notifyStageLoaded, kickHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&KickHandler::notifyStageUnloaded, kickHandler));
		terrorClient->addNetworkObserver(kickHandler);

		EndGameHandlerPtr endGameHandler(new EndGameHandler(endGameLayout, lobbyLayout, chatLayout, loadingLayout, stageHandler, pauseHandler, terrorClient));		
		terrorClient->addNetworkObserver(endGameHandler);
		core->addInputHandler(endGameHandler, "Pause");
		stageHandler->connectStageLoadedEvent(Bind(&EndGameHandler::notifyStageLoaded, endGameHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&EndGameHandler::notifyStageUnloaded, endGameHandler));

		GameModeSystemPtr gameModeSystem(new GameModeSystem(endGameHandler, characterHandler, terrorClient));
		core->addEventHandler(static_ptr_cast<LifeMode>(gameModeSystem->getGameModes()[LIFE_MODE]));
		stageHandler->connectStageLoadedEvent(Bind(&LifeMode::notifyStageLoaded, static_ptr_cast<LifeMode>(gameModeSystem->getGameModes()[LIFE_MODE]), _1));
		stageHandler->connectStageUnloadedEvent(Bind(&LifeMode::notifyStageUnloaded, static_ptr_cast<LifeMode>(gameModeSystem->getGameModes()[LIFE_MODE])));

		DeathSystemPtr deathSystem(new DeathSystem(characterHandler, terrorClient, gameModeSystem, pauseHandler));
		stageHandler->connectStageLoadedEvent(Bind(&DeathSystem::notifyStageLoaded, deathSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&DeathSystem::notifyStageUnloaded, deathSystem));
		deathSystem->connectHumanDeathEvent(Bind(&CharacterHandler::notifyHumanDeath, characterHandler, _1));
		terrorClient->addNetworkObserver(deathSystem);

		SoundPtr hearingSound = core->createSound("blind_hearing.ogg", "AmbientMusic");
		hearingSound->setRepeat(true);
		SoundPtr notHearingSound = core->createSound("blind_music.ogg", "AmbientMusic");
		notHearingSound->setRepeat(true);
		////Tentative change, Original: monster_closer.ogg
		SoundPtr fleeingSound = core->createSound("blind_hearing.ogg");
		fleeingSound->setRepeat(true);

		BlindMonsterHearingSystemPtr blindMonsterHearingSystem(
			new BlindMonsterHearingSystem(hearingSound, notHearingSound, fleeingSound, characterHandler, terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&BlindMonsterHearingSystem::notifyStageLoaded, blindMonsterHearingSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&BlindMonsterHearingSystem::notifyStageUnloaded, blindMonsterHearingSystem));
		core->addEventHandler(blindMonsterHearingSystem, "Ingame");
		terrorClient->addNetworkObserver(blindMonsterHearingSystem);

		ItemSystemPtr itemSystem(new ItemSystem(characterHandler, terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&ItemSystem::notifyStageLoaded, itemSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&ItemSystem::notifyStageUnloaded, itemSystem));
		terrorClient->addNetworkObserver(itemSystem);

		CollisionTesterPtr collisionTester = core->getPhysicsSystem()->getCollisionTester();
		ScareTrapSystemPtr scareTrapSystem(
			new ScareTrapSystem(blindMonsterHearingSystem, characterHandler, collisionTester, terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&ScareTrapSystem::notifyStageLoaded, scareTrapSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&ScareTrapSystem::notifyStageUnloaded, scareTrapSystem));
		core->addEventHandler(scareTrapSystem, "Ingame");
		terrorClient->addNetworkObserver(scareTrapSystem);	

		ManipulationSystemPtr manipulationSystem(new ManipulationSystem(characterHandler, core->getPhysicsSystem(), terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&ManipulationSystem::notifyStageLoaded, manipulationSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&ManipulationSystem::notifyStageUnloaded, manipulationSystem));
		terrorClient->addNetworkObserver(manipulationSystem);

		SpectatorSystemPtr spectatorSystem(new SpectatorSystem(characterHandler, camera, terrorClient, terrorSave));
		stageHandler->connectStageLoadedEvent(Bind(&SpectatorSystem::notifyStageLoaded, spectatorSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&SpectatorSystem::notifyStageUnloaded, spectatorSystem));
		core->addInputHandler(spectatorSystem, "Ingame");
		
		PostProcessingEffectPtr monsterInFovView = postProcessingSystem->addPostProcessingEffect("MonsterInFovView");
		InsanitySystemPtr insanitySystem(new InsanitySystem(characterHandler, collisionTester, camera, monsterInFovView));
		core->addEventHandler(insanitySystem, "Ingame");
		stageHandler->connectStageUnloadedEvent(Bind(&InsanitySystem::notifyStageUnloaded, insanitySystem));
		deathSystem->connectHumanDeathEvent(Bind(&InsanitySystem::notifyHumanDeath, insanitySystem, _1));
		manipulationSystem->connectLockedObjectEvent(Bind(&InsanitySystem::notifyLockedObject, insanitySystem, _1));
		scareTrapSystem->connectScareTrapTriggeredEvent(Bind(&InsanitySystem::notifyScareTrapTriggered, insanitySystem, _1));

		BerserkerEnergyRegenerationSystemPtr berserkerEnergyRegenerationSystem(new BerserkerEnergyRegenerationSystem(characterHandler, collisionTester));		
		core->addEventHandler(berserkerEnergyRegenerationSystem, "Ingame");

		SoundPtr teleportSound = core->createSound("berserk_teleport.ogg");
		BerserkerTeleportSystemPtr berserkerTeleportSystem(new BerserkerTeleportSystem(characterHandler, terrorClient, collisionTester, camera, teleportSound));		
		terrorClient->addNetworkObserver(berserkerTeleportSystem);
		core->addEventHandler(berserkerTeleportSystem, "Ingame");

		StatueClonesSystemPtr statueClonesSystem(new StatueClonesSystem(terrorClient));
		terrorClient->addNetworkObserver(statueClonesSystem);
		core->addEventHandler(statueClonesSystem, "Ingame");

		CharacterLoaderPtr characterLoader(new CharacterLoader(characterHandler, statueClonesSystem, scareTrapSystem, deathSystem, itemSystem, manipulationSystem, 
			spectatorSystem, insanitySystem, particleFxHandler, berserkerEnergyRegenerationSystem, berserkerTeleportSystem, terrorSave, core));
		characterLoader->connectLocalPlayerMonsterLoadedEvent(Bind(&StageHandler::notifyLocalPlayerMonsterLoaded, stageHandler, _1));		

		GameStateHandlerPtr gameStateHandler(
			new GameStateHandler(core->getPhysicsSystem(), deathSystem, characterHandler, characterLoader, terrorClient, core));
		stageHandler->connectStageLoadedEvent(Bind(&GameStateHandler::setStage, gameStateHandler, _1));
		deathSystem->connectHumanDeathEvent(Bind(&GameStateHandler::notifyHumanDeath, gameStateHandler, _1));
		terrorClient->addNetworkObserver(gameStateHandler);
		static_ptr_cast<LifeMode>(gameModeSystem->getGameModes()[LIFE_MODE])->connectLocalHumanRespawningEvent(Bind(&GameStateHandler::notifyLocalHumanRespawning, gameStateHandler, _1));

		GameNetworkHandlerPtr gameNetworkHandler(new GameNetworkHandler(terrorClient, gameStateHandler, characterLoader, manipulationSystem));
		stageHandler->connectStageLoadedEvent(Bind(&GameNetworkHandler::notifyStageLoaded, gameNetworkHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&GameNetworkHandler::notifyStageUnloaded, gameNetworkHandler));
		deathSystem->connectHumanDeathEvent(Bind(&GameNetworkHandler::notifyHumanDeath, gameNetworkHandler, _1));
		terrorClient->addNetworkObserver(gameNetworkHandler);
		core->addEventHandler(gameNetworkHandler, "Default");

		stageHandler->connectStageLoadedEvent(Bind(&BerserkerEnergyRegenerationSystem::notifyStageLoaded, berserkerEnergyRegenerationSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&BerserkerEnergyRegenerationSystem::notifyStageUnloaded, berserkerEnergyRegenerationSystem));
		stageHandler->connectStageLoadedEvent(Bind(&BerserkerTeleportSystem::notifyStageLoaded, berserkerTeleportSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&BerserkerTeleportSystem::notifyStageUnloaded, berserkerTeleportSystem));
		stageHandler->connectStageUnloadedEvent(Bind(&StatueClonesSystem::notifyStageUnloaded, statueClonesSystem));

		SoundFactory soundFactory(core->getResourceFinder());
		SoundPtr beep = soundFactory.createSound("beep.ogg", false, SFML_SOUND);
		beep->setRepeat(false);
		InsanityEffectPtr soundTurnedDownEffect(new SoundTurnedDownEffect(beep, terrorSave));
		insanitySystem->connectInsanityEffect(soundTurnedDownEffect);

		SoundPtr maleGasping = core->createSound("gasping.ogg");
		maleGasping->setRepeat(false);
		SoundPtr femaleGasping = core->createSound("gasping_girl.ogg");
		femaleGasping->setRepeat(false);
		InsanityEffectPtr gammaEffect(new GammaEffect(deferredShadingSystem, terrorClient, terrorSave, maleGasping, femaleGasping));
		insanitySystem->connectInsanityEffect(gammaEffect);

		SoundPtr flashlightSwitchSound = core->createSound("flashlight_switch.ogg");
		InsanityEffectPtr flashlightFlashingEffect(new FlashlightFlashingEffect(flashlightSwitchSound, characterHandler, terrorClient));
		insanitySystem->connectInsanityEffect(flashlightFlashingEffect);

		InsanityEffectPtr monsterArmsEffect(new MonsterArmsEffect(collisionTester, characterHandler, terrorClient, femaleGasping, maleGasping));
		insanitySystem->connectInsanityEffect(monsterArmsEffect);
		static_ptr_cast<MonsterArmsEffect>(monsterArmsEffect)->connectMonsterArmContactEvent(Bind(&InsanitySystem::notifyMonsterArmContact, insanitySystem));
		stageHandler->connectStageLoadedEvent(Bind(&MonsterArmsEffect::notifyStageLoaded, static_ptr_cast<MonsterArmsEffect>(monsterArmsEffect), _1));
		stageHandler->connectStageUnloadedEvent(Bind(&MonsterArmsEffect::notifyStageUnloaded, static_ptr_cast<MonsterArmsEffect>(monsterArmsEffect)));

		SoundPtr monsterSound = core->createSound("monster_spawn3.ogg");
		monsterSound->setRepeat(false);
		InsanityEffectPtr buttonDisabledEffect(new ButtonDisabledEffect(gameStateHandler, monsterSound, terrorSave->controls));
		insanitySystem->connectInsanityEffect(buttonDisabledEffect);

		PostProcessingEffectPtr blackAndWhiteView = postProcessingSystem->addPostProcessingEffect("BlackAndWhiteView");
		SoundPtr breathSound = core->createSound("breath2.ogg");
		breathSound->setRepeat(false);
		InsanityEffectPtr blackAndWhiteEffect(new BlackAndWhiteEffect(blackAndWhiteView, breathSound, characterHandler));
		insanitySystem->connectInsanityEffect(blackAndWhiteEffect);

		SoundSourcePtr insanitySoundSource = core->createSoundSource();
		InsanityEffectPtr soundSourceEffect(new SoundSourceEffect(insanitySoundSource, characterHandler, terrorClient));
		insanitySystem->connectInsanityEffect(soundSourceEffect);

		PostProcessingEffectPtr possessedCharViews[] = {
			postProcessingSystem->addPostProcessingEffect("PossessedCharView0"),
			postProcessingSystem->addPostProcessingEffect("PossessedCharView1"),
			postProcessingSystem->addPostProcessingEffect("PossessedCharView2"),
			postProcessingSystem->addPostProcessingEffect("PossessedCharView3")
		};
		SoundPtr interferenceSound = core->createSound("interference.ogg");
		interferenceSound->setRepeat(false);
		InsanityEffectPtr possessedCharacterEffect(new PossessedCharacterEffect(possessedCharViews, interferenceSound));
		insanitySystem->connectInsanityEffect(possessedCharacterEffect);
				
		berserkerTeleportSystem->connectMonsterTeleportedEvent(Bind(&ScareTrapSystem::notifyMonsterTeleported, scareTrapSystem));
		berserkerTeleportSystem->connectMonsterTeleportedEvent(Bind(&GameNetworkHandler::notifyMonsterTeleported, gameNetworkHandler));

		AutomaticLoginHandlerPtr automaticLoginHandler(new AutomaticLoginHandler(loginLayout, mainLayout, activationLayout, loginSaver, playerProfile, terrorClient));
		terrorClient->addNetworkObserver(automaticLoginHandler);
		automaticLoginHandler->tryLogin();

		TitleWindowHandlerPtr titleWindowHandler(
			new TitleWindowHandler(mainLayout, hostLayout, joinLayout, optionsLayout, loginLayout, creditsLayout, terrorClient, exitTrigger, loginSaver, playerProfile));

		CreditsWindowHandlerPtr creditsWindowHandler(
			new CreditsWindowHandler(creditsLayout, mainLayout, terrorSave));
		core->addInputHandler(creditsWindowHandler, "Menu");

		OptionsWindowHandlerPtr optionsWindowHandler(
			new OptionsWindowHandler(optionsLayout, graphicsLayout, soundLayout, controlsLayout, profileLayout, connectionLayout, terrorSave));
		OptionsWindowHandlerPtr optionsIngameWindowHandler(
			new OptionsWindowHandler(optionsIngameLayout, graphicsLayout, soundLayout, controlsLayout, connectionLayout, terrorSave));
		core->addInputHandler(optionsWindowHandler, "Menu");
		core->addInputHandler(optionsIngameWindowHandler, "Pause");
		GraphicsWindowHandlerPtr graphicsWindowHandler(
			new GraphicsWindowHandler(graphicsLayout, gammaLayout, graphicsOptions, deferredShadingSystem, sceneManager, particleFxHandler, terrorSave));
		core->addInputHandler(graphicsWindowHandler, "Menu");
		core->addInputHandler(graphicsWindowHandler, "Pause");
		GammaWindowHandlerPtr gammaWindowHandler(new GammaWindowHandler(gammaLayout, postProcessingSystem, deferredShadingSystem, terrorSave));
		core->addInputHandler(gammaWindowHandler, "Menu");
		core->addInputHandler(gammaWindowHandler, "Pause");
		SoundWindowHandlerPtr soundWindowHandler(new SoundWindowHandler(soundLayout, voiceChatHandler, terrorSave));
		core->addInputHandler(soundWindowHandler, "Menu");
		core->addInputHandler(soundWindowHandler, "Pause");
		ControlsWindowHandlerPtr controlsWindowHandler(new ControlsWindowHandler(controlsLayout, terrorSave, core->getButtonTranslator(), guiSystem));
		core->addInputHandler(controlsWindowHandler, "Menu");
		core->addInputHandler(controlsWindowHandler, "Pause");

		ConnectionWindowHandlerPtr connectionWindowHandler(new ConnectionWindowHandler(connectionLayout, terrorSave));
		core->addInputHandler(connectionWindowHandler, "Menu");
		core->addInputHandler(connectionWindowHandler, "Pause");

		PauseWindowHandlerPtr pauseWindowHandler(
			new PauseWindowHandler(pauseLayout, mainLayout, loadingLayout, optionsIngameLayout, startGameLayout, pauseHandler, stageHandler, terrorSave->controls, exitTrigger, terrorClient));
		core->addInputHandler(pauseWindowHandler, "Default");
		stageHandler->connectStageLoadedEvent(Bind(&PauseWindowHandler::notifyStageLoaded, pauseWindowHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&PauseWindowHandler::notifyStageUnloaded, pauseWindowHandler));

		ProfileWindowHandlerPtr profileWindowHandler(new ProfileWindowHandler(profileLayout, playerProfile, terrorClient, terrorSave));
		core->addInputHandler(profileWindowHandler, "Menu");
		core->addInputHandler(profileWindowHandler, "Pause");
		terrorClient->addNetworkObserver(profileWindowHandler);

		LoginWindowHandlerPtr loginWindowHandler(
			new LoginWindowHandler(loginLayout, mainLayout, activationLayout, registerLayout, resetPasswordLayout, optionsLayout, terrorClient, playerProfile, exitTrigger, loginSaver));
		terrorClient->addNetworkObserver(loginWindowHandler);

		WarningMessageHandlerPtr warningMessageHandler(new WarningMessageHandler(warningLayout));
		ResetPasswordWindowHandlerPtr resetPasswordWindowHandler(new ResetPasswordWindowHandler(resetPasswordLayout, terrorClient, warningMessageHandler, terrorSave));
		terrorClient->addNetworkObserver(resetPasswordWindowHandler);
		core->addInputHandler(resetPasswordWindowHandler);

		ActivationWindowHandlerPtr activationWindowHandler(new ActivationWindowHandler(activationLayout, mainLayout, loginLayout, terrorClient, playerProfile, exitTrigger, loginSaver));
		terrorClient->addNetworkObserver(activationWindowHandler);

		RegisterWindowHandlerPtr registerWindowHandler(new RegisterWindowHandler(registerLayout, loginLayout, terrorClient, terrorSave));
		terrorClient->addNetworkObserver(registerWindowHandler);
		core->addInputHandler(registerWindowHandler);

		SoundPtr loadGameSound = core->createSound("lobby_start_game.ogg");
		loadGameSound->setRepeat(false);
		StageSelectionWindowHandlerPtr stageSelectionWindowHandler(new StageSelectionWindowHandler(lobbyLayout, stageSelectionLayout, terrorClient));
		core->addEventHandler(stageSelectionWindowHandler, "Menu");
		LobbyWindowHandlerPtr lobbyWindowHandler(
			new LobbyWindowHandler(lobbyLayout, chatLayout, characterSelectionLayout, loadingLayout, allowedMonstersLayout, stageSelectionLayout, pauseLayout,
			stageSelectionWindowHandler, loadGameSound, terrorClient, stageHandler, kickHandler, terrorSave)
			);
		terrorClient->addNetworkObserver(lobbyWindowHandler);
		core->addInputHandler(lobbyWindowHandler, "Menu");
		stageHandler->connectStageLoadedEvent(Bind(&LobbyWindowHandler::notifyStageLoaded, lobbyWindowHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&LobbyWindowHandler::notifyStageUnloaded, lobbyWindowHandler));

		SoundPtr playerConnectedSound = core->createSound("lobby_enter.ogg");
		CharacterSelectionWindowHandlerPtr characterSelectionWindowHandler(
			new CharacterSelectionWindowHandler(lobbyLayout, characterSelectionLayout, terrorClient, gameStateHandler, characterLoader, playerProfile, playerConnectedSound)
			);
		terrorClient->addNetworkObserver(characterSelectionWindowHandler);
		stageHandler->connectStageLoadedEvent(Bind(&CharacterSelectionWindowHandler::notifyStageLoaded, characterSelectionWindowHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&CharacterSelectionWindowHandler::notifyStageUnloaded, characterSelectionWindowHandler));
		mainLayout->connectShowEvent(Bind(&CharacterSelectionWindowHandler::reset, characterSelectionWindowHandler));

	    AllowedMonstersWindowHandlerPtr allowedMonstersWindowHandler(new AllowedMonstersWindowHandler(allowedMonstersLayout, terrorClient));

		GameSettingsPtr gameSettings = GameSettings::getInstance();
		gameSettings->connectUpdatedEvent(Bind(&LobbyWindowHandler::notifyGameSettingsUpdated, lobbyWindowHandler));
		gameSettings->connectUpdatedEvent(Bind(&CharacterSelectionWindowHandler::notifyGameSettingsUpdated, characterSelectionWindowHandler));

		ChatWindowHandlerPtr chatWindowHandler(new ChatWindowHandler(chatLayout, terrorClient, pauseHandler, terrorSave));
		core->addInputHandler(chatWindowHandler, "Default");
		terrorClient->addNetworkObserver(chatWindowHandler);
		stageHandler->connectStageLoadedEvent(Bind(&ChatWindowHandler::notifyStageLoaded, chatWindowHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&ChatWindowHandler::notifyStageUnloaded, chatWindowHandler));
		lobbyLayout->connectShowEvent(Bind(&ChatWindowHandler::reset, chatWindowHandler));
		lobbyLayout->connectShowEvent(Bind(&GuiLayout::hide, characterSelectionLayout, false));
		lobbyLayout->connectShowEvent(Bind(&GuiLayout::hide, stageSelectionLayout, false));
		lobbyLayout->connectShowEvent(Bind(&GuiLayout::hide, allowedMonstersLayout, false));

		HostGameWindowHandlerPtr hostGameWindowHandler(new HostGameWindowHandler(hostLayout, lobbyLayout, chatLayout, terrorClient, terrorSave));
		core->addInputHandler(hostGameWindowHandler, "Menu");

		MyGUI::MultiListBox* gameSessionsListbox = static_cast<MyGUI::MultiListBox*>(joinLayout->getWidget("GameSessionsMultiList"));
		GameSessionListboxHandlerPtr gameSessionListboxHandler(
			new GameSessionListboxHandler(joinLayout->getWidget("RefreshHostsButton"), gameSessionsListbox, terrorClient));
		terrorClient->addNetworkObserver(gameSessionListboxHandler);

		JoinGameWindowHandlerPtr joinGameWindowHandler(
			new JoinGameWindowHandler(joinLayout, gamePasswordLayout, lobbyLayout, chatLayout, terrorClient, terrorSave));
		core->addInputHandler(joinGameWindowHandler, "Menu");
		terrorClient->addNetworkObserver(joinGameWindowHandler);
		stageHandler->connectStageLoadedEvent(Bind(&JoinGameWindowHandler::notifyStageLoaded, joinGameWindowHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&JoinGameWindowHandler::notifyStageUnloaded, joinGameWindowHandler));

		SoundPtr startGameSound = core->createSound("game_start.ogg");
		startGameSound->setRepeat(false);
		StartGameHandlerPtr startGameHandler(new StartGameHandler(startGameLayout, pauseLayout, startGameSound, pauseHandler, terrorClient, terrorSave, core->getButtonTranslator()));
		stageHandler->connectStageLoadedEvent(Bind(&StartGameHandler::notifyStageLoaded, startGameHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&StartGameHandler::notifyStageUnloaded, startGameHandler));
		core->addInputHandler(startGameHandler, "Pause");
		terrorClient->addNetworkObserver(startGameHandler);

		PlayersStatsHandlerPtr playersStatsHandler(new PlayersStatsHandler(kickHandler, terrorClient, terrorSave));
		stageHandler->connectStageLoadedEvent(Bind(&PlayersStatsHandler::notifyStageLoaded, playersStatsHandler, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&PlayersStatsHandler::notifyStageUnloaded, playersStatsHandler));
		core->addInputHandler(playersStatsHandler, "Ingame");
		core->addInputHandler(playersStatsHandler, "Pause");
		terrorClient->addNetworkObserver(playersStatsHandler);		

		GameEndingRoomSystemPtr gameEndingRoomSystem(new GameEndingRoomSystem(endGameHandler, characterHandler, terrorClient));
		stageHandler->connectStageLoadedEvent(Bind(&GameEndingRoomSystem::notifyStageLoaded, gameEndingRoomSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&GameEndingRoomSystem::notifyStageUnloaded, gameEndingRoomSystem));
		core->addEventHandler(gameEndingRoomSystem, "Ingame");		

		SteamTransformationAchievementsSystemPtr steamTransformationAchievementsSystem(new SteamTransformationAchievementsSystem(terrorClient));
		characterLoader->connectLocalPlayerMonsterLoadedEvent(Bind(&SteamTransformationAchievementsSystem::notifyLocalPlayerMonsterLoaded, steamTransformationAchievementsSystem, _1));
		stageHandler->connectStageUnloadedEvent(Bind(&SteamTransformationAchievementsSystem::notifyStageUnloaded, steamTransformationAchievementsSystem));
		deathSystem->connectHumanDeathEvent(Bind(&SteamTransformationAchievementsSystem::notifyHumanDeath, steamTransformationAchievementsSystem, _1));

		MessageSystemPtr messageSystem = MessageSystem::getInstance();
		core->addEventHandler(messageSystem, "Default");

		ItemDatabasePtr itemDatabase = ItemDatabase::getInstance();
		itemDatabase->initialize();
		
		core->run();
	}catch(std::exception& e) {
		logManager->logMessage(e.what());
		std::cout << "\nDamned closed due to an error!" <<
			"If this was unexpected, send the content of the file Damned.log under the game folder to contact@9heads.com with the title \"Damned Error File\".";
		system("PAUSE");
	}
}