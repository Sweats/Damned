#ifndef TERROR_LOBBY_GREETINGS_H
#define TERROR_LOBBY_GREETINGS_H

#include "Packets/PacketsEnum.h"

namespace terror {
	class Greetings : public Sendable {
	public:
		friend class boost::serialization::access;

		Greetings() {}

		Greetings(const string& name, int id) : name(name), id(id) {}

		virtual int getLabel() const {
			return GREETINGS;
		}

		string getName() {
			return this->name;
		}

		int getId() {
			return this->id;
		}

		virtual ~Greetings() {}

	protected:
		string name;
		int id;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
			archive & this->id;
		}
	};

	typedef SharedPointer<Greetings> GreetingsPtr;
}

#endif