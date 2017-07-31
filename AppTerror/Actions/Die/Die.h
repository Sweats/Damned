#ifndef TERROR_DIE_H
#define TERROR_DIE_H

#include "Config.h"

namespace terror {
	class Die : public Action {
	public:
		Die(CharacterDataPtr characterData, const string& animationName, SoundPtr screaming)
			: Action(characterData, animationName), screaming(screaming), deadShape(new BoxShape(btVector3(0.31f, 0.075f, 0.85f))), 
			meshOffset(0.0f, 0.775f, 0.0f)
		{
			if (this->isAnimated()) {
				this->animation->connectHasEndedEvent(Bind(&Die::changeCollisionShape, this));
			}
		}

		virtual void execute() {
			if (this->characterData->dead) {
				return;
			}
			this->characterData->actor->setMovementDirection(Vector3d(0.0f, 0.0f, 0.0f));
			this->characterData->actor->setMaterialId(10);
			this->characterData->actor->setMask(-1);
			this->characterData->dead = true;
			if (this->screaming) {
				this->screaming->play();
			}
			if (this->isAnimated()) {
				this->queueAnimation();
			}else {
				this->changeCollisionShape();
			}
		}

		void fastForward() {
			if (this->characterData->dead) {
				return;
			}
			this->characterData->actor->setMovementDirection(Vector3d(0.0f, 0.0f, 0.0f));
			this->characterData->actor->setMaterialId(10);
			this->characterData->actor->setMask(-1);			
			this->characterData->dead = true;
			if (this->isAnimated()) {
				this->queueAnimation();
			}else {
				this->changeCollisionShape();
			}
		}

		virtual ~Die() {}

	protected:
		SoundPtr screaming;
		ConvexShapePtr deadShape;
		Vector3d meshOffset;

	private:
		void changeCollisionShape() {
			this->characterData->actor->getMotionState()->setNodeOffset(this->meshOffset);
			this->characterData->actor->changeCollisionShape(this->deadShape);	
			this->characterData->actor->getCollisionObject()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);	
			this->characterData->actor->getController()->disable();
		}
	};

	typedef SharedPointer<Die> DiePtr;
}

#endif
