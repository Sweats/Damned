#ifndef TERROR_DEATH_SYSTEM_H
#define TERROR_DEATH_SYSTEM_H

#include "Packets/Packets.h"
#include "CharacterHandler/CharacterHandler.h"
#include "MessageSystem/MessageSystem.h"
#include "GameModeSystem/GameModeSystem.h"

namespace terror {
	class DeathSystem : public NetworkObserver {
	public:
		DeathSystem(CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient, GameModeSystemPtr gameModeSystem) 
			: NetworkObserver(false), characterHandler(characterHandler), terrorClient(terrorClient), gameModeSystem(gameModeSystem) {}

		void notifyStageLoaded(StagePtr stage) {
			this->stage = stage;
		}

		void notifyStageUnloaded() {
			this->stage.reset();
		}

		void requestKillCharacter(CharacterType character) {
			if (!this->stage) {
				return;
			}

			if (!this->terrorClient->isHost()) {
				KillCharacterRequestPtr killCharacterRequest(new KillCharacterRequest(character));
				this->terrorClient->sendObjectToHost(killCharacterRequest, true);
				return;
			}
			int numberOfLives = static_ptr_cast<LifeMode>(this->gameModeSystem->getGameModes()[LIFE_MODE])->getNumberOfLives();
			KillCharacterPtr killCharacter(new KillCharacter(character, numberOfLives));
			this->terrorClient->sendObjectToPlayers(killCharacter, true);
			this->doKillCharacter(character);
		}

		void doKillCharacter(CharacterType character, bool playerDisconnected = false) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			
			if (localPlayerInfo && !isMonsterCharacter(localPlayerInfo->getCharacter())) {
				MonsterPtr monster = this->characterHandler->getMonster();
				if (monster) {
					monster->queueAttackAnimation();
				}
			}

			HumanPtr human = this->characterHandler->getHumanAlive(character);
			if (!human || human->isDead()) {
				return;
			}

			human->die();
			if (localPlayerInfo && !playerDisconnected) {
				CharacterType localCharacter = localPlayerInfo->getCharacter();
				if (isMonsterCharacter(localCharacter)) {
					this->setSteamStatsAndAchievements(character);
				}
				this->writeDeadMessage(character);
			}
			this->humanDeathSignal(human);

			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getGameModeType() == NORMAL_MODE) {
				PlayersStatePtr playersState = PlayersState::getInstance();
				playersState->addDeadCharacter(character);
			}

			this->gameModeSystem->handleHumanDeath(human, playerDisconnected);
		}

		Connection connectHumanDeathEvent(const HumanSignal::slot_type& humanDeathEvent) {
			return this->humanDeathSignal.connect(humanDeathEvent);
		}

		virtual ~DeathSystem() {}

	protected:
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		StagePtr stage;
		HumanSignal humanDeathSignal;
		GameModeSystemPtr gameModeSystem;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (!this->stage) {
				return;
			}

			// Host
			if (udpPacket->getLabel() == KILL_CHARACTER_REQUEST) {
				KillCharacterRequestPtr killCharacterRequest(new KillCharacterRequest);
				if (!udpPacket->buildObject(killCharacterRequest)) {
					return;
				}
				int numberOfLives = static_ptr_cast<LifeMode>(this->gameModeSystem->getGameModes()[LIFE_MODE])->getNumberOfLives();
				KillCharacterPtr killCharacter(new KillCharacter(killCharacterRequest->getCharacter(), numberOfLives));
				this->terrorClient->sendObjectToPlayers(killCharacter, true);
				this->doKillCharacter(killCharacterRequest->getCharacter());
			}
			if (udpPacket->getLabel() == PLAYER_JOINED_GAME && this->terrorClient->isHost()) {
				int numberOfLives = static_ptr_cast<LifeMode>(this->gameModeSystem->getGameModes()[LIFE_MODE])->getNumberOfLives();
				DeadCharactersPtr deadCharacters(new DeadCharacters(numberOfLives));
				HumanPtrMap& deadHumans = this->characterHandler->getDeadHumans();
				foreach (HumanPtrMap::value_type& humanEntry, deadHumans) {
					HumanPtr deadHuman = humanEntry.second;
					HumanInfoPtr humanInfo = deadHuman->getInfo();

					DeadHumanDataPtr deadHumanData(new DeadHumanData);					
					deadHumanData->setCharacterType(humanInfo->character);
					deadHumanData->setTransform(humanInfo->stateInfo->getPosition(), humanInfo->stateInfo->getOrientation());

					CharacterItemsPtr characterItems(new CharacterItems(humanInfo->character));
					ItemPtrMap& items = deadHuman->getInventory()->getItems();
					foreach (ItemPtrMap::value_type& itemEntry, items) {
						ItemPtr item = itemEntry.second;
						characterItems->addItemMask(item->getMask());
					}
					deadHumanData->setCharacterItems(characterItems);
					deadHumanData->setId(humanEntry.first);
					deadCharacters->addDeadCharacterData(deadHumanData);
				}
				this->terrorClient->sendObjectTo(deadCharacters, udpPacket->getRemoteUdpAddress(), true);
			}

			// Client
			if (udpPacket->getLabel() == KILL_CHARACTER) {				
				KillCharacterPtr killCharacter(new KillCharacter);
				if (!udpPacket->buildObject(killCharacter)) {
					return;
				}
				static_ptr_cast<LifeMode>(this->gameModeSystem->getGameModes()[LIFE_MODE])->setNumberOfLives(killCharacter->getNumberOfLives());
				this->doKillCharacter(killCharacter->getCharacter());
			}
			if (udpPacket->getLabel() == DEAD_CHARACTERS) {
				DeadCharactersPtr deadCharacters(new DeadCharacters);
				if (!udpPacket->buildObject(deadCharacters)) {
					return;
				}
				static_ptr_cast<LifeMode>(this->gameModeSystem->getGameModes()[LIFE_MODE])->setNumberOfLives(deadCharacters->getNumberOfLives());	
			}
		}

		void writeDeadMessage(CharacterType character) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());

			std::string message;
			if (localPlayerInfo && localPlayerInfo->getCharacter() == character) {
				message = std::string("You died!");
			}else {
				try {
					PlayerInfoPtr playerInfo = playersState->getPlayerInfoByCharacter(character);
					message = std::string("Player ") + playerInfo->getName() + std::string(" died!");
				}catch (std::exception&) {}				
			}
			writeMessage(message);
		}

		void setSteamStatsAndAchievements(CharacterType character) {
			SteamSystemPtr steamSystem = SteamSystem::getInstance();
			steamSystem->incrementStat("survivors_killed", 1);
			string characterName = this->getCharacterString(character);
			if (!steamSystem->hasAchievement(characterName)) {
				steamSystem->incrementStat("characters_killed", 1);
				steamSystem->setAchievement(characterName);
			}
		}

		string getCharacterString(CharacterType character) {
			switch (character) {
			case CHAR_0:
				return "Mia";
				break;
			case CHAR_1:
				return "Scott";
				break;
			case CHAR_2:
				return "Jacob";
				break;
			case CHAR_3:
				return "John";
				break;
			case CHAR_MONSTER_0:
				return "Lurker";
				break;
			case CHAR_MONSTER_1:
				return "Phantom";
				break;
			case CHAR_MONSTER_2:
				return "Mary";
				break;
			case CHAR_MONSTER_3:
				return "Fallen";
				break;
			case CHAR_RANDOM:
			case CHAR_NONE:
				return "";
				break;
			}
			return "";
		}
	};

	typedef SharedPointer<DeathSystem> DeathSystemPtr;	
}

#endif