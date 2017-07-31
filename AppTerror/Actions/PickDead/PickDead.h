#ifndef TERROR_PICK_DEAD_H
#define TERROR_PICK_DEAD_H

#include "Config.h"

namespace terror {
	class PickDead : public Action {
	public:
		PickDead(CharacterDataPtr characterData, CollisionTesterPtr collisionTester, RenderCameraPtr camera) 
			: Action(characterData, ""), collisionTester(collisionTester), camera(camera), dead(0), reach(1.5f) {}

		virtual void execute() {
			if (this->dead) {
				if (this->dead->getMask() == -1) {
					this->dead->setMaterialId(10);
				}
			}

			if (this->characterData->dead) {
				this->dead = 0;
				return;
			}

			ClosestNotMeRayResultCallback callback(this->characterData->actor->getCollisionObject().get());
			Vector3d from = toAlgebra(this->camera->getPosition());
			Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
			Vector3d to = from + (this->reach/cameraDirection.horizontalNorm())*cameraDirection;
			if (!this->collisionTester->rayTest(toBullet(from), toBullet(to), callback)) {
				this->dead = 0;
				return;
			}

			this->dead = static_cast<Actor*>(callback.m_collisionObject->getUserPointer());
			if (!this->dead || this->dead->getType() != ACTOR || this->dead->getMask() != -1) {
				this->dead = 0;
				return;
			}

			this->dead->setMaterialId(11);
		}

		int getDeadId() {
			if (!this->dead) {
				return -1;
			}
			return this->dead->getId();
		}

		virtual ~PickDead() {}

	protected:
		EntitySystemPtr entitySystem;
		CollisionTesterPtr collisionTester;
		RenderCameraPtr camera;
		Actor* dead;
		float reach;
	};

	typedef SharedPointer<PickDead> PickDeadPtr;
}

#endif
