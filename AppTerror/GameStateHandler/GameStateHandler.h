#ifndef TERROR_GAME_STATE_HANDLER_H
#define TERROR_GAME_STATE_HANDLER_H

#include "Stage/Stage.h"
#include "HumanController/HumanController.h"
#include "HumanController/LocalPlayerHumanController/LocalPlayerHumanController.h"
#include "HumanController/RemotePlayerHumanController/RemotePlayerHumanController.h"
#include "DeathSystem/DeathSystem.h" 
#include "MonsterControllers/MonsterController.h"
#include "CharacterLoader/CharacterLoader.h"

namespace terror {
	typedef IntPointerMap<ObjectInfo> ObjectInfoPtrMap;
	typedef IntPointerMap<HumanController> HumanControllerPtrMap;
	typedef PointerList<RemotePlayerHumanController> RemotePlayerHumanControllerPtrList;

	class GameStateHandler : public NetworkObserver {
	public:
		GameStateHandler(PhysicsSystemPtr physicsSystem, DeathSystemPtr deathSystem, CharacterHandlerPtr characterHandler,
			CharacterLoaderPtr characterLoader, TerrorClientPtr terrorClient, CorePtr core)
			:  NetworkObserver(false), physicsSystem(physicsSystem), deathSystem(deathSystem), characterHandler(characterHandler),
			characterLoader(characterLoader), terrorClient(terrorClient), core(core)
		{
			this->monsterInvisibleSound = this->core->createSound("monster_out.ogg");
			this->monsterInvisibleSound->setRepeat(false);
			this->monsterVisibleSound = this->core->createSound("monster_trans.ogg");
			this->monsterVisibleSound->setRepeat(false);
		}		

		void notifyHumanDeath(HumanPtr human) {
			HumanControllerPtr humanController = this->humanControllers.get(human->getCharacterType());
			this->humanControllers.erase(human->getCharacterType());

			if (!humanController || humanController->getType() == LOCAL_HUMAN_CONTROLLER) {
				return;
			}

			RemotePlayerHumanControllerPtr deadController = static_ptr_cast<RemotePlayerHumanController>(humanController);
			this->addDeadHumanController(deadController);
		}

		void notifyLocalHumanRespawning(HumanPtr human) {
			if (!this->localHumanController || this->localHumanController->getHuman() != human) {
				return;
			}

			CharacterType characterType = human->getCharacterType();
			if (characterType == CHAR_NONE) {
				return;
			}

			RemotePlayerHumanControllerPtr remoteController = this->characterLoader->transformLocalPlayerHumanInRemote(this->localHumanController);
			this->addDeadHumanController(remoteController);

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			SpawnPointPtr spawnPoint = this->stage->getScene()->getSpawnPoint(playerInfo->getSpawnPointId());
			this->localHumanController->getHuman()->getInventory()->setLocal(false);
			this->localHumanController->destroy(false);
			this->addHumanController(this->characterLoader->loadLocalPlayerHuman(characterType, spawnPoint, (this->stage->getName() == "Black_Lake_Woods")), true);
		}

		void setStage(StagePtr stage) {
			this->stage = stage;
			ObjectPtrListPtr dynamicObjects = EntitySystem::getInstance()->getDynamicObjects();
			foreach (ObjectPtr object, *dynamicObjects) {
				object->getMotionState()->connectUpdatedEvent(Bind(&GameStateHandler::addToUpdatedList, this, _1, _2, _3));
			}
		}

		StagePtr getStage() const {
			return this->stage;
		}

		void setDestroyedObjects(const std::list<int>& destroyedObjectIds) {
			foreach (int destroyedObjectId, destroyedObjectIds) {
				this->stage->destroyObject(destroyedObjectId);
			}
		}

		void addHumanController(HumanControllerPtr humanController, bool isLocal = false) {
			this->humanControllers[humanController->getHuman()->getCharacterType()] = humanController;
			if (isLocal) {
				this->localHumanController = static_ptr_cast<LocalPlayerHumanController>(humanController);
			}
			if (this->monsterController && this->monsterController->getMonster()->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(this->monsterController->getMonster());
				if (spectrum->isVisible()) {
					return;
				}
				if (isLocal) {
					ActorPtr actor = spectrum->getActor();
					actor->hide();
				}else if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
					HumanPtr human = humanController->getHuman();
					human->hide();
				}
			}
			if (this->monsterController && this->monsterController->getMonster()->getType() == BLIND) {
				if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
					HumanPtr human = humanController->getHuman();
					human->hide();
				}
			}
			if (this->monsterController && this->monsterController->getMonster()->getType() == STATUE) {
				StatuePtr statue = static_ptr_cast<Statue>(this->monsterController->getMonster());
				statue->notifyHumanAdded(humanController->getHuman());
			}
		}

		void addDeadHumanController(RemotePlayerHumanControllerPtr remotePlayerHumanController) {
			this->deadHumanControllers.push_back(remotePlayerHumanController);
		}

		void setMonsterController(MonsterControllerPtr monsterController) {
			this->monsterController = monsterController;
			if (this->monsterController->getMonster()->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(this->monsterController->getMonster());
				spectrum->connectBecomeInvisibleEvent(Bind(&GameStateHandler::notifyMonsterInvisible, this));
				spectrum->connectBecomeVisibleEvent(Bind(&GameStateHandler::notifyMonsterVisible, this));
			}
			if (this->monsterController->getMonster()->getType() == BLIND) {
				if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
					foreach (HumanControllerPtrMap::value_type& entry, this->humanControllers) {
						HumanPtr human = entry.second->getHuman();
						human->hide();
					}
					ObjectPtrListPtr specialObjects = EntitySystem::getInstance()->getDynamicObjects();
					foreach (ObjectPtr specialObject, *specialObjects) {
						specialObject->hide();
					}
				}
			}
			if (this->monsterController && this->monsterController->getMonster()->getType() == STATUE) {
				StatuePtr statue = static_ptr_cast<Statue>(this->monsterController->getMonster());
				foreach (HumanControllerPtrMap::value_type& entry, this->humanControllers) {
					HumanPtr human = entry.second->getHuman();
					statue->notifyHumanAdded(human);
				}				
			}
		}		

		void applyHumanInfo(HumanInfoPtr humanInfo) {
			HumanControllerPtr humanController = this->humanControllers.get(humanInfo->character);
			if (humanController) {
				humanController->updateHumanAndController(humanInfo);
			}
		}

		void applyMonsterActionRequest(MonsterActionRequestPtr monsterActionRequest) {
			this->monsterController->update(monsterActionRequest);
		}

		void updateHumanInfo(HumanInfoPtr humanInfo) {
			HumanControllerPtr humanController = this->humanControllers.get(humanInfo->character);
			if (humanController) {
				humanController->update(humanInfo);
			}
		}

		void updateMonsterInfo(MonsterInfoPtr monsterInfo) {
			this->monsterController->update(monsterInfo);
		}

		void updateObjectInfo(ObjectInfoPtr objectInfo) {
			ObjectPtr object = EntitySystem::getInstance()->getObject(objectInfo->id);
			if (!object) {
				return;
			}
			if (object->isLocked()) {
				object->unlock();
			}
			object->resetMomentum();
			object->setPosition(objectInfo->position.buildVector3d(), 0.1f);
			object->setOrientation(objectInfo->orientation.buildQuaternion(), 0.1f);
		}		

		bool humanExists(int character) {
			bool humanFound = this->humanControllers.find(character) != this->humanControllers.end();
			if (!humanFound) {
				HumanPtrMap& deadHumans = this->characterHandler->getDeadHumans();
				foreach (HumanPtrMap::value_type& entry, deadHumans) {
					if (entry.second->getCharacterType() == character) {
						humanFound = true;
					}
				}
			}
			return humanFound;
		}

		bool isHumanAlive(int character) {
			return this->humanControllers.find(character) != this->humanControllers.end();
		}

		bool isHumanDead(int character) {
			return this->humanExists(character) && this->humanControllers.find(character) == this->humanControllers.end();
		}

		bool notifyHumanDisconnected(int character) {
			if (!this->isHumanAlive(character)) {
				return false;
			}
			this->deathSystem->doKillCharacter(CharacterType(character), true);
			return true;
		}

		bool monsterExists() {
			return this->monsterController != 0;
		}

		LocalPlayerHumanControllerPtr getLocalHumanController() {
			return this->localHumanController;
		}

		HumanControllerPtrMap& getHumanControllers() {
			return this->humanControllers;
		}

		MonsterControllerPtr getMonsterController() {
			return this->monsterController;
		}

		RemotePlayerHumanControllerPtrList& getDeadHumanControllers() {
			return this->deadHumanControllers;
		}

		GameStatePtr getGameState() {		
			GameStatePtr gameState(new GameState);
			foreach (HumanControllerPtrMap::value_type& entry, this->humanControllers) {
				gameState->charactersGameState->humans.add(entry.second->getHuman()->getInfo());
			}
			if (this->monsterController) {
				gameState->charactersGameState->monster = this->monsterController->getMonster()->getInfo();
			}

			foreach (ObjectInfoPtrMap::value_type& entry, this->objectInfos) {
				gameState->objectsGameState->objects.add(entry.second);
			}
			this->objectInfos.clear();

			gameState->destroyedObjectsGameState->destroyedObjectIds = this->destroyedObjectIds;
			this->destroyedObjectIds.clear();
			return gameState;
		}

		void reset() {
			this->stage.reset();

			this->humanControllers.clear();
			this->deadHumanControllers.clear();

			this->localHumanController.reset();
			this->monsterController.reset();
			this->characterHandler->resetDeadHumans();

			this->objectInfos.clear();
			this->destroyedObjectIds.clear();
		}

		virtual ~GameStateHandler() {}

	protected:
		PhysicsSystemPtr physicsSystem;
		DeathSystemPtr deathSystem;
		CharacterLoaderPtr characterLoader;
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;

		StagePtr stage;

		HumanControllerPtrMap humanControllers;
		LocalPlayerHumanControllerPtr localHumanController;
		MonsterControllerPtr monsterController;
		RemotePlayerHumanControllerPtrList deadHumanControllers;

		SoundPtr monsterInvisibleSound;
		SoundPtr monsterVisibleSound;

		ObjectInfoPtrMap objectInfos;
		std::list<int> destroyedObjectIds;

		CorePtr core;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (udpPacket->getLabel() == DEAD_CHARACTERS) {
				if (!this->stage) {
					return;
				}

				DeadCharactersPtr deadCharacters(new DeadCharacters);
				if (!udpPacket->buildObject(deadCharacters)) {
					return;
				}
				DeadHumanDataPtrList& deadHumansData = deadCharacters->getDeadCharactersData();
				foreach (DeadHumanDataPtr deadHumanData, deadHumansData) {
					RemotePlayerHumanControllerPtr controller = this->characterLoader->loadRemotePlayerDeadHuman(deadHumanData, (this->stage->getName() == "Black_Lake_Woods"));
					HumanPtr deadHuman = this->characterHandler->getDeadHuman(deadHumanData->getId());
					CharacterItemsPtr characterItems = deadHumanData->getCharacterItems();
					foreach (int itemMask, characterItems->getItemMasks()) {						
						ItemPtr item(new Item(itemMask));
						deadHuman->addItem(item, false);
					}
					this->addDeadHumanController(controller);
				}
			}
		}
		void addToUpdatedList(int id, const Vector3d& position, const Quaternion& orientation) {
			ObjectPtr object = EntitySystem::getInstance()->getObject(id);
			if (!object) {
				return;
			}
			ObjectInfoPtr objectInfo(new ObjectInfo(object));
			this->objectInfos[id] = objectInfo;
		}

		void notifyMonsterInvisible() {
			//this->monsterInvisibleSound->stop();
			//this->monsterInvisibleSound->play();
			if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
				foreach (HumanControllerPtrMap::value_type& entry, this->humanControllers) {
					HumanPtr human = entry.second->getHuman();
					human->hide();
				}
				ObjectPtrListPtr specialObjects = EntitySystem::getInstance()->getDynamicObjects();
				foreach (ObjectPtr specialObject, *specialObjects) {
					specialObject->hide();
				}
			}/*else {
			 ActorPtr actor = this->monsterController->getMonster()->getActor();
			 actor->hide();
			 }*/
		}

		void notifyMonsterVisible() {			
			if (this->monsterController->getType() == LOCAL_MONSTER_CONTROLLER) {
				foreach (HumanControllerPtrMap::value_type& entry, this->humanControllers) {
					HumanPtr human = entry.second->getHuman();
					human->show();
				}
				ObjectPtrListPtr specialObjects = EntitySystem::getInstance()->getDynamicObjects();
				foreach (ObjectPtr specialObject, *specialObjects) {
					specialObject->show();
				}
			}/*else {
			 //this->monsterVisibleSound->stop();
			 //this->monsterVisibleSound->play();
			 ActorPtr actor = this->monsterController->getMonster()->getActor();
			 actor->show();
			 }*/
		}
	};

	typedef SharedPointer<GameStateHandler> GameStateHandlerPtr;
}

#endif