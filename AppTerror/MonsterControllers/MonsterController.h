#ifndef TERROR_MONSTER_CONTROLLER_H
#define TERROR_MONSTER_CONTROLLER_H

#include "Monsters/Spectrum/Spectrum.h"
#include "Monsters/Blind/Blind.h"
#include "Monsters/Berserker/Berserker.h"
#include "Monsters/Statue/Statue.h"

namespace terror {
	enum MonsterControllerType {
		LOCAL_MONSTER_CONTROLLER,
		AI_MONSTER_CONTROLLER,
		REMOTE_MONSTER_CONTROLLER
	};

	class MonsterController {
	public:
		MonsterController(MonsterPtr monster) : monster(monster), manipulateConstraint(0) {}

		virtual MonsterControllerType getType() = 0;
		virtual void destroy() = 0;
		virtual void update(MonsterInfoPtr monsterInfo) {}

		MonsterType getMonsterType() {
			return this->monster->getType();
		}

		void update(MonsterActionRequestPtr monsterActionRequest) {
			this->monster->updateInfo(monsterActionRequest);
			this->update(this->monster->getInfo());
		}	

		MonsterPtr getMonster() {
			return this->monster;
		}

		void setManipulateConstraint(btPoint2PointConstraint* manipulateConstraint) {
			this->manipulateConstraint = manipulateConstraint;
		}

		btPoint2PointConstraint* getManipulateConstraint() {
			return this->manipulateConstraint;
		}

		virtual ~MonsterController() {}

	protected:
		MonsterPtr monster;
		btPoint2PointConstraint* manipulateConstraint;
	};

	typedef SharedPointer<MonsterController> MonsterControllerPtr;
}

#endif