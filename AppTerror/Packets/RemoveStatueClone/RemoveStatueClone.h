#ifndef TERROR_REMOVE_STATUE_CLONE_H
#define TERROR_REMOVE_STATUE_CLONE_H

#include "Packets/PacketsEnum.h"

namespace terror {	
	class RemoveStatueClone : public Sendable {
	public:
		friend class boost::serialization::access;

		RemoveStatueClone() {}

		RemoveStatueClone(int id) : id(id) {}

		int getId() {
			return this->id;
		}
		
		virtual int getLabel() const {
			return REMOVE_STATUE_CLONE;
		}

		virtual ~RemoveStatueClone() {}

	private:
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->id;
		}
	};

	typedef SharedPointer<RemoveStatueClone> RemoveStatueClonePtr;
}

#endif