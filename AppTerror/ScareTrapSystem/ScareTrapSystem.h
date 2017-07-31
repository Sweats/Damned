#ifndef TERROR_SCARE_TRAP_SYSTEM_H
#define TERROR_SCARE_TRAP_SYSTEM_H

#include "Packets/Packets.h"
#include "ScareTrap/ScareTrap.h"
#include "BlindMonsterHearingSystem/BlindMonsterHearingSystem.h"

namespace terror {
	typedef unordered_map<int, ScareTrapPtr> ScareTrapPtrMap;
	typedef std::vector<ScareTrapPtr> ScareTrapPtrVector;
	typedef Signal<void (ScareTrapPtr)> ScareTrapSignal;

	class ScareTrapSystem : public EventHandler, public NetworkObserver {
	public:
		ScareTrapSystem(BlindMonsterHearingSystemPtr blindMonsterHearingSystem, CharacterHandlerPtr characterHandler, CollisionTesterPtr collisionTester, TerrorClientPtr terrorClient)
			: EventHandler(20), NetworkObserver(false), blindMonsterHearingSystem(blindMonsterHearingSystem), characterHandler(characterHandler), 
			collisionTester(collisionTester), terrorClient(terrorClient), autoReset(false), forest(false), autoResetTime(0.0f), time(0.0f), trapsActivationRange(6.0f) {}

		void notifyStageLoaded(StagePtr stage) {			
			ObjectPtrListPtr traps = EntitySystem::getInstance()->getObjectsWithMask(500);
			foreach (ObjectPtr trap, *traps) {
				ScareTrapPtr scareTrap(new ScareTrap(this->collisionTester, trap, this->trapsActivationRange));
				this->scareTraps[scareTrap->getId()] = scareTrap;
				this->scareTrapsLocalId.push_back(scareTrap);
			}
			if (stage->getName() == "Black_Lake_Woods") {
				this->trapsActivationRange = 8.0f;
				this->forest = true;
			}
		}

		void notifyStageUnloaded() {
			this->scareTraps.clear();
			this->scareTrapsLocalId.clear();
			this->trapsActivationRange = 6.0f;
			this->forest = false;
			this->autoReset = false;
			this->autoResetTime = 0.0f;
			this->relativeAmount = 0.0f;
		}

		void notifyMonsterTeleported() {
			foreach(ScareTrapPtrMap::value_type entry, this->scareTraps) {
				MonsterPtr monster = this->characterHandler->getMonster();
				float distance = (monster->getActor()->getPosition() - entry.second->getPosition()).norm();
				if (distance > this->trapsActivationRange) {
					entry.second->disarm();
				}else {
					entry.second->arm();
				}
			}
		}

		void requestArmTrap(int id) {
			if (!this->terrorClient->isHost()) {
				ArmScareTrapRequestPtr armScareTrapRequest(new ArmScareTrapRequest(id));
				this->terrorClient->sendObjectToHost(armScareTrapRequest, true);
				return;
			}

			if (!this->armTrap(id)) {
				return;
			}

			ArmScareTrapPtr armScareTrap(new ArmScareTrap(id));
			this->terrorClient->sendObjectToPlayers(armScareTrap, true);
		}

		Connection connectScareTrapTriggeredEvent(const ScareTrapSignal::slot_type& scareTrapTriggeredEvent) {
			return this->scareTrapTriggeredSignal.connect(scareTrapTriggeredEvent);
		}

		Connection connectScareTrapArmedEvent(const ScareTrapSignal::slot_type& scareTrapArmedEvent) {
			return this->scareTrapArmedSignal.connect(scareTrapArmedEvent);
		}

		void reset(float relativeAmount = 0.5f, bool autoReset = false, float autoResetTime = 0.0f) {
			this->autoReset = autoReset;
			this->autoResetTime = autoResetTime;
			this->relativeAmount = relativeAmount;

			if (!this->terrorClient->isHost()) {
				ResetScareTrapRequestPtr resetScareTrapRequest(new ResetScareTrapRequest(relativeAmount, this->autoReset, this->autoResetTime));
				this->terrorClient->sendObjectToHost(resetScareTrapRequest, true);			
				return;
			}

			int seed = randomNumber(1, 2147483647);
			this->resetBasedOnSeed(relativeAmount, seed);

			ResetScareTrapPtr resetScareTrap(new ResetScareTrap(seed, relativeAmount, this->autoReset, this->autoResetTime));
			this->terrorClient->sendObjectToPlayers(resetScareTrap, true);
		}

		virtual ~ScareTrapSystem() {}

	protected:
		BlindMonsterHearingSystemPtr blindMonsterHearingSystem;
		EntitySystemPtr entitySystem;
		CharacterHandlerPtr characterHandler;
		CollisionTesterPtr collisionTester;
		TerrorClientPtr terrorClient;
		ScareTrapPtrMap scareTraps;
		ScareTrapPtrVector scareTrapsLocalId;
		ScareTrapSignal scareTrapTriggeredSignal;
		ScareTrapSignal scareTrapArmedSignal;
		bool autoReset;
		bool forest;
		float autoResetTime;
		float time;
		float trapsActivationRange;
		float relativeAmount;

	private:
		virtual bool handleEvent(float diffTime) {
			if (!this->terrorClient->isHost()) {
				return true;
			}

			HumanPtrMap& humans = characterHandler->getHumansAlive();
			foreach (ScareTrapPtrMap::value_type& scareTrapEntry, this->scareTraps) {
				ScareTrapPtr scareTrap = scareTrapEntry.second;
				if (scareTrap->isArmed()) {
					foreach (HumanPtrMap::value_type& humanEntry, humans) {
						HumanPtr human = humanEntry.second;
						if (!human->isDead() && scareTrap->isSurvivorInRange(human)) {
							this->triggerScareTrap(scareTrap);							

							this->blindMonsterHearingSystem->notifyTrapTriggered(scareTrap->getPosition());

							TriggerScareTrapPtr triggerScareTrap(new TriggerScareTrap(scareTrap->getId()));
							this->terrorClient->sendObjectToPlayers(triggerScareTrap, true);
						}
					}
				}
			}

			if (this->autoReset) {
				this->time += diffTime;
				if (this->time >= this->autoResetTime) {
					this->time = 0.0f;
					
					int seed = randomNumber(1, 2147483647);
					this->resetBasedOnSeed(this->relativeAmount, seed);

					ResetScareTrapPtr resetScareTrap(new ResetScareTrap(seed, this->relativeAmount, this->autoReset, this->autoResetTime));
					this->terrorClient->sendObjectToPlayers(resetScareTrap, true);
				}
			}
			return true;
		}

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			// Host
			if (udpPacket->getLabel() == RESET_SCARE_TRAP_REQUEST) {
				ResetScareTrapRequestPtr resetScareTrapRequest(new ResetScareTrapRequest);
				if (!udpPacket->buildObject(resetScareTrapRequest)) {
					return;
				}

				this->autoReset = resetScareTrapRequest->getAutoReset();
				this->autoResetTime = resetScareTrapRequest->getAutoResetTime();

				int seed = randomNumber(1, 2147483647);
				this->resetBasedOnSeed(resetScareTrapRequest->getRelativeAmount(), seed);

				ResetScareTrapPtr resetScareTrap(new ResetScareTrap(seed, resetScareTrapRequest->getRelativeAmount(), this->autoReset, this->autoResetTime));
				this->terrorClient->sendObjectToPlayers(resetScareTrap, true);
			}
			if (udpPacket->getLabel() == ARM_SCARE_TRAP_REQUEST) {
				ArmScareTrapRequestPtr armScareTrapRequest(new ArmScareTrapRequest);
				if (!udpPacket->buildObject(armScareTrapRequest)) {
					return;
				}

				if (!this->armTrap(armScareTrapRequest->getId())) {
					return;
				}

				ArmScareTrapPtr armScareTrap(new ArmScareTrap(armScareTrapRequest->getId()));
				this->terrorClient->sendObjectToPlayers(armScareTrap, true);
			}

			// Client
			if (udpPacket->getLabel() == RESET_SCARE_TRAP) {
				ResetScareTrapPtr resetScareTrap(new ResetScareTrap);
				if (!udpPacket->buildObject(resetScareTrap)) {
					return;
				}

				this->autoReset = resetScareTrap->getAutoReset();
				this->autoResetTime = resetScareTrap->getAutoResetTime();

				this->resetBasedOnSeed(resetScareTrap->getRelativeAmount(), resetScareTrap->getSeed());
			}
			if (udpPacket->getLabel() == ARM_SCARE_TRAP) {
				ArmScareTrapPtr armScareTrap(new ArmScareTrap);
				if (!udpPacket->buildObject(armScareTrap)) {
					return;
				}
				this->armTrap(armScareTrap->getId());
			}
			if (udpPacket->getLabel() == TRIGGER_SCARE_TRAP) {
				TriggerScareTrapPtr triggerScareTrap(new TriggerScareTrap);
				if (!udpPacket->buildObject(triggerScareTrap)) {
					return;
				}
				ScareTrapPtr scareTrap = this->getScareTrap(triggerScareTrap->getId());
				if (!scareTrap) {
					return;
				}
				this->triggerScareTrap(scareTrap);				
			}
		}

		void resetBasedOnSeed(float relativeAmount, int seed) {
			RandomizerPtr randomizer(new Randomizer(seed));

			foreach (ScareTrapPtrMap::value_type& entry, this->scareTraps) {
				ScareTrapPtr scareTrap = entry.second;
				scareTrap->disarm();
				scareTrap->disable();
			}

			int trapsToActivate = 0;
			if (this->autoReset) {
				if (forest) {
					trapsToActivate = 0.3f*this->scareTrapsLocalId.size();
				}else {
					trapsToActivate = this->scareTrapsLocalId.size()/5;
				}
			}else {
				trapsToActivate = int(relativeAmount*float(this->scareTrapsLocalId.size()));
			}
			int trapsActivated = 0;
			while (trapsActivated < trapsToActivate) {
				int scareTrapLocalId = randomizer->randomNumber(0, this->scareTrapsLocalId.size() - 1);
				ScareTrapPtr scareTrap = this->scareTrapsLocalId[scareTrapLocalId];
				if (!scareTrap->isEnabled()) {					
					if (this->autoReset) {
						scareTrap->arm();
					}else {
						scareTrap->enable();
					}
					++trapsActivated;
				}
			}
		}

		bool armTrap(int id) {
			ScareTrapPtr scareTrap = this->getScareTrap(id);
			if (!scareTrap || !scareTrap->isEnabled()) {
				return false;
			}
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (playerInfo && isMonsterCharacter(playerInfo->getCharacter())) {
				SoundPtr armSoundEffect = scareTrap->getArmSoundEffect();
				if (armSoundEffect) {
					armSoundEffect->play();
				}
				SteamSystem::getInstance()->incrementStat("traps_activated", 1);
			}					
			scareTrap->arm();
			scareTrap->disable();
			this->scareTrapArmedSignal(scareTrap);
			return true;
		}

		void triggerScareTrap(ScareTrapPtr scareTrap) {
			scareTrap->trigger();
			this->scareTrapTriggeredSignal(scareTrap);

			if (!PlayersState::getInstance()->monsterPlayerExists()) {
				return;
			}
			int id = PlayersState::getInstance()->getMonsterPlayerId();
			if (this->terrorClient->getId() == id) {
				MonsterPtr monster = this->characterHandler->getMonster();
				if (monster && monster->getType() == STATUE) {
					return;
				}

				scareTrap->enableSeeThroughMaterial();
				EventSequencePtr sequence(new EventSequence);

				EventDelayerPtr eventDelayer(new EventDelayer(2.0f));
				sequence->addEvent(eventDelayer);

				EventFunctionPtr eventFunction(new EventFunction(Bind(&ScareTrap::restoreMaterial, scareTrap)));
				sequence->addEvent(eventFunction);

				Core::getInstance()->runEvent(sequence);
			}
		}

		ScareTrapPtr getScareTrap(int id) {
			ScareTrapPtrMap::const_iterator entry = this->scareTraps.find(id);
			if (entry == this->scareTraps.end()) {
				return ScareTrapPtr();
			}
			return entry->second;
		}
	};

	typedef SharedPointer<ScareTrapSystem> ScareTrapSystemPtr;
}

#endif