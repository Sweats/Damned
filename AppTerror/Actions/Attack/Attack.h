#ifndef TERROR_ATTACK_H
#define TERROR_ATTACK_H

#include "Config.h"

namespace terror {
	class Attack : public Action {
	public:
		Attack(CharacterDataPtr characterData, const string& animationName, PhysicsSystemPtr physicsSystem, RenderCameraPtr camera) 
			: Action(characterData, animationName), physicsSystem(physicsSystem), camera(camera),
			attackDetectionShape(new SphereShape(0.2f)), reach(1.4f), actor(0), cooldown(-1.0f) {}

		virtual void execute() {
			if (this->cooldown > 0.0f) {
				return;
			}

			this->queueAnimation();
			this->cooldown = 0.75f;

			ClosestNotMeConvexResultCallback callback(this->characterData->actor->getCollisionObject().get());
			Vector3d from = toAlgebra(this->camera->getPosition());
			Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
			Vector3d to = from + (this->reach/cameraDirection.horizontalNorm())*cameraDirection;
			if (!this->physicsSystem->getCollisionTester()->convexSweepTest(this->attackDetectionShape, toBulletTransform(from), toBulletTransform(to), callback)) {
				this->actor = 0;
				return;
			}

			this->actor = static_cast<Actor*>(callback.m_hitCollisionObject->getUserPointer());
			if (!this->actor || this->actor->getType() != ACTOR || this->actor->getMask() == -1) {
				this->actor = 0;
			}
		}

		void update(float diffTime) {
			this->cooldown -= diffTime;
			if (this->cooldown <= 0.0f) {
				this->cooldown = -1.0f;
			}			
		}

		int getActorId() {
			if (!this->actor) {
				return -1;
			}
			return this->actor->getId();
		}

		void setAnimationSpeedFactor(float speedFactor) {
			this->animation->setSpeedFactor(speedFactor);
		}

		void reset() {
			this->actor = 0;
		}

		virtual ~Attack() {}

	protected:
		EntitySystemPtr entitySystem;
		PhysicsSystemPtr physicsSystem;
		RenderCameraPtr camera;
		ConvexShapePtr attackDetectionShape;
		
		Actor* actor;
		float reach;
		float cooldown;
	};

	typedef SharedPointer<Attack> AttackPtr;
}

#endif
