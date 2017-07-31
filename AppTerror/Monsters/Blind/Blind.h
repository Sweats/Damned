#ifndef TERROR_BLIND_H
#define TERROR_BLIND_H

#include "Monsters/Monster.h"

namespace terror {
	class Blind : public Monster {
	public:
		Blind(CharacterDataPtr characterData, CharacterActionsPtr characterActions, MonsterActionsPtr monsterActions) 
			: Monster(characterData, characterActions, monsterActions)
		{
			this->data->actor->enableGhostMode();
			this->actions->walk->setAnimationSpeed(3.5f);
			this->actions->run->setAnimationSpeed(3.5f);
			this->monsterActions->attack->setAnimationSpeedFactor(2.0f);
		}

		virtual MonsterType getType() {
			return BLIND;
		}

		virtual ~Blind() {}
	};

	typedef SharedPointer<Blind> BlindPtr;
}

#endif