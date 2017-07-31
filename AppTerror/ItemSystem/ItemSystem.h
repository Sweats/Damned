#ifndef TERROR_ITEM_SYSTEM_H
#define TERROR_ITEM_SYSTEM_H

#include "Packets/Packets.h"
#include "CharacterHandler/CharacterHandler.h"
#include "MessageSystem/MessageSystem.h"

namespace terror {
	class ItemSystem : public NetworkObserver {
	public:
		ItemSystem(CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient) 
			: NetworkObserver(false), characterHandler(characterHandler), terrorClient(terrorClient) {}

		void notifyStageLoaded(StagePtr stage) {
			this->stage = stage;
			ObjectPtrListPtr items = this->stage->getObjectsWithMaskBetween(1, 499);
			foreach (ObjectPtr item, *items) {
				item->setMaterialId(10);
			}
			ObjectPtrListPtr batteries = this->stage->getObjectsWithMask(FLASHLIGHT_BATTERY);
			foreach (ObjectPtr battery, *batteries) {
				battery->setMaterialId(10);
			}
		}

		void notifyStageUnloaded() {
			this->stage.reset();
		}

		void requestAddItem(CharacterType character, int objectId) {
			if (!this->stage) {
				return;
			}

			ObjectPtr object = this->stage->getObject(objectId);
			if (!object) {
				return;
			}

			int itemMask = object->getMask();
			if (itemMask == FLASHLIGHT_BATTERY) {
				HumanPtr human = this->characterHandler->getHumanAlive(character);
				if (human && human->getFlashlightBatteryCharge() > 0.75f) {
					PlayersStatePtr playersState = PlayersState::getInstance();
					PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
					if (localPlayerInfo && localPlayerInfo->getCharacter() == character) {
						writeMessage("Your flashlight is already charged!");
					}					
					return;
				}				
			}

			if (!this->terrorClient->isHost()) {
				AddItemRequestPtr addItemRequest(new AddItemRequest(character, objectId));
				this->terrorClient->sendObjectToHost(addItemRequest, true);
				return;
			}

			if (!this->addItemToHuman(character, objectId)) {
				return;
			}

			AddItemPtr addItem(new AddItem(character, objectId));
			this->terrorClient->sendObjectToPlayers(addItem, true);
		}

		void requestAddItemsFromDead(CharacterType character, int deadId) {
			if (!this->stage) {
				return;
			}

			if (!this->terrorClient->isHost()) {
				AddDeadItemsRequestPtr addDeadItemsRequest(new AddDeadItemsRequest(character, deadId));
				this->terrorClient->sendObjectToHost(addDeadItemsRequest, true);
				return;
			}

			this->addItemsFromDead(character, deadId);

			AddDeadItemsPtr addDeadItems(new AddDeadItems(character, deadId));
			this->terrorClient->sendObjectToPlayers(addDeadItems, true);
		}

		virtual ~ItemSystem() {}

	protected:
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		StagePtr stage;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (!this->stage) {
				return;
			}

			// Host
			if (udpPacket->getLabel() == ADD_ITEM_REQUEST) {
				AddItemRequestPtr addItemRequest(new AddItemRequest);
				if (!udpPacket->buildObject(addItemRequest)) {
					return;
				}

				if (!this->addItemToHuman(addItemRequest->getCharacter(), addItemRequest->getObjectId())) {
					return;
				}

				AddItemPtr addItem(new AddItem(addItemRequest->getCharacter(), addItemRequest->getObjectId()));
				this->terrorClient->sendObjectToPlayers(addItem, true);
			}
			if (udpPacket->getLabel() == ADD_DEAD_ITEMS_REQUEST) {
				AddDeadItemsRequestPtr addDeadItemsRequest(new AddDeadItemsRequest);
				if (!udpPacket->buildObject(addDeadItemsRequest)){
					return;
				}

				this->addItemsFromDead(addDeadItemsRequest->getCharacter(), addDeadItemsRequest->getDeadId());

				AddDeadItemsPtr addDeadItems(new AddDeadItems(addDeadItemsRequest->getCharacter(), addDeadItemsRequest->getDeadId()));
				this->terrorClient->sendObjectToPlayers(addDeadItems, true);
			}
			if (udpPacket->getLabel() == PLAYER_JOINED_GAME) {
				CharactersItemsPtr charactersItems(new CharactersItems);
				HumanPtrMap& humans = characterHandler->getHumansAlive();
				foreach (HumanPtrMap::value_type& entry, humans) {
					HumanPtr human = entry.second;
					CharacterItemsPtr characterItems(new CharacterItems(human->getCharacterType()));

					ItemPtrMap& items = human->getInventory()->getItems();
					foreach (ItemPtrMap::value_type& entry, items) {
						ItemPtr item = entry.second;
						characterItems->addItemMask(item->getMask());
					}
					charactersItems->addCharacterItems(characterItems);
				}
				this->terrorClient->sendObjectTo(charactersItems, udpPacket->getRemoteUdpAddress(), true);
			}

			// Client
			if (udpPacket->getLabel() == ADD_ITEM) {
				AddItemPtr addItem(new AddItem);
				if (!udpPacket->buildObject(addItem)) {
					return;
				}

				this->addItemToHuman(addItem->getCharacter(), addItem->getObjectId());
			}
			if (udpPacket->getLabel() == ADD_DEAD_ITEMS) {
				AddDeadItemsPtr addDeadItems(new AddDeadItems);
				if (!udpPacket->buildObject(addDeadItems)) {
					return;
				}

				this->addItemsFromDead(addDeadItems->getCharacter(), addDeadItems->getDeadId());
			}
			if (udpPacket->getLabel() == CHARACTERS_ITEMS) {
				CharactersItemsPtr charactersItems(new CharactersItems);
				if (!udpPacket->buildObject(charactersItems)){
					return;
				}

				foreach (CharacterItemsPtr characterItems, charactersItems->getCharactersItems()) {
					foreach (int itemMask, characterItems->getItemMasks()) {						
						HumanPtr human = this->characterHandler->getHumanAlive(characterItems->getCharacter());
						if (human) {
							ItemPtr item(new Item(itemMask));
							human->addItem(item, false);
						}
					}
				}
			}
		}

		bool addItemToHuman(CharacterType character, int objectId) {			
			ObjectPtr object = this->stage->getObject(objectId);
			if (!object) {
				return false;
			}
			
			HumanPtr human = this->characterHandler->getHumanAlive(character);
			if (!human) {
				return false;
			}

			int itemMask = object->getMask();

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!localPlayerInfo) {
				return false;
			}

			if (human->hasItem(itemMask)) {
				if (localPlayerInfo->getCharacter() == character) {
					string message = "You already have this item";
					writeMessage(message);
				}
				return true;
			}

			this->writeItemMessage(character, itemMask);
			this->stage->destroyObject(object);

			CharacterType localCharacter = localPlayerInfo->getCharacter();

			if (itemMask == FLASHLIGHT_BATTERY) {
				if (character == localCharacter) {
					SteamSystem::getInstance()->incrementStat("batteries_picked", 1);
				}
				return this->treatFlashlightBattery(character);
			}
			
			ItemPtr item(new Item(itemMask));
			human->addItem(item);
			if (character == localCharacter) {
				string objectName = ItemDatabase::getInstance()->getItemData(itemMask)->getName();
				if (objectName == "Silver Key") {
					SteamSystem::getInstance()->setAchievement("pick_a_silver_key");
				}else if (objectName == "Hammer" || objectName == "Crowbar") {
					SteamSystem::getInstance()->setAchievement("pick_a_hammer_or_crowbar");
				}
				if (human->getInventory()->getItems().size() >= 10) {
					SteamSystem::getInstance()->setAchievement("hold_more_than_10_items");
				}
			}
			return true;
		}

		void addItemsFromDead(CharacterType character, int deadId) {
			HumanPtr human = this->characterHandler->getHumanAlive(character);
			if (!human) {
				return;
			}
			HumanPtr dead = this->characterHandler->getDeadHuman(deadId);
			if (!dead) {
				return;
			}
			ItemPtrMap& deadItems = dead->getInventory()->getItems();
			if (deadItems.empty()) {
				this->writeNoItemMessage(character);
			}

			PlayerInfoPtr playerInfo = PlayersState::getInstance()->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				return;
			}

			CharacterType localCharacter = playerInfo->getCharacter();			
			ItemDatabasePtr itemDatabase = ItemDatabase::getInstance();
			foreach (ItemPtrMap::value_type& entry, deadItems) {
				ItemPtr item = entry.second;				
				this->writeItemMessage(character, item->getMask());
				if (!human->hasItem(item->getMask())) {
					human->addItem(item);
					if (character == localCharacter) {
						std::string itemName = itemDatabase->getItemData(item->getMask())->getName();
						if (itemName == "Silver_Key") {
							SteamSystem::getInstance()->setAchievement("pick_a_silver_key");
						}else if (itemName == "Hammer" || itemName == "Crowbar") {
							SteamSystem::getInstance()->setAchievement("pick_a_hammer_or_crowbar");
						}
					}
				}
			}

			if (character == localCharacter && human->getInventory()->getItems().size() >= 10) {
				SteamSystem::getInstance()->setAchievement("hold_more_than_10_items");
			}

			dead->getInventory()->clear();
			dead->getActor()->setMask(-2);
			dead->getActor()->setMaterialId(0);
			dead->getActor()->disablePhysics();
		}

		void writeItemMessage(CharacterType character, int itemMask) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!localPlayerInfo || isMonsterCharacter(localPlayerInfo->getCharacter())) {
				return;
			}		

			ItemDatabasePtr itemDatabase = ItemDatabase::getInstance();
			ItemDataPtr itemData = itemDatabase->getItemData(itemMask);

			std::string message;
			if (localPlayerInfo->getCharacter() == character) {
				if (itemMask == FLASHLIGHT_BATTERY) {
					message = std::string("You got a battery");
				}else {
					message = std::string("You got a ") + itemData->getName();
				}
			}else if (itemMask != FLASHLIGHT_BATTERY){
				try {
					PlayerInfoPtr playerInfo = playersState->getPlayerInfoByCharacter(character);
					message = std::string("Player ") + playerInfo->getName() + std::string(" got a ") + itemData->getName();	
				}catch (std::exception&) {}			
			}
			writeMessage(message);
		}

		void writeNoItemMessage(CharacterType character) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!localPlayerInfo || isMonsterCharacter(localPlayerInfo->getCharacter())) {
				return;
			}

			std::string message;
			if (localPlayerInfo->getCharacter() == character) {
				message = std::string("You found no items with this dead body");
			}else {
				try {
					PlayerInfoPtr playerInfo = playersState->getPlayerInfoByCharacter(character);
					message = std::string("Player ") + playerInfo->getName() + std::string(" found no items with a dead body");
				}catch (std::exception&) {}
			}
			writeMessage(message);
		}

		bool treatFlashlightBattery(CharacterType character) {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr localPlayerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (localPlayerInfo && character == localPlayerInfo->getCharacter()) {
				HumanPtr human = this->characterHandler->getHumanAlive(character);
				human->addChargeToFlashlightBattery(1.0f);
			}
			return true;
		}
	};

	typedef SharedPointer<ItemSystem> ItemSystemPtr;	
}

#endif