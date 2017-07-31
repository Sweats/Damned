#ifndef TERROR_STAGE_DATA_H
#define TERROR_STAGE_DATA_H

#include "Config.h"

namespace terror {
	struct GroupObjectData {
		ObjectDataPtr objectData;
		int mask;
	};
	typedef SharedPointer<GroupObjectData> GroupObjectDataPtr;

	typedef unordered_map<int, GroupObjectDataPtr> GroupObjectDataPtrMap;
	struct Group {
		GroupObjectDataPtrMap objectDatas;
	};
	typedef SharedPointer<Group> GroupPtr;

	typedef unordered_map<int, int> IntMap;
	struct ObjectSpawnPoint {
		Vector3d position;
		Quaternion orientation;
		IntMap groupIds;
	};
	typedef SharedPointer<ObjectSpawnPoint> ObjectSpawnPointPtr;

	struct RandomObject {
		string objectName;
		int probability;
	};
	typedef SharedPointer<RandomObject> RandomObjectPtr;

	struct LockedObject {
		string objectName;
		int unlockerMask;
	};
	typedef SharedPointer<LockedObject> LockedObjectPtr;

	typedef unordered_map<int, GroupPtr> GroupPtrMap;
	typedef unordered_map<int, ObjectSpawnPointPtr> ObjectSpawnPointPtrMap;
	typedef PointerList<RandomObject> RandomObjectPtrList;
	typedef PointerList<LockedObject> LockedObjectPtrList;
	struct StageData {
		string name;
		SceneDataPtr sceneData;
		GroupPtrMap groups;
		ObjectSpawnPointPtrMap objectSpawnPoints;
		RandomObjectPtrList randomObjects;
		LockedObjectPtrList lockedObjects;
		ObjectDataPtr batteryData;
	};
	typedef SharedPointer<StageData> StageDataPtr;
}

#endif
