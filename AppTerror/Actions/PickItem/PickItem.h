#ifndef TERROR_PICK_ITEM_H
#define TERROR_PICK_ITEM_H

#include "Config.h"

namespace terror {
	class PickItem : public Action {
	public:
		PickItem(CharacterDataPtr characterData, CollisionTesterPtr collisionTester, RenderCameraPtr camera, int minMask, int maxMask, int defaultMaterialId, int pickedMaterialId) 
			: Action(characterData, ""), collisionTester(collisionTester), camera(camera), minMask(minMask), maxMask(maxMask), defaultMaterialId(defaultMaterialId), pickedMaterialId(pickedMaterialId), item(0), reach(1.5f) {}

		virtual void execute() {
			if (this->item) {
				if (this->item->getMask() >= this->minMask && this->item->getMask() <= this->maxMask) {
					this->item->setMaterialId(this->defaultMaterialId);
				}
			}

			if (this->characterData->dead) {
				this->item = 0;
				return;
			}

			ClosestNotMeRayResultCallback callback(this->characterData->actor->getCollisionObject().get());
			Vector3d from = toAlgebra(this->camera->getPosition());
			Vector3d cameraDirection = toAlgebra(this->camera->getDirection());
			Vector3d to = from + (this->reach/cameraDirection.horizontalNorm())*cameraDirection;
			if (!this->collisionTester->rayTest(toBullet(from), toBullet(to), callback)) {
				this->item = 0;
				return;
			}

			this->item = static_cast<Object*>(callback.m_collisionObject->getUserPointer());
			if (!this->item || this->item->getType() != OBJECT || this->item->getMask() < this->minMask || this->item->getMask() > this->maxMask) {
				this->item = 0;
				return;
			}

			this->item->setMaterialId(this->pickedMaterialId);
		}

		int getObjectId() {
			if (!this->item) {
				return -1;
			}
			return this->item->getId();
		}

		virtual ~PickItem() {}

	protected:
		EntitySystemPtr entitySystem;
		CollisionTesterPtr collisionTester;
		RenderCameraPtr camera;
		Object* item;
		int minMask;
		int maxMask;
		int defaultMaterialId;
		int pickedMaterialId;
		float reach;		
	};

	typedef SharedPointer<PickItem> PickItemPtr;
}

#endif
