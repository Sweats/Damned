#ifndef TERROR_INVENTORY_H
#define TERROR_INVENTORY_H

#include "Item/Item.h"
#include "ItemDatabase/ItemDatabase.h"

namespace terror {
	typedef unordered_map<int, ItemPtr> ItemPtrMap;

	class Inventory {
	public:
		Inventory(bool isLocal) : isLocal(isLocal) {
			if (this->isLocal) {
				this->inventoryLayout = loadLayout("Inventory.layout");
				this->inventoryLayout->show();
			}
		}
		
		void addItem(ItemPtr item, bool playSound) {
			this->items[item->getMask()] = item;
			ItemDatabasePtr itemDatabase = ItemDatabase::getInstance();
			ItemDataPtr itemData = itemDatabase->getItemData(item->getMask());			

			if (!this->isLocal) {
				return;
			}

			if (playSound) {
				itemData->getSound()->play();
			}
			std::string imageBoxName = std::string("Item") + lexical_cast<std::string>(this->items.size());
			MyGUI::ImageBox* itemBox = static_cast<MyGUI::ImageBox*>(this->inventoryLayout->getWidget(imageBoxName));
			itemBox->setImageTexture(itemData->getIcon());
		}

		bool hasItem(int mask) {
			return this->items.find(mask) != this->items.end();
		}

		ItemPtrMap& getItems() {
			return this->items;
		}

		void setLocal(bool isLocal) {
			if (this->isLocal && !isLocal && this->inventoryLayout) {
				this->inventoryLayout->destroy();
			}else if (!this->isLocal && isLocal) {
				this->inventoryLayout = loadLayout("Inventory.layout");
				this->inventoryLayout->show();
			}
			this->isLocal = isLocal;
		}

		void clear() {			
			this->items.clear();
			if (this->isLocal) {
				this->inventoryLayout->destroy();
			}
		}

		virtual ~Inventory() {}

	protected:
		ItemPtrMap items;
		GuiLayoutPtr inventoryLayout;
		bool isLocal;
	};

	typedef SharedPointer<Inventory> InventoryPtr;
}

#endif
