#ifndef TERROR_STAGE_BUILDER_H
#define TERROR_STAGE_BUILDER_H

#include "StageData/StageData.h"

namespace terror {
	class StageBuilder {
	public:		
		StageBuilder(ParseTreeFactoryPtr parseTreeFactory, SceneBuilderPtr sceneBuilder)
			: parseTreeFactory(parseTreeFactory), sceneBuilder(sceneBuilder), vector3dBuilder(new Vector3dBuilder), quaternionBuilder(new QuaternionBuilder) {}

		StageDataPtr build(NodePtr data) {
			StageDataPtr stageData(new StageData);

			stageData->name = data->getValue<string>();

			std::string sceneName = data->getChildByType("scene", true)->getValue<std::string>();
			NodePtr sceneNode = this->parseTreeFactory->create(sceneName + ".scene")->getChildByValue(sceneName, true);
			stageData->sceneData = this->sceneBuilder->build(sceneNode);

			NodePtr groupsNode = data->getChildByType("groups");
			if (groupsNode) {
				foreach (NodePtr groupNode, groupsNode->getChildren()) {
					int groupId = groupNode->getValue<int>();
					stageData->groups[groupId] = this->buildGroup(groupNode);
				}
			}

			NodePtr objectSpawnPointsNode = data->getChildByType("object_spawn_points");
			if (objectSpawnPointsNode) {
				foreach (NodePtr objectSpawnPointNode, objectSpawnPointsNode->getChildren()) {
					string spawnPointName = objectSpawnPointNode->getValue<string>();
					std::vector<string> spawnPointNameSplit;
					split(spawnPointNameSplit, spawnPointName, is_any_of("."));
					int spawnPointId = lexical_cast<int>(spawnPointNameSplit[1]);
					stageData->objectSpawnPoints[spawnPointId] = this->buildObjectSpawnPoint(objectSpawnPointNode);
				}
			}

			NodePtr randomObjectsNode = data->getChildByType("random_objects");
			if (randomObjectsNode) {
				foreach (NodePtr randomObjectNode, randomObjectsNode->getChildren()) {
					RandomObjectPtr randomObject(new RandomObject);
					randomObject->objectName = randomObjectNode->getChildValue("object", string(""));
					randomObject->probability = randomObjectNode->getChildValue("probability", 100);
					stageData->randomObjects.add(randomObject);
				}
			}

			NodePtr lockedObjectsNode = data->getChildByType("locked_objects");
			if (lockedObjectsNode) {
				foreach (NodePtr lockedObjectNode, lockedObjectsNode->getChildren()) {
					LockedObjectPtr lockedObject(new LockedObject);
					lockedObject->objectName = lockedObjectNode->getChildValue("object", string(""));
					lockedObject->unlockerMask = lockedObjectNode->getChildValue("unlocker_mask", 0);
					stageData->lockedObjects.add(lockedObject);
				}
			}

			stageData->batteryData = EntitySystem::getInstance()->preloadObject("battery");

			return stageData;
		}

		virtual ~StageBuilder() {}

	protected:
		ParseTreeFactoryPtr parseTreeFactory;
		SceneBuilderPtr sceneBuilder;
		Vector3dBuilderPtr vector3dBuilder;
		QuaternionBuilderPtr quaternionBuilder;

	private:
		GroupPtr buildGroup(NodePtr groupNode) {
			GroupPtr group(new Group);
			NodePtr objectsNode = groupNode->getChildByType("objects");
			if (!objectsNode) {
				return group;
			}
			int index = 0;
			foreach (NodePtr objectNode, objectsNode->getChildren()) {
				group->objectDatas[index] = this->buildGroupObject(objectNode);
				++index;
			}
			return group;
		}

		GroupObjectDataPtr buildGroupObject(NodePtr groupObjectNode) {
			GroupObjectDataPtr groupObjectData(new GroupObjectData);
			groupObjectData->objectData = EntitySystem::getInstance()->preloadObject(groupObjectNode->getChildValue("type", string("")));
			groupObjectData->mask = groupObjectNode->getChildValue("mask", 0);
			return groupObjectData;
		}

		ObjectSpawnPointPtr buildObjectSpawnPoint(NodePtr objectSpawnPointNode) {
			ObjectSpawnPointPtr objectSpawnPoint(new ObjectSpawnPoint);
			objectSpawnPoint->position = this->vector3dBuilder->build(objectSpawnPointNode->getChild("position", true));
			objectSpawnPoint->orientation = this->quaternionBuilder->build(objectSpawnPointNode->getChild("orientation", true));
			NodePtr groupsNode = objectSpawnPointNode->getChildByType("groups");
			if (!groupsNode) { //Temp
				return objectSpawnPoint;
			}
			foreach (NodePtr groupNode, groupsNode->getChildren()) {
				int groupId = groupNode->getValue<int>();
				int mask = groupNode->getChildValue("require_mask", 0);
				objectSpawnPoint->groupIds[mask] = groupId;
			}
			return objectSpawnPoint;
		}
	};

	typedef SharedPointer<StageBuilder> StageBuilderPtr;
}

#endif