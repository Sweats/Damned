#ifndef TERROR_CHARACTER_HANDLER_H
#define TERROR_CHARACTER_HANDLER_H

#include "Human/Human.h"
#include "Monsters/Spectrum/Spectrum.h"
#include "Monsters/Blind/Blind.h"

namespace terror {	
	typedef PointerMap<int, Human> HumanPtrMap;

	class CharacterHandler {
	public:
		CharacterHandler() {}

		void addHumanAlive(HumanPtr human) {
			if (!human->isDead()) {
				this->humans[human->getCharacterType()] = human;
			}
		}

		HumanPtr getHumanAlive(int characterType) {
			HumanPtrMap::const_iterator entry = this->humans.find(characterType);
			if (entry == this->humans.end()) {
				return HumanPtr();
			}
			return entry->second;
		}

		HumanPtrMap& getHumansAlive() {
			return this->humans;
		}

		void addDeadHuman(HumanPtr human, int id = -1) {
			if (!human->isDead()) {
				return;
			}
			if (id == -1) {
				id = 50 + this->deadHumans.size();
			}
			human->getActor()->setId(id);
			this->deadHumans[id] = human;
		}

		HumanPtr getDeadHuman(int deadHumanId) {
			HumanPtrMap::const_iterator entry = this->deadHumans.find(deadHumanId);
			if (entry == this->deadHumans.end()) {
				return HumanPtr();
			}
			return entry->second;
		}	

		void notifyHumanDeath(HumanPtr human) {
			this->removeHumanAlive(human);
			this->addDeadHuman(human);
		}

		HumanPtrMap& getDeadHumans() {
			return this->deadHumans;
		}

		void setMonster(MonsterPtr monster) {
			this->monster = monster;
		}

		MonsterPtr getMonster() {
			return this->monster;
		}

		CharacterPtr getCharacter(int characterType) {
			if (isMonsterCharacter(CharacterType(characterType))) {
				return this->monster;
			}
			return this->getHumanAlive(characterType);
		}

		void removeHumanAlive(HumanPtr human) {	
			this->humans.erase(human->getCharacterType());
		}

		void resetMonster() {
			this->monster.reset();
		}		

		void resetDeadHumans() {
			this->deadHumans.clear();
		}

		virtual ~CharacterHandler() {}

	protected:
		MonsterPtr monster;
		HumanPtrMap humans;
		HumanPtrMap deadHumans;
	};

	typedef SharedPointer<CharacterHandler> CharacterHandlerPtr;
}

#endif