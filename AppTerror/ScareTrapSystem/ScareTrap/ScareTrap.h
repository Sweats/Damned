#ifndef TERROR_SCARE_TRAP_H
#define TERROR_SCARE_TRAP_H

#include "Human/Human.h"

namespace terror {
	class ScareTrap {
	public:
		ScareTrap(CollisionTesterPtr collisionTester, ObjectPtr object, float range) 
			: collisionTester(collisionTester), object(object), range(range), armed(false) {}

		SoundPtr getArmSoundEffect() {
			return this->object->getUnlockSoundEffect();
		}

		Vector3d getPosition() {
			return this->object->getPosition();
		}

		bool isSurvivorInRange(HumanPtr human) {
			if (!this->object) {
				return false;
			}
			Vector3d trapToHuman = this->object->getPosition() - human->getActor()->getPosition();
			if (trapToHuman.horizontalNorm() > this->range) {
				return false;
			}
			ClosestRayResultCallback callback;
			callback.addIgnoreObject(this->object->getCollisionObject().get());
			callback.addIgnoreObject(human->getActor()->getCollisionObject().get());
			if (this->collisionTester->rayTest(toBullet(this->object->getPosition()), toBullet(human->getActor()->getPosition()), callback)) {
				return false;
			}
			return true;
		}

		void trigger() {
			this->armed = false;

			AnimationPtrList& animations = this->object->getAnimations();
			if (!animations.empty()) {
				int chosenAnimationId = randomNumber(0, animations.size() - 1);
				int animationId = 0;
				AnimationPtr chosenAnimation;
				foreach (AnimationPtr animation, animations) {
					if (animationId == chosenAnimationId) {
						chosenAnimation = animation;
					}
					++animationId;
				}
				if (chosenAnimation) {
					this->object->queueAnimation(chosenAnimation);
					return;
				}
			}

			SoundPtr soundEffect = this->object->getCollisionSoundEffect();
			if (!soundEffect) {
				return;
			}
			soundEffect->play();
		}

		void arm() {
			this->armed = true;
		}

		void disarm() {
			this->armed = false;
		}

		bool isArmed() {
			return this->armed;
		}

		void enable() {
			this->object->setMask(500);
			this->object->setMaterialId(20);
		}

		void disable() {
			this->object->setMask(0);
			this->object->setMaterialId(0);
		}

		bool isEnabled() {
			return this->object->getMask() == 500;
		}

		int getId() {
			return this->object->getId();
		}

		void enableSeeThroughMaterial() {
			this->object->setMaterial("highlight_mat");
		}

		void restoreMaterial() {
			if (this->object) {
				this->object->restoreMaterials();
			}
		}

		virtual ~ScareTrap() {}

	protected:
		CollisionTesterPtr collisionTester;
		ObjectPtr object;
		float range;
		bool armed;
	};

	typedef SharedPointer<ScareTrap> ScareTrapPtr;
}

#endif