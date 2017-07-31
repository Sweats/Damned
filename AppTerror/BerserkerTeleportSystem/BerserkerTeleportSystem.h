#ifndef TERROR_BERSERKER_TELEPORT_SYSTEM_H
#define TERROR_BERSERKER_TELEPORT_SYSTEM_H

#include "Packets/BerserkerMonsterTeleport/BerserkerMonsterTeleport.h"
#include "Packets/BerserkerMonsterTeleportRequest/BerserkerMonsterTeleportRequest.h"
#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	typedef PointerList<Human> HumanPtrList;

	class BerserkerTeleportSystem : public EventHandler, public NetworkObserver {
	public:
		BerserkerTeleportSystem(CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient, CollisionTesterPtr collisionTester, RenderCameraPtr camera, SoundPtr teleportSound)
			: EventHandler(20), NetworkObserver(false), characterHandler(characterHandler), terrorClient(terrorClient), collisionTester(collisionTester), camera(camera), 
			teleportSound(teleportSound), inBerserkerMode(false), lastPortalId(-1), timeToAutoTeleport(-1.0f) {}
			
		void notifyStageLoaded(StagePtr stage) {
			MonsterPtr monster = this->characterHandler->getMonster();
			if (monster && monster->getType() == BERSERKER) {
				this->berserker = static_ptr_cast<Berserker>(monster);
				this->berserker->connectBecomeBerserkerEvent(Bind(&BerserkerTeleportSystem::notifyMonsterInBerserkerMode, this));
				this->berserker->connectBecomeGhostEvent(Bind(&BerserkerTeleportSystem::notifyMonsterInGhostMode, this));
				this->energyForTeleporting = this->berserker->getMinEnergyForTeleporting();
			}
			this->stage = stage;
		}

		void notifyStageUnloaded() {
			this->lastPortalId = -1;
			this->inBerserkerMode = false;
			this->timeToAutoTeleport = this->computeTimeToAutoTeleport();
			this->berserker.reset();
			this->stage.reset();
		}

		bool tryTeleport() {
			if (!this->stage || !this->berserker) {
				return false;
			}

			EnergyPtr energy = this->berserker->getEnergy();
			if (energy->getValue() > this->energyForTeleporting) {
				if (this->terrorClient->isHost()) {
					int portalId = this->randomizePortalId();
					BerserkerMonsterTeleportPtr berserkerMonsterTeleport(new BerserkerMonsterTeleport(energy->getValue(), portalId));
					this->terrorClient->sendObjectToPlayers(berserkerMonsterTeleport, true);
					this->teleport(portalId);
				}else {
					BerserkerMonsterTeleportRequestPtr berserkerMonsterTeleportRequest(new BerserkerMonsterTeleportRequest);
					this->terrorClient->sendObjectToHost(berserkerMonsterTeleportRequest, true);
				}
				return true;
			}
			return false;
		}

		void connectMonsterTeleportedEvent(const BasicSignal::slot_type& monsterTeleportedEvent) {
			this->monsterTeleportedSignal.connect(monsterTeleportedEvent);
		}

		virtual ~BerserkerTeleportSystem() {}

	protected:
		CharacterHandlerPtr characterHandler;
		BerserkerPtr berserker;
		TerrorClientPtr terrorClient;
		StagePtr stage;
		BasicSignal monsterTeleportedSignal;
		CollisionTesterPtr collisionTester;
		RenderCameraPtr camera;
		SoundPtr teleportSound;
		float timeToAutoTeleport;
		float energyForTeleporting;
		int lastPortalId;
		bool inBerserkerMode;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->terrorClient->isHost() || !this->berserker || this->inBerserkerMode) {
				return true;
			}
			
			this->timeToAutoTeleport -= diffTime;
			if (this->timeToAutoTeleport < 0.0f) {
				this->autoTeleport();
			}
			return true;
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (!this->berserker) {
				return;
			}

			if (udpPacket->getLabel() == BERSERKER_MONSTER_TELEPORT) {
				BerserkerMonsterTeleportPtr berserkerMonsterTeleport(new BerserkerMonsterTeleport);
				if (!udpPacket->buildObject(berserkerMonsterTeleport)) {
					return;
				}
				EnergyPtr energy = this->berserker->getEnergy();
				energy->setValue(berserkerMonsterTeleport->getEnergy());
				int portalId = berserkerMonsterTeleport->getPortalId();
				bool autoTeleport = berserkerMonsterTeleport->isAutoTeleport();
				this->teleport(portalId, !autoTeleport);
			}

			if (udpPacket->getLabel() == BERSERKER_MONSTER_TELEPORT_REQUEST) {
				int portalId = this->randomizePortalId();
				float energyValue = this->berserker->getEnergy()->getValue();
				BerserkerMonsterTeleportPtr berserkerMonsterTeleport(new BerserkerMonsterTeleport(energyValue, portalId));
				this->terrorClient->sendObjectToPlayers(berserkerMonsterTeleport, true);
				this->teleport(portalId);
			}
		}

		void notifyMonsterInBerserkerMode() {
			this->inBerserkerMode = true;
		}

		void notifyMonsterInGhostMode() {
			if (this->inBerserkerMode) {
				this->autoTeleport();
			}
			this->inBerserkerMode = false;
			this->timeToAutoTeleport = this->computeTimeToAutoTeleport();
		}

		void autoTeleport() {
			if (!this->terrorClient->isHost() || !this->berserker || !this->stage) {
				return;
			}
			float monsterEnergy = this->berserker->getEnergy()->getValue();
			int portalId = this->randomizePortalId();
			if (portalId != -1) {
				this->teleport(portalId, false);
				BerserkerMonsterTeleportPtr berserkerMonsterTeleport(new BerserkerMonsterTeleport(monsterEnergy, portalId, true));
				this->terrorClient->sendObjectToPlayers(berserkerMonsterTeleport, true);
			}
		}

		void teleport(int portalId, bool consumeEnergy = true) {
			if (portalId < 5 || !this->berserker || !this->stage) {
				return;
			}
			
			SpawnPointPtr portal = this->stage->getScene()->getSpawnPoint(portalId);
			if (!portal) {
				return;
			}

			this->timeToAutoTeleport = this->computeTimeToAutoTeleport();
			this->berserker->getActor()->setPosition(portal->getPosition());
			if (consumeEnergy) {
				this->berserker->getEnergy()->spend(this->energyForTeleporting);
			}
			PlayerInfoPtr playerInfo = PlayersState::getInstance()->getPlayerInfo(this->terrorClient->getId());
			if (playerInfo && isMonsterCharacter(playerInfo->getCharacter())) {
				if (!this->pointToClosestInTheSameRoomHuman()) {
					Vector3d direction = portal->getOrientation().getZAxis();
					this->camera->setDirection(toOgre(direction));
				} 
				this->teleportSound->play();
			}
			this->monsterTeleportedSignal();
		}

		int randomizePortalId() {
			if (!this->stage) {
				throw std::runtime_error("Stage not initialized in BerserlerTeleportSystem");
			}
			int spawnPointsSize = this->stage->getScene()->getSpawnPoints().size();
			if (spawnPointsSize - 1 < 5) {
				return -1;
			}
			int portalId = randomNumber(5, spawnPointsSize - 1);
			if (portalId == this->lastPortalId) {
				return this->randomizePortalId();
			}else {
				this->lastPortalId = portalId;
				return portalId;
			}
		}

		bool isHumanInTheSameRoom(HumanPtr human) {
			ClosestRayResultCallback callback;
			callback.addIgnoreObject(this->berserker->getActor()->getCollisionObject().get());
			foreach(HumanPtrMap::value_type entry, this->characterHandler->getHumansAlive()) {
				callback.addIgnoreObject(entry.second->getActor()->getCollisionObject().get());
			}
			Vector3d from = this->berserker->getActor()->getPosition() + 1.0*Vector3d::yUnit;
			Vector3d to = human->getActor()->getPosition() + 1.0*Vector3d::yUnit;
			return !this->collisionTester->rayTest(toBullet(from), toBullet(to), callback);
		}

		bool pointToClosestInTheSameRoomHuman() {
			HumanPtrList humansIgnored;
			HumanPtr human = this->pickClosestHuman(humansIgnored);
			if (human) {
				Vector3d direction = human->getActor()->getPosition() - this->berserker->getActor()->getPosition();
				direction.setY(0.0f);
				direction.normalize();
				this->camera->setDirection(toOgre(direction));
				return true;
			}
			return false;
		}

		HumanPtr pickClosestHuman(HumanPtrList& humansIgnored) {
			HumanPtrMap humans = this->characterHandler->getHumansAlive();
			HumanPtr closestHuman;
			float smallerDistance = -1.0f;
			foreach(HumanPtrMap::value_type entry, humans) {
				if (!humansIgnored.contains(entry.second)) {
					Vector3d direction = entry.second->getActor()->getPosition() - this->berserker->getActor()->getPosition();
					float distance = direction.norm();
					if (smallerDistance < 0.0f) {
						smallerDistance = distance;
						closestHuman = entry.second;
					}else if (smallerDistance > distance) {
						smallerDistance = distance;
						closestHuman = entry.second;
					}
				}
			}
			if (!closestHuman) {
				return HumanPtr();
			}
			if (!this->isHumanInTheSameRoom(closestHuman)) {
				humansIgnored.push_back(closestHuman);
				return this->pickClosestHuman(humansIgnored);
			}
			return closestHuman;
		}

		float computeTimeToAutoTeleport() {
			if (this->stage && this->stage->getName() == "Black_Lake_Woods") {
				return randomNumber(25.0f, 45.0f);
			}
			return randomNumber(40.0f, 100.0f);
		}
	};

	typedef SharedPointer<BerserkerTeleportSystem> BerserkerTeleportSystemPtr;
}

#endif