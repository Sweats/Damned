#ifndef TERROR_STAGE_LOADER_H
#define TERROR_STAGE_LOADER_H

#include "StageBuilder/StageBuilder.h"
#include "StageFactory/StageFactory.h"
#include "MenuSceneLoader/MenuSceneLoader.h"

namespace terror {
	class StageHandler : public NetworkObserver, public EventHandler {
	public:
		typedef Signal<void (StagePtr)> StageSignal;

		StageHandler(CorePtr core, SceneBuilderPtr sceneBuilder, SceneFactoryPtr sceneFactory, MenuSceneLoaderPtr menuSceneLoader, TerrorClientPtr terrorClient)
			: EventHandler(20), NetworkObserver(false), entitySystem(EntitySystem::getInstance()), 
			stageBuilder(new StageBuilder(core->getParseTreeFactory(), sceneBuilder)),
			stageFactory(new StageFactory(sceneFactory, core->getPhysicsSystem())),
			parseTreeFactory(core->getParseTreeFactory()), stageLoadRequested(false), stageUnloadRequested(false), framesToSleep(0),
			menuSceneLoader(menuSceneLoader), terrorClient(terrorClient), stageInfo(new StageInfo) {}

		void requestLoadStage(const std::string& stageName) {
			GameSessionStageNamePtr gameSessionStageName(new GameSessionStageName(stageName));
			this->terrorClient->sendObject(gameSessionStageName);

			int seed = randomNumber(1, 2147483647);
			this->stageInfo->name = stageName;
			this->stageInfo->seed = seed;
			this->stageLoadRequested = true;
			this->framesToSleep = 5;

			this->terrorClient->sendObjectToPlayers(this->stageInfo, true);
		}

		void resetStage() {
			this->stageInfo->name = "";
			this->stageInfo->seed = -1;
		}

		void requestUnloadStage() {
			if (this->terrorClient->isHost()) {
				GameSessionStageNamePtr gameSessionStageName(new GameSessionStageName(""));
				this->terrorClient->sendObject(gameSessionStageName);
			}
			this->resetStage();
			this->stageUnloadRequested = true;
			this->framesToSleep = 5;
		}

		void connectStageLoadedEvent(const StageSignal::slot_type& stageLoadedEvent) {
			this->stageLoadedSignal.connect(stageLoadedEvent);
		}

		void connectStageUnloadedEvent(const BasicSignal::slot_type& stageUnloadedEvent) {
			this->stageUnloadedSignal.connect(stageUnloadedEvent);
		}

		void notifyLocalPlayerMonsterLoaded(MonsterPtr monster) {
			this->stage->getScene()->stopAmbientMusic();
		}

		virtual ~StageHandler() {}

	protected:
		MenuSceneLoaderPtr menuSceneLoader;

		EntitySystemPtr entitySystem;
		StageBuilderPtr stageBuilder;
		StageFactoryPtr stageFactory;
		ParseTreeFactoryPtr parseTreeFactory;

		StageSignal stageLoadedSignal;
		BasicSignal stageUnloadedSignal;

		StagePtr stage;
		StageInfoPtr stageInfo;

		bool stageLoadRequested;
		bool stageUnloadRequested;
		int framesToSleep;

		TerrorClientPtr terrorClient;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			//Host
			if (udpPacket->getLabel() == GREETINGS) {
				if (!this->terrorClient->isHost() || this->stageInfo->name == "") {
					return;
				}
				this->terrorClient->sendObjectTo(this->stageInfo, udpPacket->getRemoteUdpAddress(), true);
			}
			if (udpPacket->getLabel() == STAGE_STATE_REQUEST) {
				if (!this->stage) {
					return;
				}				
				this->terrorClient->sendObjectTo(this->stage->getState(), udpPacket->getRemoteUdpAddress(), true);
			}

			//Client
			if (udpPacket->getLabel() == STAGE_INFO) {
				if (this->stage) {
					return;
				}

				if (!udpPacket->buildObject(this->stageInfo)) {
					return;
				}

				this->menuSceneLoader->unloadMenuScene();
				this->stageLoadRequested = true;
				this->framesToSleep = 5;
			}
			if (udpPacket->getLabel() == STAGE_STATE) {
				if (!this->stage) {
					return;
				}

				StageStatePtr stageState(new StageState);
				if (!udpPacket->buildObject(stageState)) {
					return;
				}

				foreach (ObjectInfoPtr objectInfo, stageState->objectInfos) {
					ObjectPtr object = this->entitySystem->getObject(objectInfo->id);
					if (object) {
						if (object->isLocked()) {
							object->unlock(false);
						}
						object->setPosition(objectInfo->position.buildVector3d());
						object->setOrientation(objectInfo->orientation.buildQuaternion());
					}
				}
				foreach (int id, stageState->destroyedObjectIds) {
					this->stage->destroyObject(id);
				}
			}
		}

		virtual bool handleEvent(float diffTime) {
			if (!this->stageLoadRequested && !this->stageUnloadRequested) {
				return true;
			}

			--this->framesToSleep;
			if (this->framesToSleep > 0) {
				return true;
			}

			if (this->stageLoadRequested) {
				this->loadStage();
				this->stageLoadRequested = false;
				if (!this->terrorClient->isHost()) {
					StageStateRequestPtr stageStateRequest(new StageStateRequest);
					this->terrorClient->sendObjectToHost(stageStateRequest, true);
				}
				return true;
			}
			if (this->stageUnloadRequested) {
				this->unloadStage();
				this->stageUnloadRequested = false;				
			}
			return true;
		}

		void loadStage() {
			Core::getInstance()->switchState("Pregame");
			if (this->stage) {
				this->stage->destroy();
				this->stage.reset();
				this->stageUnloadedSignal();
			}else {
				this->menuSceneLoader->unloadMenuScene();
			}

			NodePtr stageNode = this->parseTreeFactory->create(this->stageInfo->name + ".stage")->getChildByType("stage", true);
			StageDataPtr stageData = this->stageBuilder->build(stageNode);

			EntitySystem::getInstance()->destroyAllObjects();
			this->stage = this->stageFactory->create(stageData, this->stageInfo->seed);
			this->stageLoadedSignal(stage);
		}

		void unloadStage() {
			Core::getInstance()->switchState("Menu");
			if (this->stage) {
				this->stage->destroy();
				this->stage.reset();
			}
			this->menuSceneLoader->loadMenuScene();
			this->stageUnloadedSignal();
		}
	};
	typedef SharedPointer<StageHandler> StageHandlerPtr;
}

#endif