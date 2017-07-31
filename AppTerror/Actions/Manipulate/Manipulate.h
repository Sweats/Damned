#ifndef TERROR_MANIPULATE_H
#define TERROR_MANIPULATE_H

#include "Config.h"

namespace terror {
	class Manipulate : public Action {
	public:
		typedef Signal<void (CharacterType)> CharacterSignal;
		typedef Signal<void (CharacterType, const Vector3d&)> CharacterVectorSignal;
		typedef Signal<void (CharacterType, int, const Vector3d&)> CharacterIntVectorSignal;

		Manipulate(CharacterDataPtr characterData, CharacterType character, PhysicsSystemPtr physicsSystem, RenderCameraPtr camera) 
			: Action(characterData, ""), character(character), physicsSystem(physicsSystem), camera(camera), 
			manipulateSendTime(0.0f), object(0), handPosition(0.0f, 0.0f, 0.0f), handDistance(0.0f), reach(2.0f), holdingObject(false) {}

		void connectGrabEvent(const CharacterIntVectorSignal::slot_type& grabEvent) {
			this->grabSignal.connect(grabEvent);
		}

		void connectManipulateEvent(const CharacterVectorSignal::slot_type& manipulateEvent) {
			this->manipulateSignal.connect(manipulateEvent);
		}

		void connectReleaseEvent(const CharacterSignal::slot_type& releaseEvent) {
			this->releaseSignal.connect(releaseEvent);
		}

		virtual void execute() {
			if (this->object) {
				this->releaseObject();
			}

			if (this->characterData->dead) {
				this->object = 0;
				return;
			}

			ClosestNotMeRayResultCallback callback(this->characterData->actor->getCollisionObject().get());
			Vector3d from = toAlgebra(this->camera->getPosition());
			Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
			Vector3d to = from + (this->reach/cameraDirection.horizontalNorm())*cameraDirection;
			if (!this->physicsSystem->getCollisionTester()->rayTest(toBullet(from), toBullet(to), callback)) {
				this->object = 0;
				return;
			}

			this->object = static_cast<Object*>(callback.m_collisionObject->getUserPointer());
			if (!this->object || this->object->getType() != OBJECT || this->object->getMask() < 1000 || this->object->getMask() >= 2500) {
				this->object = 0;
				return;
			}

			this->handPosition = lerp(from, to, callback.m_closestHitFraction);
			this->handDistance = (this->handPosition - from).norm();
			this->object->setMaterialId(1);
		}

		void releaseObject() {
			this->holdingObject = false;
			if (!this->object) {
				return;
			}
			this->object->setMaterialId(5);
			this->object = 0;
			this->releaseSignal(this->character);
		}

		void grabObject() {
			if (!this->object) {
				return;
			}
			this->holdingObject = true;
			this->grabSignal(this->character, this->object->getId(), this->handPosition);
		}

		void update(float diffTime, int xMouseRelative) {
			if (!this->holdingObject) {
				this->manipulateSendTime = 0.0f;
				return;
			}

			Vector3d relativeCharacterPosition = this->characterData->actor->getPosition() - this->object->getPosition();
			if (relativeCharacterPosition.horizontalNorm() > this->reach) {
				this->manipulateSendTime = 0.0f;
				this->releaseObject();
				return;
			}
			
			this->updateHandDistance(xMouseRelative);
			this->clampHandDistance(diffTime);
			Vector3d from = toAlgebra(this->camera->getPosition());
			Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
			this->handPosition = from + this->handDistance*cameraDirection;		
			this->object->setMaterialId(2);

			this->manipulateSendTime += diffTime;
			if (this->manipulateSendTime < 1.0/30.0f) {
				return;
			}
			this->manipulateSendTime = 0.0f;
			this->manipulateSignal(this->character, this->handPosition);
		}

		bool isManipulating() {
			return this->holdingObject;
		}

		virtual ~Manipulate() {}

	protected:
		CharacterType character;
		EntitySystemPtr entitySystem;
		PhysicsSystemPtr physicsSystem;
		RenderCameraPtr camera;

		CharacterIntVectorSignal grabSignal;
		CharacterVectorSignal manipulateSignal;
		CharacterSignal releaseSignal;
		float manipulateSendTime;

		Object* object;
		bool holdingObject;
		Vector3d handPosition;		
		float handDistance;
		float reach;		

	private:
		void updateHandDistance(int xMouseRelative) {
			if (this->object->getMask() >= 1000 && this->object->getMask() < 1500) {
				Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
				float sign = dot(cameraDirection, this->object->getOrientation().getZAxis());
				if (fabs(sign) < 0.25f) {
					sign = 0.0f;
				}
				this->handDistance += -0.005f*sign*xMouseRelative;
			}
		}

		void clampHandDistance(float diffTime) {
			Vector3d handToActor = this->handPosition - this->characterData->actor->getPosition();
			float horizontalHandDistance = handToActor.horizontalNorm();
			if (horizontalHandDistance < 0.5f) {
				this->handDistance += 0.2f*diffTime;
			}else if (horizontalHandDistance > this->reach) {
				this->handDistance -= 0.2f*diffTime;
			}
		}
	};

	typedef SharedPointer<Manipulate> ManipulatePtr;
}

#endif
