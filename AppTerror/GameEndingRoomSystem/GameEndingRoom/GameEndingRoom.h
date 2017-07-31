#ifndef TERROR_GAME_ENDING_ROOM_H
#define TERROR_GAME_ENDING_ROOM_H

#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	class GameEndingRoom {
	public:
		GameEndingRoom(ObjectPtr object) : object(object) {}

		bool isAllHumansInside(CharacterHandlerPtr characterHandler) {
			HumanPtrMap& humans = characterHandler->getHumansAlive();
			bool atLeastOneHumanInside = false;
			foreach (HumanPtrMap::value_type& entry, humans) {
				HumanPtr human = entry.second;
				if (!human->isDead()) {
					if (this->isHumanInside(human)) {
						atLeastOneHumanInside = true;
					}else {
						return false;
					}
				}
			}
			return atLeastOneHumanInside;
		}

		virtual ~GameEndingRoom() {}

	protected:
		ObjectPtr object;

	private:
		bool isHumanInside(HumanPtr human) {
			Vector3d humanPosition = human->getActor()->getPosition();

			Vector3d position = this->object->getPosition();
			Quaternion orientation = this->object->getOrientation();			

			Vector3d humanPositionObjectSpace = rotate(inverse(orientation), humanPosition - position);

			btTransform identityTransform;
			identityTransform.setIdentity();
			btVector3 aabbMin;
			btVector3 aabbMax;
			object->getCollisionShape()->getAabb(identityTransform, aabbMin, aabbMax);

			if (humanPositionObjectSpace.getX() < aabbMax.x() && humanPositionObjectSpace.getX() > aabbMin.x() &&
				humanPositionObjectSpace.getY() < aabbMax.y() && humanPositionObjectSpace.getY() > aabbMin.y() &&
				humanPositionObjectSpace.getZ() < aabbMax.z() && humanPositionObjectSpace.getZ() > aabbMin.z()) 
			{
				return true;
			}
			return false;
		}
	};

	typedef SharedPointer<GameEndingRoom> GameEndingRoomPtr;
}

#endif