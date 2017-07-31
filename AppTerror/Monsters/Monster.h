#ifndef TERROR_MONSTER_H
#define TERROR_MONSTER_H

#include "Actions/Manipulate/Manipulate.h"
#include "Actions/Attack/Attack.h"
#include "Actions/PickItem/PickItem.h"

namespace terror {
	enum MonsterType {
		SPECTRUM,
		BLIND,
		BERSERKER,
		STATUE,
		N_MONSTERS = 4
	};

	std::string toMonsterTypeText(int monsterType) {
		switch (monsterType) {
			case SPECTRUM:
				return "Lurker";
			case BLIND:
				return "Phantom";
			case BERSERKER:
				return "Mary";
			case STATUE:
				return "Fallen";
			default:
				return "Unknown";
		}
	}

	MonsterType toMonsterType(const std::string& monsterTypeText) {
		if (monsterTypeText == "Lurker") {
			return SPECTRUM;
		}
		if (monsterTypeText == "Phantom") {
			return BLIND;
		}
		if (monsterTypeText == "Mary") {
			return BERSERKER;
		}
		if (monsterTypeText == "Fallen") {
			return STATUE;
		}
		throw runtime_error("Unknown Monster Type");
	}

	struct MonsterAnimationNames : public CharacterAnimationNames {
		string attack;
		string monsterIdle;
	};

	typedef SharedPointer<MonsterAnimationNames> MonsterAnimationNamesPtr;

	struct MonsterActions {
		ManipulatePtr manipulate;
		PickItemPtr pickItem;
		AttackPtr attack;
		IdlePtr idle;
	};

	typedef SharedPointer<MonsterActions> MonsterActionsPtr;

	class Monster : public Character {
	public:
		Monster(CharacterDataPtr characterData, CharacterActionsPtr characterActions, MonsterActionsPtr monsterActions)
			: Character(characterData, characterActions), monsterActions(monsterActions), monsterInfo(new MonsterInfo) {}

		virtual MonsterType getType() = 0;

		void localUpdate(float diffTime) {
			this->monsterActions->attack->update(diffTime);
			this->monsterActions->pickItem->execute();
			if (!this->monsterActions->manipulate->isManipulating()) {	
				this->monsterActions->manipulate->execute();
			}
		}

		void updateManipulation(float diffTime, int xMouseRelative) {
			this->monsterActions->manipulate->update(diffTime, xMouseRelative);
		}

		bool tryAttacking() {
			this->monsterActions->attack->execute();
			return this->monsterActions->attack->getActorId() != -1;
		}

		void queueAttackAnimation() {
			this->monsterActions->attack->queueAnimation();
		}

		int getAttackActorId() {
			return this->monsterActions->attack->getActorId();
		}

		bool tryGrabbingObject() {
			this->monsterActions->manipulate->grabObject();
			return this->monsterActions->manipulate->isManipulating();
		}

		void releaseObject() {
			this->monsterActions->manipulate->releaseObject();
		}

		bool tryPickingItem() {			
			return this->monsterActions->pickItem->getObjectId() != -1;
		}

		int getItemId() {
			return this->monsterActions->pickItem->getObjectId();
		}

		virtual void updateInfo(MonsterInfoPtr monsterInfo) {
			this->monsterInfo = monsterInfo;
			this->updateStateInfo(monsterInfo->stateInfo);
			this->updateAdditionalInfo(monsterInfo->stateInfo);
		}

		virtual void updateInfo(MonsterActionRequestPtr monsterActionRequest) {
			this->monsterInfo->update(monsterActionRequest);
		}

		virtual MonsterInfoPtr getInfo() {
			return this->monsterInfo;
		}

		void monsterIdle() {
			if (this->monsterActions->idle) {
				this->monsterActions->idle->execute();
				return;
			}
			this->idle();
		}

		void destroy() {
			this->getActor()->destroy();
		}

		virtual ~Monster() {}

	protected:
		MonsterActionsPtr monsterActions;
		MonsterInfoPtr monsterInfo;

	private:
		virtual void updateAdditionalInfo(CharacterStateInfoPtr stateInfo) {}

		void updateStateInfo(CharacterStateInfoPtr stateInfo) {
			Vector3d oldPosition = this->getActor()->getPosition();
			float distance = (stateInfo->getPosition() - oldPosition).norm();
			if (distance > 10.0f) {
				this->getActor()->setPosition(stateInfo->getPosition());
				this->getActor()->setOrientation(stateInfo->getOrientation());
			}else{
				this->getActor()->setPosition(stateInfo->getPosition(), 0.1f);
				this->getActor()->setOrientation(stateInfo->getOrientation(), 0.1f);
			}
			if (stateInfo->isCrouched() && !this->isCrouched()) {
				this->crouch();
			}
			if (!stateInfo->isCrouched() && this->isCrouched()) {
				this->stand();
			}
			if (stateInfo->isIdle()) {
				if (this->getType() == BERSERKER && stateInfo->isVisible()) {
					this->monsterIdle();
				}else {
					this->idle();
				}
				return;
			}
			if (stateInfo->isRunning()) {
				this->run(stateInfo->getLeftMove(), stateInfo->getFrontMove());
				return;
			}
			this->walk(stateInfo->getLeftMove(), stateInfo->getFrontMove());
		}
	};

	typedef SharedPointer<Monster> MonsterPtr;
}

#endif