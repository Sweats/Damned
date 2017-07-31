#ifndef TERROR_DEAD_HUMAN_DATA_H
#define TERROR_DEAD_HUMAN_DATA_H

#include "Packets/Vector3dInfo/Vector3dInfo.h"
#include "Packets/QuaternionInfo/QuaternionInfo.h"
#include "Packets/CharSelectionRequest/CharSelectionRequest.h"
#include "Packets/CharactersItems/CharactersItems.h" 

namespace terror {
	class DeadHumanData {
	public:
		friend class boost::serialization::access;

		DeadHumanData() {}

		void setCharacterType(CharacterType characterType) {
			this->characterType = characterType;
		}

		void setTransform(const Vector3d& position, const Quaternion& orientation) {
			this->position.setVector3d(position);
			this->orientation.setQuaternion(orientation);
		}

		void setPosition(const Vector3d& position) {
			this->position.setVector3d(position);
		}

		void setCharacterItems(CharacterItemsPtr characterItems) {
			this->characterItems = characterItems;
		}

		void setId(int id) {
			this->id = id;
		}

		CharacterType getCharacterType() {
			return this->characterType;
		}

		Vector3d getPosition() const {
			return this->position.buildVector3d();
		}

		Quaternion getOrientation() const {
			return this->orientation.buildQuaternion();
		}

		CharacterItemsPtr getCharacterItems() {
			return this->characterItems;
		}

		int getId() {
			return this->id;
		}

		virtual ~DeadHumanData() {}

	protected:
		CharacterType characterType;
		Vector3dInfo position;
		QuaternionInfo orientation;
		CharacterItemsPtr characterItems;
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->characterType;
			archive & this->position;
			archive & this->orientation;
			archive & this->characterItems;
			archive & this->id;
		}
	};

	typedef SharedPointer<DeadHumanData> DeadHumanDataPtr;
}

#endif