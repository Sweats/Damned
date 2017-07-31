#ifndef TERROR_HUMAN_CONTROLLER_H
#define TERROR_HUMAN_CONTROLLER_H

#include "Human/Human.h"

namespace terror {
	enum HumanControllerType {
		LOCAL_HUMAN_CONTROLLER,
		REMOTE_HUMAN_CONTROLLER
	};

	class HumanController {
	public:
		HumanController(HumanPtr human) : human(human) {}

		virtual HumanControllerType getType() = 0;
		virtual void update(HumanInfoPtr humanInfo) = 0;
		virtual void destroy(bool destroyHuman = true) = 0;	

		void updateHumanAndController(HumanInfoPtr humanInfo) {
			this->human->updateInfo(humanInfo);
			this->update(humanInfo);
		}

		HumanPtr getHuman() {
			return this->human;
		}		

		virtual ~HumanController() {}

	protected:		
		HumanPtr human;
	};

	typedef SharedPointer<HumanController> HumanControllerPtr;
}

#endif