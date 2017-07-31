#ifndef TERROR_CHARACTER_LOADER_H
#define TERROR_CHARACTER_LOADER_H

#include "ParticleFxHandler/ParticleFxHandler.h"
#include "HumanController/LocalPlayerHumanController/LocalPlayerHumanController.h"
#include "HumanController/RemotePlayerHumanController/RemotePlayerHumanController.h"
#include "MonsterControllers/SpectrumController/LocalSpectrumController/LocalPlayerSpectrumController/LocalPlayerSpectrumController.h"
#include "MonsterControllers/SpectrumController/LocalSpectrumController/LocalAiSpectrumController/LocalAiSpectrumController.h"
#include "MonsterControllers/SpectrumController/RemotePlayerSpectrumController/RemotePlayerSpectrumController.h"
#include "MonsterControllers/BlindController/LocalBlindController/LocalPlayerBlindController/LocalPlayerBlindController.h"
#include "MonsterControllers/BlindController/LocalBlindController/LocalAiBlindController/LocalAiBlindController.h"
#include "MonsterControllers/BlindController/RemotePlayerBlindController/RemotePlayerBlindController.h"
#include "MonsterControllers/BerserkerController/LocalBerserkerController/LocalPlayerBerserkerController/LocalPlayerBerserkerController.h"
#include "MonsterControllers/BerserkerController/LocalBerserkerController/LocalAiBerserkerController/LocalAiBerserkerController.h"
#include "MonsterControllers/BerserkerController/RemotePlayerBerserkerController/RemotePlayerBerserkerController.h"
#include "MonsterControllers/StatueController/LocalStatueController/LocalPlayerStatueController/LocalPlayerStatueController.h"
#include "MonsterControllers/StatueController/LocalStatueController/LocalAiStatueController/LocalAiStatueController.h"
#include "MonsterControllers/StatueController/RemotePlayerStatueController/RemotePlayerStatueController.h"
#include "BerserkerEnergyRegenerationSystem/BerserkerEnergyRegenerationSystem.h"
#include "InsanitySystem/InsanitySystem.h"

namespace terror {
	typedef Signal<void (MonsterPtr)> MonsterSignal;

	class CharacterLoader {
	public:
		CharacterLoader(CharacterHandlerPtr characterHandler, StatueClonesSystemPtr statueClonesSystem, ScareTrapSystemPtr scareTrapSystem, DeathSystemPtr deathSystem, ItemSystemPtr itemSystem, 
			ManipulationSystemPtr manipulationSystem, SpectatorSystemPtr spectatorSystem, InsanitySystemPtr insanitySystem, ParticleFxHandlerPtr particleFxHandler, 
			BerserkerEnergyRegenerationSystemPtr berserkerEnergyRegenerationSystem, BerserkerTeleportSystemPtr berserkerTeleportSystem, TerrorSavePtr terrorSave, CorePtr core) 
			: characterHandler(characterHandler), statueClonesSystem(statueClonesSystem), scareTrapSystem(scareTrapSystem), deathSystem(deathSystem), itemSystem(itemSystem), manipulationSystem(manipulationSystem),
			spectatorSystem(spectatorSystem), sprintSystem(new SprintSystem), particleFxHandler(particleFxHandler), berserkerEnergyRegenerationSystem(berserkerEnergyRegenerationSystem), 
			berserkerTeleportSystem(berserkerTeleportSystem), terrorSave(terrorSave), insanitySystem(insanitySystem), core(core)
		{
			this->core->addEventHandler(this->sprintSystem);
		}	

		LocalPlayerHumanControllerPtr loadLocalPlayerHuman(CharacterType character, SpawnPointPtr spawnPoint, bool forest) {			
			ActorPtr actor = this->loadActor(character, spawnPoint);
			actor->setMaterialId(30);
			this->particleFxHandler->setLocalActor(actor);

			FlashlightPtr flashlight = this->loadFlashlight(actor, true);			
			InventoryPtr inventory(new Inventory(true));

			CharacterDataPtr characterData(new CharacterData(actor));
			HumanPtr human = this->loadHuman(characterData, character, flashlight, inventory, forest);
			this->characterHandler->addHumanAlive(human);
			RenderCameraPtr camera = this->core->getGraphicsSystem()->getCamera();

			PostProcessingSystemPtr postProcessingSystem = this->core->getGraphicsSystem()->getPostProcessingSystem();
			PostProcessingEffectPtr humanView = postProcessingSystem->addPostProcessingEffect("HumanView");

			SoundPtr monsterAroundSound = this->core->createSound("monster_around.ogg", "AmbientMusic");
			monsterAroundSound->setRepeat(true);
			SoundPtr monsterCloseSound = this->core->createSound("monster_close.ogg", "AmbientMusic");
			monsterCloseSound->setRepeat(true);
			SoundPtr monsterCloserSound = this->core->createSound("monster_closer.ogg", "AmbientMusic");
			monsterCloserSound->setRepeat(true);

			SoundPtr blindCloseSound = this->core->createSound("blind_close.ogg", "AmbientMusic");
			blindCloseSound->setRepeat(true);

			SoundStreamPtrVector ghostThroughSounds;
			ghostThroughSounds.push_back(this->core->createSound("one_heart.ogg"));
			ghostThroughSounds.push_back(this->core->createSound("breath1.ogg"));
			ghostThroughSounds.push_back(this->core->createSound("breath2.ogg"));
			foreach (SoundPtr sound, ghostThroughSounds) {
				sound->setRepeat(false);
			}

			SoundPtr berserkerInBerserkerStateSound = this->core->createSound("beserk_hunting.ogg", "AmbientMusic");
			berserkerInBerserkerStateSound->setRepeat(true);
			SoundPtr berserkerInGhostStateSound = this->core->createSound("beserk_close.ogg", "AmbientMusic");
			berserkerInGhostStateSound->setRepeat(true);

			PostProcessingEffectPtr deathEffect = postProcessingSystem->addPostProcessingEffect("DeathEffect");		
			PostProcessingEffectPtr seeingStatueEffect = postProcessingSystem->addPostProcessingEffect("SeeingStatueEffect");

			this->insanitySystem->setLocalHuman(human);

			LocalPlayerHumanEffectsPtr localPlayerHumanEffects(
				new LocalPlayerHumanEffects(this->statueClonesSystem, core->getPhysicsSystem()->getCollisionTester(), this->monsterController->getMonster(), 
				monsterAroundSound, monsterCloseSound, monsterCloserSound, blindCloseSound, ghostThroughSounds, berserkerInBerserkerStateSound, 
				berserkerInGhostStateSound, deathEffect, seeingStatueEffect));

			StatueTimeToUpdateHandlerPtr statueTimeToUpdateHandler(new StatueTimeToUpdateHandler(this->characterHandler, human));

			LocalPlayerHumanControllerPtr controller(
				new LocalPlayerHumanController(human, this->itemSystem, this->manipulationSystem, this->spectatorSystem, camera, humanView, localPlayerHumanEffects, 
				statueTimeToUpdateHandler, this->terrorSave->controls));
			this->core->addInputHandler(controller, "Ingame");
			this->core->addEventHandler(controller, "Ingame");

			return controller;
		}

		RemotePlayerHumanControllerPtr loadRemotePlayerHuman(CharacterType character, bool forest) {
			ActorPtr actor = this->loadActor(character);
			actor->setMaterialId(30);

			FlashlightPtr flashlight = this->loadFlashlight(actor, false);
			InventoryPtr inventory(new Inventory(false));

			CharacterDataPtr characterData(new CharacterData(actor));
			HumanPtr human = this->loadHuman(characterData, character, flashlight, inventory, forest);

			RemotePlayerHumanControllerPtr controller(new RemotePlayerHumanController(human));
			this->core->addEventHandler(controller, "Ingame");

			this->characterHandler->addHumanAlive(human);

			return controller;
		}

		RemotePlayerHumanControllerPtr loadRemotePlayerDeadHuman(DeadHumanDataPtr deadHumanData, bool forest) {
			ActorPtr actor = this->loadActor(deadHumanData->getCharacterType());

			FlashlightPtr flashlight = this->loadFlashlight(actor, false);
			InventoryPtr inventory(new Inventory(false));

			CharacterDataPtr characterData(new CharacterData(actor));
			HumanPtr human = this->loadHuman(characterData, deadHumanData->getCharacterType(), flashlight, inventory, forest);

			RemotePlayerHumanControllerPtr controller(new RemotePlayerHumanController(human));
			this->core->addEventHandler(controller);

			actor->setPosition(deadHumanData->getPosition(), 0.1f);
			actor->setOrientation(deadHumanData->getOrientation(), 0.1f);	

			human->die(true);
			this->characterHandler->addDeadHuman(human, deadHumanData->getId());

			return controller;
		}

		RemotePlayerHumanControllerPtr transformLocalPlayerHumanInRemote(LocalPlayerHumanControllerPtr oldController) {			
			this->core->removeInputHandler(oldController);
			this->core->removeEventHandler(oldController);
			this->particleFxHandler->resetLocalActor();

			HumanPtr human = oldController->getHuman();

			RemotePlayerHumanControllerPtr controller(new RemotePlayerHumanController(human));
			this->core->addEventHandler(controller, "Ingame");
			return controller;
		}

		void destroyHumanController(HumanControllerPtr controller) {
			this->characterHandler->removeHumanAlive(controller->getHuman());

			if (controller->getType() == LOCAL_HUMAN_CONTROLLER) {
				this->core->removeEventHandler(static_ptr_cast<LocalPlayerHumanController>(controller));
				this->core->removeInputHandler(static_ptr_cast<LocalPlayerHumanController>(controller));
				this->particleFxHandler->resetLocalActor();
				this->insanitySystem->reset();
			}
			if (controller->getType() == REMOTE_HUMAN_CONTROLLER) {
				this->core->removeEventHandler(static_ptr_cast<RemotePlayerHumanController>(controller));
			}
			controller->destroy();
		}

		MonsterControllerPtr loadLocalPlayerMonster(CharacterType monsterType, SpawnPointPtr spawnPoint, bool forest, bool factory, const string& stageName) {			
			ActorPtr actor = this->loadActor(monsterType, spawnPoint);
			CharacterDataPtr characterData(new CharacterData(actor));
			MonsterPtr monster;
			if (monsterType == CHAR_MONSTER_0) {			
				SpectrumPtr spectrum = this->loadSpectrum(characterData, forest);
				monster = spectrum;
				this->monsterController = this->loadLocalPlayerSpectrumController(spectrum);
			}else if (monsterType == CHAR_MONSTER_1) {
				BlindPtr blind = this->loadBlind(characterData, forest, factory);
				monster = blind;
				this->monsterController = this->loadLocalPlayerBlindController(blind, forest, factory);
			}else if (monsterType == CHAR_MONSTER_2) {
				BerserkerPtr berserker = this->loadBerserker(characterData, forest);
				monster = berserker;
				this->monsterController = this->loadLocalPlayerBerserkerController(berserker);
			}else if (monsterType == CHAR_MONSTER_3) {
				StatuePtr statue = this->loadStatue(characterData, stageName);
				monster = statue;
				this->monsterController = this->loadLocalPlayerStatueController(statue);
			}
			this->localPlayerMonsterLoadedSignal(monster);
			return this->monsterController;
		}

		MonsterControllerPtr loadLocalAiMonster(CharacterType monsterType, SpawnPointPtr spawnPoint, bool forest, bool factory, const string& stageName) {
			ActorPtr actor = this->loadActor(monsterType, spawnPoint);
			CharacterDataPtr characterData(new CharacterData(actor));
			if (monsterType == CHAR_MONSTER_0) {			
				SpectrumPtr spectrum = this->loadSpectrum(characterData, forest);
				LocalAiSpectrumControllerPtr controller(new LocalAiSpectrumController(spectrum));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monsterType == CHAR_MONSTER_1) {
				BlindPtr blind = this->loadBlind(characterData, forest, factory);
				LocalAiBlindControllerPtr controller(new LocalAiBlindController(blind));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monsterType == CHAR_MONSTER_2) {
				BerserkerPtr berserker = this->loadBerserker(characterData, forest);
				LocalAiBerserkerControllerPtr controller(new LocalAiBerserkerController(berserker));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monsterType == CHAR_MONSTER_3) {
				StatuePtr statue = this->loadStatue(characterData, stageName);
				LocalAiStatueControllerPtr controller(new LocalAiStatueController(statue));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}
			return this->monsterController;
		}

		MonsterControllerPtr loadRemotePlayerMonster(CharacterType monsterType, SpawnPointPtr spawnPoint, bool forest, bool factory, const string& stageName) {
			ActorPtr actor = this->loadActor(monsterType, spawnPoint);
			CharacterDataPtr characterData(new CharacterData(actor));
			if (monsterType == CHAR_MONSTER_0) {			
				SpectrumPtr spectrum = this->loadSpectrum(characterData, forest);
				this->monsterController = MonsterControllerPtr(new RemotePlayerSpectrumController(spectrum));
			}else if (monsterType == CHAR_MONSTER_1) {
				BlindPtr blind = this->loadBlind(characterData, forest, factory);
				this->monsterController = MonsterControllerPtr(new RemotePlayerBlindController(blind));
			}else if (monsterType == CHAR_MONSTER_2) {
				BerserkerPtr berserker = this->loadBerserker(characterData, forest);
				this->monsterController = MonsterControllerPtr(new RemotePlayerBerserkerController(berserker));
			}else if (monsterType == CHAR_MONSTER_3) {
				StatuePtr statue = this->loadStatue(characterData, stageName);
				this->monsterController = MonsterControllerPtr(new RemotePlayerStatueController(statue));
			}
			return this->monsterController;
		}

		MonsterControllerPtr transformMonsterControllerInRemote() {
			if (this->monsterController->getType() == AI_MONSTER_CONTROLLER) {
				this->removeAiMonsterControllerFromEngine();
			}else if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
				this->removeLocalMonsterControllerFromEngine();
			}

			MonsterPtr monster = this->monsterController->getMonster();
			if (monster->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
				this->monsterController = MonsterControllerPtr(new RemotePlayerSpectrumController(spectrum));
			}else if (monster->getType() == BLIND) {
				BlindPtr blind = static_ptr_cast<Blind>(monster);
				this->monsterController = MonsterControllerPtr(new RemotePlayerBlindController(blind));
			}else if (monster->getType() == BERSERKER) {
				BerserkerPtr berserker = static_ptr_cast<Berserker>(monster);
				this->monsterController = MonsterControllerPtr(new RemotePlayerBerserkerController(berserker));				
			}else if (monster->getType() == STATUE) {
				StatuePtr statue = static_ptr_cast<Statue>(monster);
				this->monsterController = MonsterControllerPtr(new RemotePlayerStatueController(statue));
			}
			return this->monsterController;
		}

		MonsterControllerPtr transformMonsterControllerInAi() {
			if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
				this->removeLocalMonsterControllerFromEngine();
			}

			MonsterPtr monster = this->monsterController->getMonster();
			if (monster->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
				LocalAiSpectrumControllerPtr controller(new LocalAiSpectrumController(spectrum));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monster->getType() == BLIND) {
				BlindPtr blind = static_ptr_cast<Blind>(monster);
				LocalAiBlindControllerPtr controller(new LocalAiBlindController(blind));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monster->getType() == BERSERKER) {
				BerserkerPtr berserker = static_ptr_cast<Berserker>(monster);
				LocalAiBerserkerControllerPtr controller(new LocalAiBerserkerController(berserker));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}else if (monster->getType() == STATUE) {
				StatuePtr statue = static_ptr_cast<Statue>(monster);
				LocalAiStatueControllerPtr controller(new LocalAiStatueController(statue));				
				this->core->addEventHandler(controller, "Ingame");
				this->monsterController = controller;
			}

			this->scareTrapSystem->reset(0.0f);

			return this->monsterController;
		}

		MonsterControllerPtr transformMonsterControllerInLocal(bool forest, bool factory) {
			if (this->monsterController->getType() == AI_MONSTER_CONTROLLER) {
				this->removeAiMonsterControllerFromEngine();
			}

			ActorPtr actor = this->monsterController->getMonster()->getActor();
			MonsterPtr monster = this->monsterController->getMonster();
			if (monster->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
				this->monsterController = this->loadLocalPlayerSpectrumController(spectrum);
			}else if (monster->getType() == BLIND) {
				BlindPtr blind = static_ptr_cast<Blind>(monster);
				this->monsterController = this->loadLocalPlayerBlindController(blind, forest, factory);
			}else if (monster->getType() == BERSERKER) {
				BerserkerPtr berserker = static_ptr_cast<Berserker>(monster);
				this->monsterController = this->loadLocalPlayerBerserkerController(berserker);
			}else if (monster->getType() == STATUE) {
				StatuePtr statue = static_ptr_cast<Statue>(monster);
				this->monsterController = this->loadLocalPlayerStatueController(statue);
			}
			this->localPlayerMonsterLoadedSignal(monster);
			return this->monsterController;
		}
		
		void destroyMonsterController() {
			if (!this->monsterController) {
				return;
			}

			if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
				this->removeLocalMonsterControllerFromEngine();
			}
			if (this->monsterController->getType() == AI_MONSTER_CONTROLLER) {
				this->removeAiMonsterControllerFromEngine();
			}
			this->monsterController->destroy();
			this->monsterController.reset();
			this->characterHandler->resetMonster();
		}

		Connection connectLocalPlayerMonsterLoadedEvent(const MonsterSignal::slot_type& localPlayerMonsterLoadedEvent) {
			return this->localPlayerMonsterLoadedSignal.connect(localPlayerMonsterLoadedEvent);
		}

		virtual ~CharacterLoader() {}

	protected:
		CharacterHandlerPtr characterHandler;
		TerrorSavePtr terrorSave;
		CorePtr core;
		SprintSystemPtr sprintSystem;
		MonsterControllerPtr monsterController;
		StatueClonesSystemPtr statueClonesSystem;
		ScareTrapSystemPtr scareTrapSystem;
		DeathSystemPtr deathSystem;
		ItemSystemPtr itemSystem;
		ManipulationSystemPtr manipulationSystem;
		SpectatorSystemPtr spectatorSystem;
		ParticleFxHandlerPtr particleFxHandler;
		InsanitySystemPtr insanitySystem;
		BerserkerEnergyRegenerationSystemPtr berserkerEnergyRegenerationSystem;
		BerserkerTeleportSystemPtr berserkerTeleportSystem;
		BerserkerGhostViewLogicPtr berserkerGhostViewLogic; 
		MonsterSignal localPlayerMonsterLoadedSignal;

	private:
		FlashlightPtr loadFlashlight(ActorPtr actor, bool local) {
			MeshInstancePtr mesh = this->core->getSceneManager()->createEntity("flashlight.mesh");
			LightPtr light = this->core->getSceneManager()->createLight();
			SoundPtr switchSound = actor->getSoundSource()->createSound("flashlight_switch.ogg");
			switchSound->setRepeat(false);

			if (local) {
				FlashlightPtr flashlight(new Flashlight(mesh, light, switchSound, this->core->getGraphicsSystem()->getCamera()));
				return flashlight;
			}

			Ogre::BillboardSet* flare = this->core->getSceneManager()->createBillboardSet();			
			flare->setMaterialName("FlareAlpha");
			flare->setBillboardType(Ogre::BBT_POINT);
			flare->setDefaultDimensions(0.75f, 0.75f);
			flare->setCommonDirection(Ogre::Vector3(-1.0f, 0.0f, 0.0f));
			flare->setCastShadows(false);
			flare->createBillboard(0.0f, 0.0f, 0.0f);			

			FlashlightPtr flashlight(new Flashlight(mesh, light, flare, switchSound, this->core->getGraphicsSystem()->getCamera()));
			return flashlight;
		}

		ActorPtr loadActor(CharacterType character) {
			string characterScript = this->getCharacterScript(character);
			NodePtr characterNode = Core::getInstance()->getParseTree(characterScript + ".character")->getChildByType("character", true);
			
			ActorPtr actor = EntitySystem::getInstance()->createActor(characterNode->getChildByType("actor", true)->getChildValue("entry", string("")));
			actor->setId(character);

			//prevent Ogre culling;
			Ogre::AxisAlignedBox aab(-100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f);
			actor->getMesh()->getMesh()->_setBounds(aab);

			return actor;
		}

		ActorPtr loadActor(CharacterType character, SpawnPointPtr spawnPoint) {
			ActorPtr actor = this->loadActor(character);			

			actor->setPosition(spawnPoint->getPosition());
			actor->setOrientation(spawnPoint->getOrientation());

			return actor;
		}

		void setAnimationSoundEffect(ActorPtr actor, const string& animationName, const string& soundEffectName) {
			AnimationPtr animation = actor->getAnimation(animationName);
			if (animation) {
				SoundPtr soundEffect = actor->getSoundSource()->createSound(soundEffectName);
				soundEffect->setRepeat(true);
				animation->setSoundEffect(soundEffect);
			}
		}

		HumanPtr loadHuman(CharacterDataPtr characterData, CharacterType character, FlashlightPtr flashlight, InventoryPtr inventory, bool forest) {
			FallSystemPtr fallSystem(new FallSystem);
			this->core->addEventHandler(fallSystem);

			CollisionTesterPtr collisionTester = this->core->getPhysicsSystem()->getCollisionTester();

			CharacterActionsFactoryPtr characterActionsFactory(new CharacterActionsFactory(fallSystem, collisionTester));
			characterData->walkSpeed = 1.0f;
			characterData->crouchedWalkSpeed = 1.0f;
			characterData->runSpeed = 2.5f;
			characterData->crouchedRunSpeed = 1.0f;
			float sprintSpeed = 5.0f;

			SoundSourcePtr soundSource = characterData->actor->getSoundSource();
			SoundPtr gasping;
			SoundPtr screaming;
			if (character == CHAR_0) {
				gasping = soundSource->createSound("gasping_girl.ogg");				
				screaming = soundSource->createSound("screaming_girl.ogg");
			}else {
				gasping = soundSource->createSound("gasping.ogg");
				screaming = soundSource->createSound("screaming_boy.ogg");
			}
			gasping->setRepeat(false);
			screaming->setRepeat(false);

			CharacterActionsPtr characterActions;
			HumanAnimationNamesPtr animationNames = this->createHumanAnimationNames();

			if (forest) {
				this->setAnimationSoundEffect(characterData->actor, animationNames->walk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftWalk, "slow_walking_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedDiagonalRightWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedDiagonalLeftWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedStrafingRightWalk, "slow_walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedStrafingLeftWalk, "slow_walking_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->run, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftRun, "walking_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedDiagonalRightRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedDiagonalLeftRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedStrafingRightRun, "walking_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->crouchedStrafingLeftRun, "walking_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->sprint, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightSprint, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftSprint, "running_grass.ogg");
			}

			characterActions = characterActionsFactory->create(characterData, animationNames);

			HumanActionsPtr humanActions(new HumanActions);
			humanActions->pickItem = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 1, 499, 10, 11));
			humanActions->pickBattery = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 502, 502, 10, 11));
			humanActions->pickDead = PickDeadPtr(new PickDead(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera()));
			humanActions->manipulate = ManipulatePtr(new Manipulate(characterData, character, this->core->getPhysicsSystem(), 
				this->core->getGraphicsSystem()->getCamera()));
			humanActions->manipulate->connectGrabEvent(Bind(&ManipulationSystem::requestGrabObject, this->manipulationSystem, _1, _2, _3));
			humanActions->manipulate->connectManipulateEvent(Bind(&ManipulationSystem::requestManipulateObject, this->manipulationSystem, _1, _2));
			humanActions->manipulate->connectReleaseEvent(Bind(&ManipulationSystem::requestReleaseObject, this->manipulationSystem, _1));
			humanActions->sprint = SprintPtr(
				new Sprint(characterData, animationNames->sprint, animationNames->diagonalRightSprint, animationNames->diagonalLeftSprint, sprintSpeed, gasping));
			this->sprintSystem->add(humanActions->sprint);
			humanActions->die = DiePtr(new Die(characterData, animationNames->die, screaming));			

			return HumanPtr(new Human(characterData, character, characterActions, humanActions, flashlight, inventory));
		}

		SpectrumPtr loadSpectrum(CharacterDataPtr characterData, bool forest) {
			FallSystemPtr fallSystem(new FallSystem);
			this->core->addEventHandler(fallSystem);

			CollisionTesterPtr collisionTester = this->core->getPhysicsSystem()->getCollisionTester();

			CharacterActionsFactoryPtr characterActionsFactory(new CharacterActionsFactory(fallSystem, collisionTester));
			characterData->walkSpeed = 1.0f;
			characterData->crouchedWalkSpeed = 1.0f;
			characterData->runSpeed = 3.0f;
			characterData->crouchedRunSpeed = 1.0f;

			CharacterActionsPtr characterActions;
			MonsterAnimationNamesPtr animationNames = this->createSpectrumAnimationNames();

			if (forest) {
				this->setAnimationSoundEffect(characterData->actor, animationNames->walk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftWalk, "running_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->run, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftRun, "running_grass.ogg");
			}

			characterActions = characterActionsFactory->create(characterData, animationNames);

			MonsterActionsPtr monsterActions(new MonsterActions);
			monsterActions->pickItem = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 500, 500, 20, 21));
			monsterActions->manipulate = ManipulatePtr(
				new Manipulate(characterData, CHAR_MONSTER_0, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->manipulate->connectGrabEvent(Bind(&ManipulationSystem::requestGrabObject, this->manipulationSystem, _1, _2, _3));
			monsterActions->manipulate->connectManipulateEvent(Bind(&ManipulationSystem::requestManipulateObject, this->manipulationSystem, _1, _2));
			monsterActions->manipulate->connectReleaseEvent(Bind(&ManipulationSystem::requestReleaseObject, this->manipulationSystem, _1));
			monsterActions->attack = AttackPtr(
				new Attack(characterData, animationNames->attack, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));

			SoundSourcePtr soundSource = characterData->actor->getSoundSource();
			SoundPtr changeState = soundSource->createSound("monster_out.ogg");
			changeState->setRepeat(false);

			SpectrumPtr spectrum(new Spectrum(characterData, characterActions, monsterActions, changeState, forest));
			this->characterHandler->setMonster(spectrum);
			return spectrum;
		}

		MonsterControllerPtr loadLocalPlayerSpectrumController(SpectrumPtr monster) {
			PostProcessingSystemPtr postProcessingSystem = this->core->getGraphicsSystem()->getPostProcessingSystem();
			PostProcessingEffectPtr monsterView = postProcessingSystem->addPostProcessingEffect("MonsterView");
			PostProcessingEffectPtr ghostView = postProcessingSystem->addPostProcessingEffect("GhostView");			
			PostProcessingEffectPtr energyMaxView = postProcessingSystem->addPostProcessingEffect("EnergyMaxView");			

			LightPtr monsterLight = this->core->getSceneManager()->createLight();
			monsterLight->setType(Light::LT_POINT);
			monsterLight->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setSpecularColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setAttenuation(3.0f, 10.715322f, 0.0f, 0.0f);
			monster->getActor()->getNode()->attachObject(monsterLight);

			SoundStreamPtrVector becomeVisibleSounds;
			becomeVisibleSounds.push_back(this->core->createSound("monster_spawn.ogg"));
			becomeVisibleSounds.push_back(this->core->createSound("monster_spawn2.ogg"));
			becomeVisibleSounds.push_back(this->core->createSound("monster_spawn3.ogg"));
			foreach (SoundPtr sound, becomeVisibleSounds) {
				sound->setRepeat(false);
			}
			SoundPtr monsterMusic = this->core->createSound("monster_music.ogg", "AmbientMusic");
			monsterMusic->setRepeat(true);
			SoundPtr ghostMusic = this->core->createSound("ghost_music.ogg", "AmbientMusic");
			ghostMusic->setRepeat(true);

			LocalPlayerSpectrumEffectsPtr localPlayerSpectrumEffects(
				new LocalPlayerSpectrumEffects(monsterLight, monsterView, ghostView, energyMaxView, monsterMusic, ghostMusic, becomeVisibleSounds));			

			RenderCameraPtr camera = this->core->getGraphicsSystem()->getCamera();
			LocalPlayerSpectrumControllerPtr controller(new LocalPlayerSpectrumController(monster, this->scareTrapSystem, this->deathSystem, 
				this->manipulationSystem, this->spectatorSystem, camera, localPlayerSpectrumEffects, this->terrorSave->controls));
			this->core->addInputHandler(controller, "Ingame");
			this->core->addEventHandler(controller, "Ingame");
			return controller;
		}

		BlindPtr loadBlind(CharacterDataPtr characterData, bool forest, bool factory) {
			FallSystemPtr fallSystem(new FallSystem);
			this->core->addEventHandler(fallSystem);

			CollisionTesterPtr collisionTester = this->core->getPhysicsSystem()->getCollisionTester();

			CharacterActionsFactoryPtr characterActionsFactory(new CharacterActionsFactory(fallSystem, collisionTester));
			characterData->walkSpeed = 1.0f;
			characterData->crouchedWalkSpeed = 1.0f;
			if (forest) {
				characterData->runSpeed = 3.25f;
			}else if (factory) {
				characterData->runSpeed = 3.0f;
			}else {
				characterData->runSpeed = 2.55f;
			}
			characterData->crouchedRunSpeed = 1.0f;

			CharacterActionsPtr characterActions;
			MonsterAnimationNamesPtr animationNames = this->createBlindAnimationNames();
			characterActions = characterActionsFactory->create(characterData, animationNames);

			MonsterActionsPtr monsterActions(new MonsterActions);
			monsterActions->pickItem = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 500, 500, 20, 21));
			monsterActions->manipulate = ManipulatePtr(
				new Manipulate(characterData, CHAR_MONSTER_1, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->manipulate->connectGrabEvent(Bind(&ManipulationSystem::requestGrabObject, this->manipulationSystem, _1, _2, _3));
			monsterActions->manipulate->connectManipulateEvent(Bind(&ManipulationSystem::requestManipulateObject, this->manipulationSystem, _1, _2));
			monsterActions->manipulate->connectReleaseEvent(Bind(&ManipulationSystem::requestReleaseObject, this->manipulationSystem, _1));
			monsterActions->attack = AttackPtr(
				new Attack(characterData, animationNames->attack, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));

			BlindPtr blind(new Blind(characterData, characterActions, monsterActions));
			this->characterHandler->setMonster(blind);
			return blind;
		}

		MonsterControllerPtr loadLocalPlayerBlindController(BlindPtr monster, bool forest, bool factory) {
			PostProcessingSystemPtr postProcessingSystem = this->core->getGraphicsSystem()->getPostProcessingSystem();
			PostProcessingEffectPtr blindView = postProcessingSystem->addPostProcessingEffect("BlindView");

			LightPtr monsterLight = this->core->getSceneManager()->createLight();
			monsterLight->setType(Light::LT_POINT);
			monsterLight->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setSpecularColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setAttenuation(3.0f, 10.715322f, 0.0f, 0.0f);
			monster->getActor()->getNode()->attachObject(monsterLight);			

			RenderCameraPtr camera = this->core->getGraphicsSystem()->getCamera();
			LocalPlayerBlindControllerPtr controller(new LocalPlayerBlindController(monster, this->scareTrapSystem, this->deathSystem, 
				this->manipulationSystem, this->spectatorSystem, camera, blindView, this->terrorSave->controls, forest, factory));
			this->core->addInputHandler(controller, "Ingame");
			this->core->addEventHandler(controller, "Ingame");
			return controller;
		}

		BerserkerPtr loadBerserker(CharacterDataPtr characterData, bool forest) {
			FallSystemPtr fallSystem(new FallSystem);
			this->core->addEventHandler(fallSystem);

			CollisionTesterPtr collisionTester = this->core->getPhysicsSystem()->getCollisionTester();

			CharacterActionsFactoryPtr characterActionsFactory(new CharacterActionsFactory(fallSystem, collisionTester));
			if (forest) {
				characterData->walkSpeed = 1.2f;
				characterData->crouchedWalkSpeed = 1.2f;
			}else {
				characterData->walkSpeed = 0.64f;
				characterData->crouchedWalkSpeed = 0.64f;
			}
			characterData->runSpeed = 10.0f;
			characterData->crouchedRunSpeed = 10.0f;

			CharacterActionsPtr characterActions;
			MonsterAnimationNamesPtr animationNames = this->createBerserkerAnimationNames();

			if (forest) {
				this->setAnimationSoundEffect(characterData->actor, animationNames->walk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftWalk, "running_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->run, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftRun, "running_grass.ogg");
			}

			characterActions = characterActionsFactory->create(characterData, animationNames);

			MonsterActionsPtr monsterActions(new MonsterActions);
			monsterActions->pickItem = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 500, 500, 20, 21));
			monsterActions->manipulate = ManipulatePtr(
				new Manipulate(characterData, CHAR_MONSTER_2, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->manipulate->connectGrabEvent(Bind(&ManipulationSystem::requestGrabObject, this->manipulationSystem, _1, _2, _3));
			monsterActions->manipulate->connectManipulateEvent(Bind(&ManipulationSystem::requestManipulateObject, this->manipulationSystem, _1, _2));
			monsterActions->manipulate->connectReleaseEvent(Bind(&ManipulationSystem::requestReleaseObject, this->manipulationSystem, _1));
			monsterActions->attack = AttackPtr(
				new Attack(characterData, animationNames->attack, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->idle = IdlePtr(new Idle(characterData, animationNames->monsterIdle));

			BerserkerPtr berserker(new Berserker(characterData, characterActions, monsterActions));
			this->characterHandler->setMonster(berserker);
			return berserker;
		}

		MonsterControllerPtr loadLocalPlayerBerserkerController(BerserkerPtr monster) {
			LightPtr monsterLight = this->core->getSceneManager()->createLight();
			monsterLight->setType(Light::LT_POINT);
			monsterLight->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setSpecularColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setAttenuation(3.0f, 10.715322f, 0.0f, 0.0f);
			monster->getActor()->getNode()->attachObject(monsterLight);			

			RenderCameraPtr camera = this->core->getGraphicsSystem()->getCamera();

			SoundPtr berserkerMusic = this->core->createSound("beserk_beserking.ogg", "AmbientMusic");
			berserkerMusic->setRepeat(true);
			SoundPtr ghostMusic = this->core->createSound("beserk_music.ogg", "AmbientMusic");
			ghostMusic->setRepeat(true);

			PostProcessingSystemPtr postProcessingSystem = this->core->getGraphicsSystem()->getPostProcessingSystem();
			if (!this->berserkerGhostViewLogic) {
				this->berserkerGhostViewLogic = BerserkerGhostViewLogicPtr(new BerserkerGhostViewLogic);
				postProcessingSystem->registerPostProcessingLogic("BerserkerGhostViewLogic", this->berserkerGhostViewLogic.get());
			}
			PostProcessingEffectPtr berserkerView = postProcessingSystem->addPostProcessingEffect("BerserkerView");
			PostProcessingEffectPtr ghostView = postProcessingSystem->addPostProcessingEffect("BerserkerGhostView");

			LocalPlayerBerserkerEffectsPtr localPlayerBerserkerEffects(new LocalPlayerBerserkerEffects(monsterLight, berserkerView, ghostView, this->berserkerGhostViewLogic, berserkerMusic, ghostMusic,
				monster->getEnergy(), this->terrorSave, this->core->getButtonTranslator()));

			LocalPlayerBerserkerControllerPtr controller(new LocalPlayerBerserkerController(monster, this->scareTrapSystem, this->deathSystem, 
				this->manipulationSystem, this->spectatorSystem, camera, localPlayerBerserkerEffects, this->berserkerEnergyRegenerationSystem, this->berserkerTeleportSystem, this->terrorSave->controls));
			this->core->addInputHandler(controller, "Ingame");
			this->core->addEventHandler(controller, "Ingame");
			return controller;
		}

		StatuePtr loadStatue(CharacterDataPtr characterData, const string& stageName) {
			FallSystemPtr fallSystem(new FallSystem);
			this->core->addEventHandler(fallSystem);

			CollisionTesterPtr collisionTester = this->core->getPhysicsSystem()->getCollisionTester();

			CharacterActionsFactoryPtr characterActionsFactory(new CharacterActionsFactory(fallSystem, collisionTester));
			if (stageName == "Black_Lake_Woods") {
				characterData->walkSpeed = 3.25f;
				characterData->crouchedWalkSpeed = 3.25f;
			}else if (stageName == "Factory") {
				characterData->walkSpeed = 2.75f;
				characterData->crouchedWalkSpeed = 2.75f;
			}else {
				characterData->walkSpeed = 2.55f;
				characterData->crouchedWalkSpeed = 2.55f;
			}

			CharacterActionsPtr characterActions;
			MonsterAnimationNamesPtr animationNames = this->createStatueAnimationNames();

			if (stageName == "Black_Lake_Woods") {
				this->setAnimationSoundEffect(characterData->actor, animationNames->walk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightWalk, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftWalk, "running_grass.ogg");

				this->setAnimationSoundEffect(characterData->actor, animationNames->run, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->diagonalLeftRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingRightRun, "running_grass.ogg");
				this->setAnimationSoundEffect(characterData->actor, animationNames->strafingLeftRun, "running_grass.ogg");
			}

			characterActions = characterActionsFactory->create(characterData, animationNames);

			MonsterActionsPtr monsterActions(new MonsterActions);
			monsterActions->pickItem = PickItemPtr(new PickItem(characterData, collisionTester, this->core->getGraphicsSystem()->getCamera(), 500, 500, 20, 21));
			monsterActions->manipulate = ManipulatePtr(
				new Manipulate(characterData, CHAR_MONSTER_3, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->manipulate->connectGrabEvent(Bind(&ManipulationSystem::requestGrabObject, this->manipulationSystem, _1, _2, _3));
			monsterActions->manipulate->connectManipulateEvent(Bind(&ManipulationSystem::requestManipulateObject, this->manipulationSystem, _1, _2));
			monsterActions->manipulate->connectReleaseEvent(Bind(&ManipulationSystem::requestReleaseObject, this->manipulationSystem, _1));
			monsterActions->attack = AttackPtr(
				new Attack(characterData, animationNames->attack, this->core->getPhysicsSystem(), this->core->getGraphicsSystem()->getCamera()));
			monsterActions->idle = IdlePtr(new Idle(characterData, animationNames->monsterIdle));

			StatuePtr statue(new Statue(characterData, characterActions, monsterActions, this->statueClonesSystem, stageName));
			this->characterHandler->setMonster(statue);
			return statue;
		}

		MonsterControllerPtr loadLocalPlayerStatueController(StatuePtr monster) {
			LightPtr monsterLight = this->core->getSceneManager()->createLight();
			monsterLight->setType(Light::LT_POINT);
			monsterLight->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setSpecularColour(ColourValue(1.0f, 1.0f, 1.0f));
			monsterLight->setAttenuation(3.0f, 10.715322f, 0.0f, 0.0f);
			monster->getActor()->getNode()->attachObject(monsterLight);			

			RenderCameraPtr camera = this->core->getGraphicsSystem()->getCamera();

			/*SoundPtr berserkerMusic = this->core->createSound("beserk_beserking.ogg", "AmbientMusic");
			berserkerMusic->setRepeat(true);
			SoundPtr ghostMusic = this->core->createSound("beserk_music.ogg", "AmbientMusic");
			ghostMusic->setRepeat(true);

			PostProcessingSystemPtr postProcessingSystem = this->core->getGraphicsSystem()->getPostProcessingSystem();
			if (!this->statueGhostViewLogic) {
			this->statueGhostViewLogic = StatueGhostViewLogicPtr(new StatueGhostViewLogic);
			postProcessingSystem->registerPostProcessingLogic("StatueGhostViewLogic", this->statueGhostViewLogic.get());
			}
			PostProcessingEffectPtr statueView = postProcessingSystem->addPostProcessingEffect("StatueView");
			PostProcessingEffectPtr ghostView = postProcessingSystem->addPostProcessingEffect("StatueGhostView");

			LocalPlayerStatueEffectsPtr localPlayerStatueEffects(new LocalPlayerStatueEffects(monsterLight, statueView, ghostView, this->statueGhostViewLogic, berserkerMusic, ghostMusic,
			this->terrorSave, this->core->getButtonTranslator()));*/

			LocalPlayerStatueControllerPtr controller(new LocalPlayerStatueController(monster, this->statueClonesSystem, this->scareTrapSystem, this->deathSystem, 
				this->manipulationSystem, this->spectatorSystem, camera, this->terrorSave->controls));
			this->core->addInputHandler(controller, "Ingame");
			this->core->addEventHandler(controller, "Ingame");
			return controller;
		}

		HumanAnimationNamesPtr createHumanAnimationNames() {
			HumanAnimationNamesPtr animationNames(new HumanAnimationNames);
			animationNames->idle = "idle";
			animationNames->crouchedIdle = "crouch";

			animationNames->walk = "slowWalk";
			animationNames->diagonalRightWalk = "slowWalk45Right";
			animationNames->diagonalLeftWalk = "slowWalk45Left";
			animationNames->strafingRightWalk = "slowStrafeRight";
			animationNames->strafingLeftWalk = "slowStrafeLeft";
			animationNames->crouchedWalk = "crouchWalk";
			animationNames->crouchedDiagonalRightWalk = "crouch45Right";
			animationNames->crouchedDiagonalLeftWalk = "crouch45Left";
			animationNames->crouchedStrafingRightWalk = "crouchStrafeRight";
			animationNames->crouchedStrafingLeftWalk = "crouchStrafeLeft";

			animationNames->run = "walking";		
			animationNames->diagonalRightRun = "walking45Right";
			animationNames->diagonalLeftRun = "walking45Left";
			animationNames->strafingRightRun = "strafeRight";
			animationNames->strafingLeftRun = "strafeLeft";
			animationNames->crouchedRun = "crouchWalk";
			animationNames->crouchedDiagonalRightRun = "crouch45Right";
			animationNames->crouchedDiagonalLeftRun = "crouch45Left";
			animationNames->crouchedStrafingRightRun = "crouchStrafeRight";
			animationNames->crouchedStrafingLeftRun = "crouchStrafeLeft";

			animationNames->crouch = "";
			animationNames->stand = "";

			animationNames->jump = "";
			animationNames->fall = "";
			animationNames->land = "";

			animationNames->sprint = "running";
			animationNames->diagonalRightSprint = "running45Right";
			animationNames->diagonalLeftSprint = "running45Left";
			animationNames->die = "death";

			return animationNames;
		}

		MonsterAnimationNamesPtr createSpectrumAnimationNames() {
			MonsterAnimationNamesPtr animationNames(new MonsterAnimationNames);
			animationNames->idle = "idle";
			animationNames->crouchedIdle = "";
			animationNames->monsterIdle = "";

			animationNames->walk = "walking";
			animationNames->diagonalRightWalk = "walk45Right";
			animationNames->diagonalLeftWalk = "walk45Left";
			animationNames->strafingRightWalk = "walkStrafeRight";
			animationNames->strafingLeftWalk = "walkStrafeLeft";
			animationNames->crouchedWalk = "";
			animationNames->crouchedDiagonalRightWalk = "";
			animationNames->crouchedDiagonalLeftWalk = "";
			animationNames->crouchedStrafingRightWalk = "";
			animationNames->crouchedStrafingLeftWalk = "";

			animationNames->run = "walking";		
			animationNames->diagonalRightRun = "walk45Right";
			animationNames->diagonalLeftRun = "walk45Left";
			animationNames->strafingRightRun = "walkStrafeRight";
			animationNames->strafingLeftRun = "walkStrafeLeft";
			animationNames->crouchedRun = "";
			animationNames->crouchedDiagonalRightRun = "";
			animationNames->crouchedDiagonalLeftRun = "";
			animationNames->crouchedStrafingRightRun = "";
			animationNames->crouchedStrafingLeftRun = "";

			animationNames->crouch = "";
			animationNames->stand = "";

			animationNames->jump = "";
			animationNames->fall = "";
			animationNames->land = "";

			animationNames->attack = "attack";

			return animationNames;
		}

		MonsterAnimationNamesPtr createBlindAnimationNames() {
			MonsterAnimationNamesPtr animationNames(new MonsterAnimationNames);
			animationNames->idle = "idle";
			animationNames->crouchedIdle = "";
			animationNames->monsterIdle = "";

			animationNames->walk = "walking";
			animationNames->diagonalRightWalk = "walk45Right";
			animationNames->diagonalLeftWalk = "walk45Left";
			animationNames->strafingRightWalk = "walkStrafeRight";
			animationNames->strafingLeftWalk = "walkStrafeLeft";
			animationNames->crouchedWalk = "";
			animationNames->crouchedDiagonalRightWalk = "";
			animationNames->crouchedDiagonalLeftWalk = "";
			animationNames->crouchedStrafingRightWalk = "";
			animationNames->crouchedStrafingLeftWalk = "";

			animationNames->run = "walking";		
			animationNames->diagonalRightRun = "walk45Right";
			animationNames->diagonalLeftRun = "walk45Left";
			animationNames->strafingRightRun = "walkStrafeRight";
			animationNames->strafingLeftRun = "walkStrafeLeft";
			animationNames->crouchedRun = "";
			animationNames->crouchedDiagonalRightRun = "";
			animationNames->crouchedDiagonalLeftRun = "";
			animationNames->crouchedStrafingRightRun = "";
			animationNames->crouchedStrafingLeftRun = "";

			animationNames->crouch = "";
			animationNames->stand = "";

			animationNames->jump = "";
			animationNames->fall = "";
			animationNames->land = "";

			animationNames->attack = "attack01";

			return animationNames;
		}

		MonsterAnimationNamesPtr createBerserkerAnimationNames() {
			MonsterAnimationNamesPtr animationNames(new MonsterAnimationNames);
			animationNames->idle = "idle";
			animationNames->crouchedIdle = "";
			animationNames->monsterIdle = "berserkerIdle";

			animationNames->walk = "walking";
			animationNames->diagonalRightWalk = "walk45Right";
			animationNames->diagonalLeftWalk = "walk45Left";
			animationNames->strafingRightWalk = "walkStrafeRight";
			animationNames->strafingLeftWalk = "walkStrafeLeft";
			animationNames->crouchedWalk = "";
			animationNames->crouchedDiagonalRightWalk = "";
			animationNames->crouchedDiagonalLeftWalk = "";
			animationNames->crouchedStrafingRightWalk = "";
			animationNames->crouchedStrafingLeftWalk = "";

			animationNames->run = "run";		
			animationNames->diagonalRightRun = "run45Right";
			animationNames->diagonalLeftRun = "run45Left";
			animationNames->strafingRightRun = "runStrafeRight";
			animationNames->strafingLeftRun = "runStrafeLeft";
			animationNames->crouchedRun = "";
			animationNames->crouchedDiagonalRightRun = "";
			animationNames->crouchedDiagonalLeftRun = "";
			animationNames->crouchedStrafingRightRun = "";
			animationNames->crouchedStrafingLeftRun = "";

			animationNames->crouch = "";
			animationNames->stand = "";

			animationNames->jump = "";
			animationNames->fall = "";
			animationNames->land = "";

			animationNames->attack = "attack";

			return animationNames;
		}

		MonsterAnimationNamesPtr createStatueAnimationNames() {
			MonsterAnimationNamesPtr animationNames(new MonsterAnimationNames);
			animationNames->idle = "idle";
			animationNames->crouchedIdle = "";
			animationNames->monsterIdle = "";

			animationNames->walk = "walking";
			animationNames->diagonalRightWalk = "walk45Right";
			animationNames->diagonalLeftWalk = "walk45Left";
			animationNames->strafingRightWalk = "walkStrafeRight";
			animationNames->strafingLeftWalk = "walkStrafeLeft";
			animationNames->crouchedWalk = "";
			animationNames->crouchedDiagonalRightWalk = "";
			animationNames->crouchedDiagonalLeftWalk = "";
			animationNames->crouchedStrafingRightWalk = "";
			animationNames->crouchedStrafingLeftWalk = "";

			animationNames->run = "";		
			animationNames->diagonalRightRun = "";
			animationNames->diagonalLeftRun = "";
			animationNames->strafingRightRun = "";
			animationNames->strafingLeftRun = "";
			animationNames->crouchedRun = "";
			animationNames->crouchedDiagonalRightRun = "";
			animationNames->crouchedDiagonalLeftRun = "";
			animationNames->crouchedStrafingRightRun = "";
			animationNames->crouchedStrafingLeftRun = "";

			animationNames->crouch = "";
			animationNames->stand = "";

			animationNames->jump = "";
			animationNames->fall = "";
			animationNames->land = "";

			animationNames->attack = "attack";

			return animationNames;
		}

		string getCharacterScript(CharacterType character) {
			switch (character) {
				case CHAR_0:
					return "char0";
					break;
				case CHAR_1:
					return "char1";
					break;
				case CHAR_2:
					return "char2";
					break;
				case CHAR_3:
					return "char3";
					break;
				case CHAR_MONSTER_0:
					return "monster0";
					break;
				case CHAR_MONSTER_1:
					return "monster1";
					break;
				case CHAR_MONSTER_2:
					return "monster2";
					break;
				case CHAR_MONSTER_3:
					return "monster3";
					break;
				default:
					return "char0";
			}
		}

		void removeAiMonsterControllerFromEngine() {
			MonsterType monsterType = monsterController->getMonster()->getType();
			if (monsterType == SPECTRUM) {
				this->core->removeEventHandler(static_ptr_cast<LocalAiSpectrumController>(this->monsterController));
			}else if (monsterType == BLIND) {
				this->core->removeEventHandler(static_ptr_cast<LocalAiBlindController>(this->monsterController));
			}else if (monsterType == BERSERKER) {
				this->core->removeEventHandler(static_ptr_cast<LocalAiBerserkerController>(this->monsterController));
			}else if (monsterType == STATUE) {
				this->core->removeEventHandler(static_ptr_cast<LocalAiStatueController>(this->monsterController));
			}else {
				throw runtime_error("Invalid monster type");
			}
		}

		void removeLocalMonsterControllerFromEngine() {
			MonsterType monsterType = monsterController->getMonster()->getType();
			if (monsterType == SPECTRUM) {
				this->core->removeInputHandler(static_ptr_cast<LocalPlayerSpectrumController>(this->monsterController));
				this->core->removeEventHandler(static_ptr_cast<LocalPlayerSpectrumController>(this->monsterController));
			}else if (monsterType == BLIND) {
				this->core->removeInputHandler(static_ptr_cast<LocalPlayerBlindController>(this->monsterController));
				this->core->removeEventHandler(static_ptr_cast<LocalPlayerBlindController>(this->monsterController));
			}else if (monsterType == BERSERKER) {
				this->core->removeInputHandler(static_ptr_cast<LocalPlayerBerserkerController>(this->monsterController));
				this->core->removeEventHandler(static_ptr_cast<LocalPlayerBerserkerController>(this->monsterController));
			}else if (monsterType == STATUE) {
				this->core->removeInputHandler(static_ptr_cast<LocalPlayerStatueController>(this->monsterController));
				this->core->removeEventHandler(static_ptr_cast<LocalPlayerStatueController>(this->monsterController));
			}else {
				throw runtime_error("Invalid monster type");
			}
		}
	};

	typedef SharedPointer<CharacterLoader> CharacterLoaderPtr;
}

#endif