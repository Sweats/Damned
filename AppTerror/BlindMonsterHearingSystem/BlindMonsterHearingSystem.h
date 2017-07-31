#ifndef TERROR_BLIND_MONSTER_HEARING_SYSTEM_H
#define TERROR_BLIND_MONSTER_HEARING_SYSTEM_H

#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	typedef SharedPointer<float> FloatPtr;
	typedef unordered_map<int, FloatPtr> FloatPtrMap;

	class BlindMonsterHearingSystem : public EventHandler, public NetworkObserver {
	public:
		BlindMonsterHearingSystem(SoundPtr hearingSound, SoundPtr notHearingSound, SoundPtr fleeingSound, 
			CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient) 
			: EventHandler(20), NetworkObserver(false), hearingSound(hearingSound), notHearingSound(notHearingSound), fleeingSound(fleeingSound),
			characterHandler(characterHandler), terrorClient(terrorClient), ingame(false) {}

		void notifyStageLoaded(StagePtr stage) {
			this->ingame = true;
			this->forest = stage->getName() == "Black_Lake_Woods";
		}

		void notifyStageUnloaded() {
			this->hearingTimers.clear();
			this->hearingSound->stop();
			this->notHearingSound->stop();
			this->fleeingSound->stop();
			this->ingame = false;
		}

		void notifyTrapTriggered(const Vector3d& trapPosition) {
			MonsterPtr monster = this->characterHandler->getMonster();
			if (!monster || monster->getType() != BLIND) {
				return;
			}

			Vector3d monsterPosition = monster->getActor()->getPosition();
			HumanPtrMap& humans = this->characterHandler->getHumansAlive();
			BlindMonsterHearsPtr blindMonsterHears(new BlindMonsterHears);
			foreach (HumanPtrMap::value_type& entry, humans) {
				HumanPtr human = entry.second;
				if (!human->isDead()) {
					Vector3d humanToTrap = trapPosition - human->getActor()->getPosition();
					Vector3d humanToMonster = monsterPosition - human->getActor()->getPosition();
					if (humanToTrap.norm() <= 5.0f && humanToMonster.norm() <= 20.0f) {
						CharacterType character = human->getCharacterType();
						blindMonsterHears->characters.push_back(character);
						this->doBlindMonsterHears(character);
					}
				}
			}
			if (!blindMonsterHears->characters.empty()) {
				this->terrorClient->sendObjectToPlayers(blindMonsterHears);
			}
		}		

		virtual ~BlindMonsterHearingSystem() {}

	protected:
		SoundPtr hearingSound;
		SoundPtr notHearingSound;
		SoundPtr fleeingSound;
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		FloatPtrMap hearingTimers;
		bool ingame;
		bool forest;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			// Client
			if (udpPacket->getLabel() == BLIND_MONSTER_HEARS) {
				BlindMonsterHearsPtr blindMonsterHears(new BlindMonsterHears);
				if (!udpPacket->buildObject(blindMonsterHears)) {
					return;
				}
				foreach (CharacterType character, blindMonsterHears->characters) {
					this->doBlindMonsterHears(character);
				}
			}
		}

		virtual bool handleEvent(float diffTime) {
			if (!this->ingame || this->terrorClient->getId() < 0) {
				return true;
			}

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (playerInfo &&  playerInfo->getCharacter() == CHAR_MONSTER_1 && !this->hearingSound->isPlaying() && !this->notHearingSound->isPlaying()) {
				this->notHearingSound->play();
			}

			FloatPtrMap tmpTimers = this->hearingTimers;
			foreach (FloatPtrMap::value_type& timerEntry, tmpTimers) {
				CharacterType character = CharacterType(timerEntry.first);
				FloatPtr timer = timerEntry.second;
				*timer -= diffTime;
				if (*timer <= 0.0f) {
					if (playerInfo &&  playerInfo->getCharacter() == CHAR_MONSTER_1) {
						HumanPtrMap& humans = this->characterHandler->getHumansAlive();
						HumanPtr human = humans.get(character);
						if (human) {
							human->hide();
							human->getActor()->restoreMaterials();
							human->getActor()->setMaterialId(0);
						}else {
							HumanPtrMap& deadHumans = this->characterHandler->getDeadHumans();
							foreach (HumanPtrMap::value_type& deadHumanEntry, deadHumans) {
								HumanPtr deadHuman = deadHumanEntry.second;
								if (deadHuman->getCharacterType() == character) {
									deadHuman->hide();
									deadHuman->getActor()->restoreMaterials();
									deadHuman->getActor()->setMaterialId(0);
								}
							}
						}
						this->hearingSound->stop();
						this->notHearingSound->play();
					}else if (playerInfo &&  playerInfo->getCharacter() == character) {
						this->fleeingSound->stop();
					}
					this->hearingTimers.erase(character);
				}
			}

			if (!this->terrorClient->isHost()) {
				return true;
			}

			MonsterPtr monster = this->characterHandler->getMonster();
			if (!monster || monster->getType() != BLIND) {
				return true;
			}

			Vector3d monsterPosition = monster->getActor()->getPosition();
			HumanPtrMap& humans = this->characterHandler->getHumansAlive();
			BlindMonsterHearsPtr blindMonsterHears(new BlindMonsterHears);
			foreach (HumanPtrMap::value_type& entry, humans) {
				HumanPtr human = entry.second;
				if (!human->isDead()) {					
					Vector3d humanToMonster = monsterPosition - human->getActor()->getPosition();
					MovementState state = human->getMovementState();
					float range = 0.0f;
					switch (state) {
						case IDLE:
							range = 1.0f;
							break;
						case WALKING:
							range = 1.5f;
							break;
						case RUNNING:
							{
								if (human->isCrouched()) {
									range = 1.5f;
								}else {
									range = 3.0f;
								}
							}							
							break;
						case SPRINTING:
							range = forest ? 12.0f : 10.0f;
							break;
					}					
					if (human->hasFlashlightSwitched()) {
						range = forest ? std::max(range, 8.0f) : std::max(range, 5.0f);
					}
					if (humanToMonster.norm() <= range) {
						CharacterType character = human->getCharacterType();						
						blindMonsterHears->characters.push_back(character);
						this->doBlindMonsterHears(character);
					}
				}
			}

			if (!blindMonsterHears->characters.empty()) {
				this->terrorClient->sendObjectToPlayers(blindMonsterHears);
			}
			return true;
		}

		void doBlindMonsterHears(CharacterType character) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			CharacterType thisChar = playerInfo->getCharacter();
			if (thisChar == CHAR_MONSTER_1) {
				this->hearingTimers[character] = forest ? FloatPtr(new float(6.0f)) : FloatPtr(new float(5.0f));
				HumanPtr human = this->characterHandler->getHumanAlive(character);
				if (human && !human->isVisible()) {
					human->show();
					human->getActor()->setMaterial("through_walls_mat");
					human->getActor()->setMaterialId(20);
				}
				this->hearingSound->play();
				this->notHearingSound->stop();
				int maxNumberOfChars = 4;
				if (this->hearingTimers.size() == maxNumberOfChars) {
					SteamSystemPtr steamSystem = SteamSystem::getInstance();
					if (!steamSystem->hasAchievement("i_see_you")) {
						steamSystem->setAchievement("i_see_you");
					}
				}
			}else if (thisChar == character) {
				this->hearingTimers[character] = forest ? FloatPtr(new float(6.0f)) : FloatPtr(new float(5.0f));
				this->fleeingSound->play();
			}
		}
	};

	typedef SharedPointer<BlindMonsterHearingSystem> BlindMonsterHearingSystemPtr;
}

#endif