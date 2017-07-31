#ifndef TERROR_ITEM_H
#define TERROR_ITEM_H

#include "Config.h"

namespace terror {
	class Item {
	public:
		Item(int mask) : mask(mask) {}

		int getMask() {
			return this->mask;
		}

		virtual ~Item() {}

	protected:
		int mask;
	};

	typedef SharedPointer<Item> ItemPtr;
}

#endif
