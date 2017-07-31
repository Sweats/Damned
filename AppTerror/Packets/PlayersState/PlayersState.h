#ifndef TERROR_PLAYERS_STATE_H
#define TERROR_PLAYERS_STATE_H

#include "Packets/PacketsEnum.h"
#include "Packets/GameSettings/GameSettings.h"
#include "Packets/PlayerInfo/PlayerInfo.h"

namespace terror {
	typedef std::vector<PlayerInfoPtr> PlayerInfoVector;
	typedef std::vector<CharacterType> CharacterTypeVector;

	class PlayersState : public Sendable {
	public:
		friend class boost::serialization::access;
		typedef SharedPointer<PlayersState> PlayersStatePtr;

		static PlayersStatePtr getInstance() {
			if (!PlayersState::instance) {
				PlayersState::instance = PlayersStatePtr(new PlayersState);
			}
			return PlayersState::instance;
		}
	
		virtual int getLabel() const {
			return PLAYERS_STATE;
		}

		void resolveRandomCharacters() {
			std::vector<CharacterType> charactersLeft(4);
			charactersLeft[0] = CHAR_0;
			charactersLeft[1] = CHAR_1;
			charactersLeft[2] = CHAR_2;
			charactersLeft[3] = CHAR_3;

			std::vector<int> randomPlayerIds;
			for (int i = 0; i < this->players.size(); ++i) {
				PlayerInfoPtr playerInfo = this->players[i];
				if (playerInfo->getCharacter() == CHAR_RANDOM) {
					randomPlayerIds.push_back(i);
				}else if (playerInfo->getCharacter() != CHAR_NONE) {
					for (int i = 0; i < charactersLeft.size(); ++i) {
						if (charactersLeft[i] == playerInfo->getCharacter()) {
							charactersLeft.erase(charactersLeft.begin() + i);
							break;
						}
					}
				}
			}

			std::vector<CharacterType> allowedMonsters;
			GameSettingsPtr gameSettings = GameSettings::getInstance();
			if (gameSettings->getMonsters()[SPECTRUM]) {
				allowedMonsters.push_back(CHAR_MONSTER_0);
			}
			if (gameSettings->getMonsters()[BLIND]) {
				allowedMonsters.push_back(CHAR_MONSTER_1);
			}
			if (gameSettings->getMonsters()[BERSERKER]) {
				allowedMonsters.push_back(CHAR_MONSTER_2);
			}
			if (gameSettings->getMonsters()[STATUE]) {
				allowedMonsters.push_back(CHAR_MONSTER_3);
			}

			if (allowedMonsters.empty()) {
				throw std::runtime_error("At least one monster should be allowed to be played with. Error while solving random characters.");
			}

			if (!this->monsterPlayerExists()) {
				this->monsterCharacter = allowedMonsters[randomNumber(0, allowedMonsters.size() - 1)];
				if (!randomPlayerIds.empty()) {					
					int monsterPlayer = randomNumber(0, randomPlayerIds.size() - 1);
					this->players[randomPlayerIds[monsterPlayer]]->setCharacter(this->monsterCharacter);					
				}
			}else {				
				this->monsterCharacter = this->getMonsterPlayerInfo()->getCharacter();
			}

			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (playerInfo->getCharacter() == CHAR_RANDOM && !charactersLeft.empty()) {
					int characterId = randomNumber(0, charactersLeft.size() - 1);
					CharacterType character = charactersLeft[characterId];					
					playerInfo->setCharacter(character);
					charactersLeft.erase(charactersLeft.begin() + characterId);
				}
			}

			/*foreach (CharacterType character, charactersLeft) {
				this->deadCharacters.push_back(character);
			}*/
		}		

		void resolveRandomCharacter(PlayerInfoPtr randomPlayerInfo) {
			if (!this->monsterPlayerExists()) {
				randomPlayerInfo->setCharacter(this->monsterCharacter);
				return;
			}

			std::vector<CharacterType> charactersLeft(4);
			charactersLeft[0] = CHAR_0;
			charactersLeft[1] = CHAR_1;
			charactersLeft[2] = CHAR_2;
			charactersLeft[3] = CHAR_3;

			foreach (CharacterType& deadCharacter, this->deadCharacters) {
				for (int i = 0; i < charactersLeft.size(); ++i) {
					if (charactersLeft[i] == deadCharacter) {
						charactersLeft.erase(charactersLeft.begin() + i);
						break;
					}
				}
			}

			for (int i = 0; i < this->players.size(); ++i) {
				PlayerInfoPtr playerInfo = this->players[i];
				if (playerInfo->getCharacter() != CHAR_NONE) {
					for (int i = 0; i < charactersLeft.size(); ++i) {
						if (charactersLeft[i] == playerInfo->getCharacter()) {
							charactersLeft.erase(charactersLeft.begin() + i);
						}
					}
				}
			}			

			if (!charactersLeft.empty()) {
				int characterId = randomNumber(0, charactersLeft.size() - 1);
				CharacterType character = charactersLeft[characterId];					
				randomPlayerInfo->setCharacter(character);
			}
		}

		void resolveSpawnPoints() {
			std::queue<int> spawnPointsLeft;
			spawnPointsLeft.push(0);
			spawnPointsLeft.push(1);
			spawnPointsLeft.push(2);
			spawnPointsLeft.push(3);

			int monsterSpawnPoint = 4;
			for (int i = 0; i < this->players.size(); ++i) {
				PlayerInfoPtr playerInfo = this->players[i];
	
				CharacterType character = playerInfo->getCharacter();
				if (isMonsterCharacter(character)) {
					playerInfo->setSpawnPointId(monsterSpawnPoint);
				}else if (character != CHAR_RANDOM && character != CHAR_NONE) {
					playerInfo->setSpawnPointId(spawnPointsLeft.front());
					spawnPointsLeft.pop();
				}
			}
		}

		void resolveSpawnPoint(PlayerInfoPtr newPlayerInfo) {
			if (isMonsterCharacter(newPlayerInfo->getCharacter())) {
				int monsterSpawnPoint = 4;
				newPlayerInfo->setSpawnPointId(monsterSpawnPoint);
				return;
			}

			std::vector<int> spawnPointsLeft(4);
			spawnPointsLeft[0] = 0;
			spawnPointsLeft[1] = 1;
			spawnPointsLeft[2] = 2;
			spawnPointsLeft[3] = 3;

			for (int i = 0; i < this->players.size(); ++i) {
				PlayerInfoPtr playerInfo = this->players[i];
	
				CharacterType character = playerInfo->getCharacter();
				if (character != CHAR_RANDOM && character != CHAR_NONE && !isMonsterCharacter(character)) {
					for (int i = 0; i < spawnPointsLeft.size(); ++i) {
						if (spawnPointsLeft[i] == playerInfo->getSpawnPointId()) {
							spawnPointsLeft.erase(spawnPointsLeft.begin() + i);
							break;
						}
					}
				}
			}
			newPlayerInfo->setSpawnPointId(spawnPointsLeft.front());
		}

		bool monsterPlayerExists() {
			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (isMonsterCharacter(playerInfo->getCharacter())) {
					return true;
				}
			}
			return false;
		}

		int getMonsterPlayerId() {
			for (int i = 0; i < this->players.size(); ++i) {
				PlayerInfoPtr playerInfo = this->players[i];
				if (isMonsterCharacter(playerInfo->getCharacter())) {
					return i;
				}
			}

			string message = "Monster player not found! Status:\n";
			foreach (PlayerInfoPtr playerInfo, this->players) {
				message += "Player " + playerInfo->getName() + ", Char = " + lexical_cast<std::string>(int(playerInfo->getCharacter())) + "\n";
			}
			throw std::runtime_error(message);
		}

		PlayerInfoPtr getMonsterPlayerInfo() {
			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (isMonsterCharacter(playerInfo->getCharacter())) {
					return playerInfo;
				}
			}

			string message = "Monster player not found! Status:\n";
			foreach (PlayerInfoPtr playerInfo, this->players) {
				message += "Player " + playerInfo->getName() + ", Char = " + lexical_cast<std::string>(int(playerInfo->getCharacter())) + "\n";
			}
			throw std::runtime_error(message);
		}

		CharacterType getMonsterCharacter() {
			return this->monsterCharacter;
		}

		void reset() {			
			foreach (PlayerInfoPtr playerInfo, this->players) {
				playerInfo->setName("");
				playerInfo->setCharacter(CHAR_NONE);		
				playerInfo->setSpawnPointId(0);	
				playerInfo->resetUdpAddress();
				playerInfo->updateGuiElements();
			}
			this->deadCharacters.clear();
			this->monsterCharacter = CHAR_NONE;
		}

		void addPlayerInfo(PlayerInfoPtr playerInfo) {
			this->players.push_back(playerInfo);
		}

		PlayerInfoPtr getPlayerInfo(int id) {
			if (id < 0 || id >= this->players.size()) {
				return PlayerInfoPtr();
			}
			return this->players[id];
		}

		bool existsCharacterPlayer(CharacterType character) {
			bool isMonster = isMonsterCharacter(character);
			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (playerInfo->getCharacter() == character || isMonster && isMonsterCharacter(playerInfo->getCharacter())) {
					return true;
				}
			}
			return false;
		}

		int getNumberOfHumanPlayers() {
			int nHumanPlayers = 0;
			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (isHumanCharacter(playerInfo->getCharacter())) {
					nHumanPlayers++;
				}
			}
			return nHumanPlayers;
		}

		PlayerInfoPtr getPlayerInfoByCharacter(CharacterType character) {
			foreach (PlayerInfoPtr playerInfo, this->players) {
				if (playerInfo->getCharacter() == character) {
					return playerInfo;
				}
			}

 			string message = "Player with character code " + lexical_cast<std::string>(int(character)) + " not found! Status:\n";
			foreach (PlayerInfoPtr playerInfo, this->players) {
				message += "Player " + playerInfo->getName() + ", Char = " + lexical_cast<std::string>(int(playerInfo->getCharacter())) + "\n";
			}
			throw std::runtime_error(message);
		}

		void addDeadCharacter(CharacterType character) {
			this->deadCharacters.push_back(character);
		}

		bool isCharacterDead(CharacterType character) {
			return std::binary_search(this->deadCharacters.begin(), this->deadCharacters.end(), character);
		}

		void resetDeadCharacters() {
			this->deadCharacters.clear();
		}

		CharacterTypeVector& getDeadCharacters() {
			return this->deadCharacters;
		}

		void setNumberOfLives(int numberOfLives) {
			if (numberOfLives >= 0) {
				this->numberOfLives = numberOfLives;
			}
		}

		int getNumberOfLives() {
			return this->numberOfLives;
		}

	protected:
		PlayerInfoVector players;
		CharacterTypeVector deadCharacters;
		CharacterType monsterCharacter;
		int numberOfLives;

	private:
		static PlayersStatePtr instance;

		PlayersState() : monsterCharacter(CHAR_NONE), numberOfLives(-1) {}

		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			PlayerInfoVector tmpPlayers = this->players;
			archive & tmpPlayers;
			archive & this->deadCharacters;
			archive & this->monsterCharacter;
			archive & this->numberOfLives;

			for (int i = 0; i < 5; i++) {
				this->players[i]->setName(tmpPlayers[i]->getName());
				this->players[i]->setCharacter(tmpPlayers[i]->getCharacter());
				this->players[i]->setSpawnPointId(tmpPlayers[i]->getSpawnPointId());
				this->players[i]->setUdpAddress(tmpPlayers[i]->getUdpAddress());
				this->players[i]->updateGuiElements();
			}
		}
	};
	typedef PlayersState::PlayersStatePtr PlayersStatePtr;

	PlayersStatePtr PlayersState::instance = PlayersStatePtr();
}

#endif