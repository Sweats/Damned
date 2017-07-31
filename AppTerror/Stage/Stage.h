#ifndef TERROR_STAGE_H
#define TERROR_STAGE_H

#include "Packets/Packets.h"
#include "StageData/StageData.h"

namespace terror {
	enum ItemMaskRange {
		MIN_MASK = 0,
		MAX_MASK = 499,
		FLASHLIGHT_BATTERY = 502,
		MONSTER_ARM = 503,
	};

	struct ExtraObjectData {
		int spawnPointId;
		int groupId;
		int groupObjectId;	
	};
	typedef SharedPointer<ExtraObjectData> ExtraObjectDataPtr;

	typedef PointerList<Object> ObjectPtrList;

	class Stage {
	public:
		Stage(StageDataPtr stageData, ScenePtr scene)
			: data(stageData), scene(scene), stageState(new StageState) {}

		string getName() const {
			return this->data->name;
		}

		ScenePtr getScene() const {
			return this->scene;
		}

		void addExtraObject(ExtraObjectDataPtr extraObjectData) {
			std::cout << "\ngroup id = " << extraObjectData->groupId << " object id = " << extraObjectData->groupObjectId << " spawn point id = " << extraObjectData->spawnPointId;

			ObjectSpawnPointPtr selectedSpawnPoint = this->data->objectSpawnPoints[extraObjectData->spawnPointId];
			GroupPtr group = this->data->groups[extraObjectData->groupId];
			GroupObjectDataPtr selectedObject = group->objectDatas[extraObjectData->groupObjectId];
			
			int objectId = this->scene->getNumberOfObjects() + this->extraObjects.size();

			ObjectPtr object = static_ptr_cast<Object>(EntitySystem::getInstance()->createObject(selectedObject->objectData));
			object->setPosition(selectedSpawnPoint->position);
			object->setOrientation(selectedSpawnPoint->orientation);
			object->setMask(selectedObject->mask);

			this->extraObjects.add(object);
		}

		void addBattery(ObjectDataPtr batteryData, int spawnPointId) {
			ObjectSpawnPointPtr batterySpawnPoint = this->data->objectSpawnPoints[spawnPointId];
			int objectId = this->scene->getNumberOfObjects() + this->extraObjects.size();
			
			ObjectPtr object = static_ptr_cast<Object>(EntitySystem::getInstance()->createObject(batteryData));
			object->setPosition(batterySpawnPoint->position);
			object->setOrientation(batterySpawnPoint->orientation);			
			object->setMask(FLASHLIGHT_BATTERY);

			this->extraObjects.add(object);
		}

		void lockObject(const string& objectName, int unlockerMask) {
			ObjectPtr object = this->scene->getObject(objectName);
			if (!object) {
				return;
			}
			object->setMask(object->getMask() + unlockerMask);
			object->lock();
		}

		void destroyObject(const string& objectName) {
			ObjectPtr object = this->scene->getObject(objectName);
			if (!object) {
				return;
			}
			object->destroy();
		}

		ObjectPtr getObject(int id) {
			return EntitySystem::getInstance()->getObject(id);
		}

		ObjectPtrListPtr getObjectsWithMask(int mask) {
			return EntitySystem::getInstance()->getObjectsWithMask(mask);
		}

		ObjectPtrListPtr getObjectsWithMaskBetween(int minMask, int maxMask) {
			return EntitySystem::getInstance()->getObjectsWithMaskBetween(minMask, maxMask);
		}

		void destroyObject(ObjectPtr object) {
			if (!object) {
				return;
			}
			this->stageState->destroyedObjectIds.push_back(object->getId());
			object->destroy();
		}

		void destroyObject(int id) {
			ObjectPtr object = EntitySystem::getInstance()->getObject(id);			
			if (!object) {
				return;
			}
			this->stageState->destroyedObjectIds.push_back(id);
			object->destroy();
		}

		StageStatePtr getState() {
			this->stageState->objectInfos.clear();
			ObjectPtrListPtr dynamicObjects = EntitySystem::getInstance()->getDynamicObjects();			
			foreach (ObjectPtr object, *dynamicObjects) {
				if (object->getMask() >= 1000 && object->getMask() < 2500 &&
					!object->isLocked() && !this->scene->isObjectInitialTransform(object)) 
				{
					ObjectInfoPtr objectInfo(new ObjectInfo(object));
					this->stageState->objectInfos.add(objectInfo);
				}
			}
			return this->stageState;
		}

		void destroy() {			
			this->scene->destroy();
			foreach (ObjectPtr object, this->extraObjects) {
				if (object) {
					object->destroy();
				}
			}
			this->extraObjects.clear();			
		}

		virtual ~Stage() {}		

	protected:
		StageDataPtr data;
		StageStatePtr stageState;
		ScenePtr scene;		
		ObjectPtrList extraObjects;
	};

	typedef SharedPointer<Stage> StagePtr;
}

#endif