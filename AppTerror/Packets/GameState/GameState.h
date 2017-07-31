#ifndef TERROR_GAME_STATE_H
#define TERROR_GAME_STATE_H

#include "Packets/PacketsEnum.h"
#include "Packets/HumanInfo/HumanInfo.h"
#include "Packets/MonsterInfo/MonsterInfo.h"
#include "Packets/ObjectInfo/ObjectInfo.h"

namespace terror {
	typedef PointerList<HumanInfo> HumanInfoPtrList;

	struct CharactersGameState : public Sendable {
		friend class boost::serialization::access;
		
		CharactersGameState() : monster(new MonsterInfo), ignoreMonsterInfo(false) {}

		virtual int getLabel() const {
			return CHARACTERS_GAME_STATE;
		}

		virtual ~CharactersGameState() {}

		HumanInfoPtrList humans;
		MonsterInfoPtr monster;
		bool ignoreMonsterInfo;

	private:
		template<class Archive_>
		void save(Archive_& archive, const unsigned int version) const {
			int size = this->humans.size();
			archive & size;
			archive & this->humans;
			archive & this->monster;
			archive & this->ignoreMonsterInfo;
		}

		template<class Archive_>
		void load(Archive_& archive, const unsigned int version) {
			int size;
			archive & size;

			this->humans.clear();
			for (int i = 0; i < size; ++i) {
				this->humans.add(HumanInfoPtr(new HumanInfo));
			}

			archive & this->humans;
			archive & this->monster;
			archive & this->ignoreMonsterInfo;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()
	};

	typedef SharedPointer<CharactersGameState> CharactersGameStatePtr;

	typedef PointerList<ObjectInfo> ObjectInfoPtrList;
	struct ObjectsGameState : public Sendable {
		friend class boost::serialization::access;
		
		ObjectsGameState() {}

		virtual int getLabel() const {
			return OBJECTS_GAME_STATE;
		}

		virtual ~ObjectsGameState() {}

		ObjectInfoPtrList objects;

	private:
		template<class Archive_>
		void save(Archive_& archive, const unsigned int version) const {
			int size = this->objects.size();
			archive & size;
			archive & this->objects;
		}

		template<class Archive_>
		void load(Archive_& archive, const unsigned int version) {
			int size;
			archive & size;

			this->objects.clear();
			for (int i = 0; i < size; ++i) {
				this->objects.add(ObjectInfoPtr(new ObjectInfo));
			}

			archive & this->objects;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()
	};

	typedef SharedPointer<ObjectsGameState> ObjectsGameStatePtr;

	struct DestroyedObjectsGameState : public Sendable {
		friend class boost::serialization::access;
		
		DestroyedObjectsGameState() {}

		virtual int getLabel() const {
			return DESTROYED_OBJECTS_GAME_STATE;
		}

		IntList destroyedObjectIds;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->destroyedObjectIds;
		}
	};

	typedef SharedPointer<DestroyedObjectsGameState> DestroyedObjectsGameStatePtr;

	struct GameState {
		GameState() : charactersGameState(new CharactersGameState), objectsGameState(new ObjectsGameState), destroyedObjectsGameState(new DestroyedObjectsGameState) {}

		CharactersGameStatePtr charactersGameState;
		ObjectsGameStatePtr objectsGameState;
		DestroyedObjectsGameStatePtr destroyedObjectsGameState;
	};

	typedef SharedPointer<GameState> GameStatePtr;
}

#endif