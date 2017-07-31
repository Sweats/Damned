#ifndef TERROR_ITEM_DATA_H
#define TERROR_ITEM_DATA_H

#include "Config.h"

namespace terror {
	class ItemData {
	public:
		ItemData(const std::string& name, const std::string& icon, SoundPtr sound)
			: name(name), icon(icon), sound(sound) {}

		std::string getName() {
			return this->name;
		}

		std::string getIcon() {
			return this->icon;
		}

		SoundPtr getSound() {
			return this->sound;
		}

		virtual ~ItemData() {}

	protected:
		std::string name;
		std::string icon;
		SoundPtr sound;
	};

	typedef SharedPointer<ItemData> ItemDataPtr;
}

#endif