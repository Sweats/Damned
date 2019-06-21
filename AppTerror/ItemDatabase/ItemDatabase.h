#ifndef TERROR_ITEM_DATABASE_H
#define TERROR_ITEM_DATABASE_H

#include "ItemData/ItemData.h"

namespace terror {
	typedef unordered_map<int, ItemDataPtr> ItemDataPtrMap;

	class ItemDatabase {
	public:
		typedef SharedPointer<ItemDatabase> ItemDatabasePtr;

		static ItemDatabasePtr getInstance() {
			if (!ItemDatabase::instance) {
				ItemDatabase::instance = ItemDatabasePtr(new ItemDatabase);
			}
			return ItemDatabase::instance;
		}

		void initialize() {
			std::stringstream stream;
			stream << " 1 Rusty_Key icon_key.png key_picking.ogg"
				<< " 2 Rusty_Key icon_key.png key_picking.ogg"
				<< " 3 Bar_Key icon_keybar.png key_picking.ogg"
				<< " 4 Rusty_Key icon_key.png key_picking.ogg"
				<< " 5 Rusty_Key icon_key.png key_picking.ogg"
				<< " 6 Rusty_Key icon_key.png key_picking.ogg"
				<< " 7 Metal_Key icon_key2.png key_picking.ogg"
				<< " 8 Depot_Key icon_keydepot.png key_picking.ogg"
				<< " 9 Metal_Key icon_key2.png key_picking.ogg"
				<< " 10 Kitchen_Key icon_keykitchen.png key_picking.ogg"
				<< " 11 Numeric_Code icon_paper.png paper.ogg"
				<< " 12 Silver_Key icon_key3.png key_picking.ogg"
				<< " 13 Crowbar icon_crowbar.png metal.ogg"
				<< " 14 Metal_Key icon_key2.png key_picking.ogg"
				<< " 15 Bar_key icon_keybar.png key_picking.ogg"
				<< " 16 Rusty_key icon_key.png key_picking.ogg"
				<< " 17 Metal_Key icon_key2.png key_picking.ogg"
				<< " 18 Rusty_key icon_key.png key_picking.ogg"
				<< " 19 Small_Metal_Key icon_small_key.png key_picking.ogg"
				<< " 21 Metal_Key icon_key2.png key_picking.ogg"
				<< " 22 Rusty_Key icon_key.png key_picking.ogg"
				<< " 23 Rusty_Key icon_key.png key_picking.ogg"
				<< " 24 Rusty_Key icon_key.png key_picking.ogg"
				<< " 25 Rusty_Key icon_key.png key_picking.ogg"
				<< " 26 Rusty_Key icon_key.png key_picking.ogg"
				<< " 27 Rusty_Key icon_key.png key_picking.ogg"
				<< " 28 Rusty_Key icon_key.png key_picking.ogg"
				<< " 29 Rusty_Key icon_key.png key_picking.ogg"
				<< " 30 Metal_Key icon_key2.png key_picking.ogg"
				<< " 31 Rusty_Key icon_key.png key_picking.ogg"
				<< " 32 Numeric_Code icon_paper.png paper.ogg"
				<< " 33 Numeric_Code icon_paper.png paper.ogg"
				<< " 34 Rusty_Key icon_key.png key_picking.ogg"
				<< " 35 Metal_Key icon_key2.png key_picking.ogg"
				<< " 36 Silver_Key icon_key3.png key_picking.ogg"
				<< " 37 Silver_Key icon_key3.png key_picking.ogg"
				<< " 38 Crowbar icon_crowbar.png metal.ogg"
				<< " 39 Rusty_Key icon_key.png key_picking.ogg"
				<< " 40 Rusty_Key icon_key.png key_picking.ogg"
				<< " 41 Metal_Key icon_key2.png key_picking.ogg"
				<< " 42 Kitchen_Key icon_keykitchen.png key_picking.ogg"
				<< " 43 Kitchen_Key icon_keykitchen.png key_picking.ogg"
				<< " 44 Numeric_Code icon_paper.png paper.ogg"
				<< " 45 Numeric_Code icon_paper.png paper.ogg"
				<< " 46 Numeric_Code icon_paper.png paper.ogg"
				<< " 47 Numeric_Code icon_paper.png paper.ogg"
				<< " 48 Numeric_Code icon_paper.png paper.ogg"
				<< " 49 Silver_Key icon_key3.png key_picking.ogg"
				<< " 50 Silver_Key icon_key3.png key_picking.ogg"
				<< " 51 Crowbar icon_crowbar.png metal.ogg"
				<< " 52 Numeric_Code icon_paper.png paper.ogg"
				<< " 53 Numeric_Code icon_paper.png paper.ogg"
				<< " 54 Numeric_Code icon_paper.png paper.ogg"
				<< " 55 Numeric_Code icon_paper.png paper.ogg"
				<< " 56 Numeric_Code icon_paper.png paper.ogg"
				<< " 57 Numeric_Code icon_paper.png paper.ogg"
				<< " 58 Numeric_Code icon_paper.png paper.ogg"
				<< " 59 Numeric_Code icon_paper.png paper.ogg"
				<< " 60 Numeric_Code icon_paper.png paper.ogg"
				<< " 61 Numeric_Code icon_paper.png paper.ogg"
				<< " 62 Numeric_Code icon_paper.png paper.ogg"
				<< " 63 Numeric_Code icon_paper.png paper.ogg"
				<< " 64 Metal_Key icon_key2.png key_picking.ogg"
				<< " 65 Silver_Key icon_key3.png key_picking.ogg"
				<< " 66 Silver_Key icon_key3.png key_picking.ogg"
				<< " 67 Crowbar icon_crowbar.png metal.ogg"
				<< " 68 Steel_Key cut_key.png key_picking.ogg"
				<< " 69 Steel_Key cut_key.png key_picking.ogg"
				<< " 70 Steel_Key cut_key.png key_picking.ogg"
				<< " 71 Steel_Key cut_key.png key_picking.ogg"
				<< " 72 Steel_Key cut_key.png key_picking.ogg"
				<< " 73 Steel_Key cut_key.png key_picking.ogg"
				<< " 74 Kitchen_Key icon_keykitchen.png key_picking.ogg"
				<< " 75 Steel_Key cut_key.png key_picking.ogg"
				<< " 76 Rusty_Key icon_key.png key_picking.ogg"
				<< " 77 Rusty_Key icon_key.png key_picking.ogg"
				<< " 78 Rusty_Key icon_key.png key_picking.ogg"
				<< " 79 Old_Key old_key.png key_picking.ogg"
				<< " 80 Rusty_Key icon_key.png key_picking.ogg"
				<< " 81 Rusty_Key icon_key.png key_picking.ogg"
				<< " 82 Rusty_Key icon_key.png key_picking.ogg"
				<< " 83 Steel_Key cut_key.png key_picking.ogg"
				<< " 84 Old_Key old_key.png key_picking.ogg"
				<< " 85 Rusty_Key icon_key.png key_picking.ogg"
				<< " 86 Rusty_Key icon_key.png key_picking.ogg"
				<< " 87 Silver_Key icon_key3.png key_picking.ogg"
				<< " 88 Hammer icon_hammer.png metal.ogg"
				<< " 89 Old_Key old_key.png key_picking.ogg"
				<< " 90 Metal_Key icon_key2.png key_picking.ogg"
				<< " 91 Rusty_Key icon_key.png key_picking.ogg"
				<< " 92 Rusty_Key icon_key.png key_picking.ogg"
				<< " 93 Steel_Key cut_key.png key_picking.ogg"
				<< " 94 Steel_Key cut_key.png key_picking.ogg"
				<< " 95 Steel_Key cut_key.png key_picking.ogg"
				<< " 96 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 97 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 98 Metal_Key icon_key2.png key_picking.ogg"
				<< " 99 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 100 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 101 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 102 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 103 Rusty_Key icon_key.png key_picking.ogg"
				<< " 104 Metal_Key icon_key2.png key_picking.ogg"
				<< " 105 Rusty_Key icon_key.png key_picking.ogg"
				<< " 106 Steel_Key cut_key.png key_picking.ogg"
				<< " 107 Steel_Key cut_key.png key_picking.ogg"
				<< " 108 Steel_Key cut_key.png key_picking.ogg"
				<< " 109 Kitchen_Key icon_keykitchen.png key_picking.ogg"
				<< " 110 Old_Key old_key.png key_picking.ogg"
				<< " 111 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 112 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 113 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 114 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 115 Silver_Key icon_key3.png key_picking.ogg"
				<< " 116 Silver_Key icon_key3.png key_picking.ogg"
				<< " 117 Hammer icon_hammer.png metal.ogg"
				<< " 118 Metal_Key icon_key2.png key_picking.ogg"
				<< " 119 Metal_Key icon_key2.png key_picking.ogg"
				<< " 120 Old_Key old_key.png key_picking.ogg"
				<< " 121 Steel_Key cut_key.png key_picking.ogg"
				<< " 122 Steel_Key cut_key.png key_picking.ogg"
				<< " 123 Steel_Key cut_key.png key_picking.ogg"
				<< " 124 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 125 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 126 Steel_Key cut_key.png key_picking.ogg"
				<< " 127 Rusty_Key icon_key.png key_picking.ogg"
				<< " 128 Silver_Key icon_key3.png key_picking.ogg"
				<< " 129 Crowbar icon_crowbar.png metal.ogg"
				<< " 130 Metal_Key icon_key2.png key_picking.ogg"
				<< " 131 Steel_Key cut_key.png key_picking.ogg"
				<< " 132 Metal_Key icon_key2.png key_picking.ogg"
				<< " 133 Steel_Key cut_key.png key_picking.ogg"
				<< " 134 Steel_Key cut_key.png key_picking.ogg"
				<< " 135 Silver_Key icon_key3.png key_picking.ogg"
				<< " 136 Crowbar icon_crowbar.png metal.ogg"
				<< " 137 Rusty_Key icon_key.png key_picking.ogg"
				<< " 138 Rusty_Key icon_key.png key_picking.ogg"
				<< " 139 Rusty_Key icon_key.png key_picking.ogg"
				<< " 140 Rusty_Key icon_key.png key_picking.ogg"
				<< " 141 Bar_key icon_keybar.png key_picking.ogg"
				<< " 142 Metal_Key icon_key2.png key_picking.ogg"
				<< " 143 Rusty_Key icon_key.png key_picking.ogg"
				<< " 144 Silver_Key icon_key3.png key_picking.ogg"
				<< " 145 Crowbar icon_crowbar.png metal.ogg"
				<< " 146 Rusty_Key icon_key.png key_picking.ogg"
				<< " 147 Iron_Key Iron_key.png key_picking.ogg"
				<< " 148 Steel_Key cut_key.png key_picking.ogg"
				<< " 149 Old_Key old_key.png key_picking.ogg"
				<< " 150 Silver_Key icon_key3.png key_picking.ogg"
				<< " 151 Hammer icon_hammer.png metal.ogg"
				<< " 152 Rusty_Key icon_key.png key_picking.ogg"
				<< " 153 Steel_Key cut_key.png key_picking.ogg"
				<< " 154 Steel_Key cut_key.png key_picking.ogg"
				<< " 155 Bolt_Cutter bolt_cutter.png metal.ogg"
				<< " 156 Iron_Key Iron_key.png key_picking.ogg"
				<< " 157 Room_Key icon_rooms_a1.png key_picking.ogg"
				<< " 158 Silver_Key icon_key3.png key_picking.ogg"
				<< " 159 Numeric_Code icon_paper.png paper.ogg"
				<< " 160 Numeric_Code icon_paper.png paper.ogg"
				<< " 161 Numeric_Code icon_paper.png paper.ogg"
				<< " 162 Numeric_Code icon_paper.png paper.ogg"
				<< " 163 Numeric_Code icon_paper.png paper.ogg"
				<< " 164 Numeric_Code icon_paper.png paper.ogg"
				<< " 165 Numeric_Code icon_paper.png paper.ogg"
				<< " 166 Numeric_Code icon_paper.png paper.ogg"
				<< " 167 Numeric_Code icon_paper.png paper.ogg"
				<< " 168 Numeric_Code icon_paper.png paper.ogg"
				<< " 169 Numeric_Code icon_paper.png paper.ogg"
				<< " 170 Numeric_Code icon_paper.png paper.ogg"
				<< " 171 Numeric_Code icon_paper.png paper.ogg"
				<< " 172 Numeric_Code icon_paper.png paper.ogg"
				<< " 173 Numeric_Code icon_paper.png paper.ogg"
				<< " 174 Numeric_Code icon_paper.png paper.ogg"
				<< " 175 Numeric_Code icon_paper.png paper.ogg"
				<< " 176 Numeric_Code icon_paper.png paper.ogg"
				<< " 177 Numeric_Code icon_paper.png paper.ogg"
				<< " 178 Numeric_Code icon_paper.png paper.ogg"
				<< " 179 Numeric_Code icon_paper.png paper.ogg"
				<< " 180 Numeric_Code icon_paper.png paper.ogg"
				<< " 181 Iron_Key Iron_key.png key_picking.ogg"
				<< " 182 Iron_Key Iron_key.png key_picking.ogg"
				<< " 183 Iron_Key Iron_key.png key_picking.ogg";
				
			this->items.clear();

			while(!stream.eof()) {
				int id;
				std::string name, icon, soundFilename;
				stream >> id >> name >> icon >> soundFilename;
				replace_all(name, "_", " ");
				SoundPtr itemSound = Core::getInstance()->createSound(soundFilename);
				itemSound->setRepeat(false);
				ItemDataPtr itemData(new ItemData(name, icon, itemSound));
				this->items[id] = itemData;
			}
			SoundPtr keyPickingSound = Core::getInstance()->createSound("key_picking.ogg");
			this->invalidItem = ItemDataPtr(new ItemData("Invalid Item", "icon_key.png", keyPickingSound));
		}

		ItemDataPtr getItemData(int id) {
			ItemDataPtrMap::const_iterator entry = this->items.find(id);
			if (entry == this->items.end()) {
				return this->invalidItem;
			}
			return entry->second;
		}

		virtual ~ItemDatabase() {}

	protected:
		ItemDataPtrMap items;
		ItemDataPtr invalidItem;

	private:
		static ItemDatabasePtr instance;

		ItemDatabase() {}
	};

	typedef ItemDatabase::ItemDatabasePtr ItemDatabasePtr;

	ItemDatabasePtr ItemDatabase::instance = ItemDatabasePtr();
}

#endif