#ifndef TERROR_STAGE_FACTORY_H
#define TERROR_STAGE_FACTORY_H

#include "Stage/Stage.h"
#include "Packets/StageInfo/StageInfo.h"

namespace terror {
	typedef unordered_map<int, int> IntMap;
	typedef SharedPointer<IntMap> IntMapPtr;

	class StageFactory {
	public:
		StageFactory(SceneFactoryPtr sceneFactory, PhysicsSystemPtr physicsSystem) 
			: sceneFactory(sceneFactory), physicsSystem(physicsSystem) {}

		StagePtr create(StageDataPtr stageData, int seed) {
			ScenePtr scene = this->sceneFactory->create(stageData->sceneData);
			scene->startAmbientMusic();
	
			RandomizerPtr randomizer(new Randomizer(seed));
			StagePtr stage(new Stage(stageData, scene));

			this->usedSpawnPoints.clear();
			this->buildExtraObjects(stage, stageData, randomizer);
			this->addBatteries(stage, stageData, randomizer);
			this->treatRandomObjects(stage, stageData, randomizer);
			this->lockObjects(stage, stageData);
			this->addObjectsConstraints(stage);

			return stage;
		}

		virtual ~StageFactory() {}

	protected:
		SceneFactoryPtr sceneFactory;
		PhysicsSystemPtr physicsSystem;
		IntMap usedSpawnPoints;

	private:
		void lockObjects(StagePtr stage, StageDataPtr stageData) {
			foreach (LockedObjectPtr lockedObject, stageData->lockedObjects) {
				stage->lockObject(lockedObject->objectName, lockedObject->unlockerMask);
			}
		}

		void buildExtraObjects(StagePtr stage, StageDataPtr stageData, RandomizerPtr randomizer) {
			std::set<int> groupIds;
			foreach (GroupPtrMap::value_type& entry, stageData->groups) {
				groupIds.insert(entry.first);
			}

			int usedSpawnPointsIndex = this->usedSpawnPoints.size();
			foreach (int groupId, groupIds) {
				IntMapPtr validSpawnPointIds = this->getValidSpawnPointIdsFromGroup(stageData, groupId);
				if (validSpawnPointIds->size() != 0) {
					ExtraObjectDataPtr extraObjectData(new ExtraObjectData);
					extraObjectData->groupId = groupId;
					int spawnPointId = randomizer->randomNumber(0, validSpawnPointIds->size() - 1);
					extraObjectData->spawnPointId = (*validSpawnPointIds)[spawnPointId];	
					GroupPtr group = stageData->groups[groupId];
					if (group->objectDatas.size() != 0) {
						int objectId = randomizer->randomNumber(0, group->objectDatas.size() - 1);
						extraObjectData->groupObjectId = objectId;						
						stage->addExtraObject(extraObjectData);
						this->usedSpawnPoints[usedSpawnPointsIndex] = extraObjectData->spawnPointId;
						++usedSpawnPointsIndex;
					}
				}
			}
		}

		void addBatteries(StagePtr stage, StageDataPtr stageData, RandomizerPtr randomizer) {
			std::set<int> batterySpawnPoints;
			foreach(ObjectSpawnPointPtrMap::value_type& entry, stageData->objectSpawnPoints) {
				if (!this->isSpawnPointUsed(entry.first)) {
					batterySpawnPoints.insert(entry.first);
				}
			}
			int usedSpawnPointsIndex = this->usedSpawnPoints.size();
			foreach (int batterySpawnPoint, batterySpawnPoints) {
				if (this->requestBatteryAddition(stage, stageData, randomizer, batterySpawnPoint)) {
					this->usedSpawnPoints[usedSpawnPointsIndex] = batterySpawnPoint;
					++usedSpawnPointsIndex;
				}
			}

		}

		bool isSpawnPointUsed(int spawnPointId) {
			foreach(IntMap::value_type& entry, this->usedSpawnPoints) {
				if (entry.second == spawnPointId) {
					return true;
				}
			}
			return false;
		}

		bool requestBatteryAddition(StagePtr stage, StageDataPtr stageData, RandomizerPtr randomizer, int spawnPointId) {
			int percentage;
			if (stageData->name == "Rose_Crimson_Hotel") {
				percentage = 90;
			}else if (stageData->name == "Factory"){
				percentage = 70;
			}else if (stageData->name == "Black_Lake_Woods") {
				percentage = 75;
			}else {
				percentage = 80;
			}
			int randomPercent = randomizer->randomNumber(1, 100);
			if (randomPercent > percentage) {
				stage->addBattery(stageData->batteryData, spawnPointId);
				return true;
			}
			return false;
		}

		/*void buildExtraObjects(StagePtr stage, StageDataPtr stageData, RandomizerPtr randomizer) { //CHEAT, all spawn points are used!
			ObjectPtrListPtr objects(new ObjectPtrList);
			for (int i = 0; i < stageData->groups.size(); ++i) {
				ExtraObjectDataPtr extraObjectData(new ExtraObjectData);

				IntMapPtr validSpawnPointIds = this->getValidSpawnPointIdsFromGroup(stageData, i);
				for (int j = 0; j < validSpawnPointIds->size(); ++j) {
					extraObjectData->spawnPointId = (*validSpawnPointIds)[j];

					GroupPtr group = stageData->groups[i];
					if (group->objectDatas.size() != 0) {
						extraObjectData->groupObjectId = randomizer->randomNumber(0, group->objectDatas.size() - 1);
						stage->addExtraObject(extraObjectData);
					}
				}
			}
		}*/

		void treatRandomObjects(StagePtr stage, StageDataPtr stageData, RandomizerPtr randomizer) {
			foreach (RandomObjectPtr randomObject, stageData->randomObjects) {
				int randomPercent = randomizer->randomNumber(1, 100);
				if (randomPercent > randomObject->probability) {
					stage->destroyObject(randomObject->objectName);
				}
			}
		}

		IntMapPtr getValidSpawnPointIdsFromGroup(StageDataPtr stageData, int groupId) {
			IntList validSpawnPointIdsList;
			foreach (ObjectSpawnPointPtrMap::value_type& objectSpawnPointEntry, stageData->objectSpawnPoints) {
				ObjectSpawnPointPtr objectSpawnPoint = objectSpawnPointEntry.second;
				foreach (IntMap::value_type& entry, objectSpawnPoint->groupIds) {
					if (entry.second == groupId) {
						if (entry.first == 0) {
							validSpawnPointIdsList.push_back(objectSpawnPointEntry.first);
						}else {
							if (!EntitySystem::getInstance()->getObjectsWithMask(entry.first)->empty()) {
								validSpawnPointIdsList.push_back(objectSpawnPointEntry.first);
							}
						}
					}
				}			
			}
			validSpawnPointIdsList.sort();
			int counter = 0;
			IntMapPtr validSpawnPointIdsMap(new IntMap);
			foreach (int validSpawnPointId, validSpawnPointIdsList) {
				(*validSpawnPointIdsMap)[counter] = validSpawnPointId;
				++counter;
			}
			return validSpawnPointIdsMap;
		}

		void addObjectsConstraints(StagePtr stage) {
			ObjectPtrListPtr doors = EntitySystem::getInstance()->getObjectsWithMaskBetween(1000, 1499);
			foreach (ObjectPtr door, *doors) {
				RigidBodyPtr body = door->getBody();				
				body->setRestitution(0.0f);
				body->setDamping(0.95f, 0.95f);

				btTransform transform = body->getWorldTransform();
				body->getWorldTransform().setIdentity();
				btVector3 aabbMin, aabbMax;
				body->getAabb(aabbMin, aabbMax);
				body->setWorldTransform(transform);
				float hingeOffset = 0.1f + std::min(fabs(aabbMax.x()), fabs(aabbMin.x()));

				btHingeConstraint* constraint = new btHingeConstraint(*body, btVector3(hingeOffset, 0.0f, 0.0f), btVector3(0.0f, 1.0f, 0.0f), true);
				//ERP = hk/(hk + c); h = time step; k = spring k; c = damping coef;
				//CFM = 1/(hk + c);
				constraint->setParam(BT_CONSTRAINT_ERP, 0.2f);
				constraint->setParam(BT_CONSTRAINT_CFM, 0.0f);
				body->addConstraintRef(constraint);
				this->physicsSystem->getDynamicsWorld()->addConstraint(constraint);

				door->setMaterialId(5);
			}

			ObjectPtrListPtr drawers = EntitySystem::getInstance()->getObjectsWithMaskBetween(1500, 1999);
			foreach (ObjectPtr drawer, *drawers) {
				RigidBodyPtr body = drawer->getBody();
				body->setRestitution(0.0f);
				body->setDamping(0.95f, 0.95f);

				Quaternion frameOrientation(Vector3d::zUnit, Vector3d::yUnit, -1.0f*Vector3d::xUnit);

				btTransform frame;
				frame.setIdentity();				
				frame.setRotation(toBullet(frameOrientation));

				btTransform transform = body->getWorldTransform();
				body->getWorldTransform().setIdentity();
				btVector3 aabbMin, aabbMax;
				body->getAabb(aabbMin, aabbMax);
				body->setWorldTransform(transform);

				btSliderConstraint* constraint = new btSliderConstraint(*body, frame, true);
				constraint->setParam(BT_CONSTRAINT_ERP, 0.2f);
				constraint->setParam(BT_CONSTRAINT_CFM, 0.0f);
				constraint->setLowerLinLimit(0.0f);
				constraint->setUpperLinLimit(0.7f*fabs(aabbMax.z() - aabbMin.z()));
				constraint->setSoftnessDirLin(0.0f);
				constraint->setSoftnessLimLin(0.0f);
				constraint->setSoftnessOrthoLin(0.0f);
				constraint->setRestitutionDirLin(0.0f);
				constraint->setRestitutionLimLin(0.0f);
				constraint->setRestitutionOrthoLin(0.0f);
				constraint->setDampingDirLin(1.0f);
				constraint->setDampingLimLin(1.0f);
				constraint->setDampingOrthoLin(1.0f);
				body->addConstraintRef(constraint);
				this->physicsSystem->getDynamicsWorld()->addConstraint(constraint);

				drawer->setMaterialId(5);
			}
		}
	};

	typedef SharedPointer<StageFactory> StageFactoryPtr;
}

#endif