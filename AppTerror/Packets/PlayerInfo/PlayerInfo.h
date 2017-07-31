#ifndef TERROR_PLAYER_INFO_H
#define TERROR_PLAYER_INFO_H

#include "Packets/CharSelectionRequest/CharSelectionRequest.h"

namespace terror {
	class PlayerInfo {
	public:
		friend class boost::serialization::access;

		PlayerInfo() : name(""), character(CHAR_NONE), spawnPointId(0), playerImagebox(0), playerNameTextbox(0) {}

		PlayerInfo(MyGUI::ImageBox* playerImagebox, MyGUI::TextBox* playerNameTextbox) 
			: name(""), character(CHAR_NONE), spawnPointId(0), playerImagebox(playerImagebox), playerNameTextbox(playerNameTextbox) 
		{
			this->updateGuiElements();
		}

		void setName(const string& name) {
			this->name = name;
		}

		void setCharacter(CharacterType character) {
			this->character = character;
		}

		void setSpawnPointId(int spawnPointId) {
			this->spawnPointId = spawnPointId;
		}

		void setUdpAddress(UdpAddressPtr udpAddress) {
			this->udpAddress = udpAddress;
		}

		void resetUdpAddress() {
			this->udpAddress.reset();
		}

		void updateGuiElements() {
			this->playerNameTextbox->setCaption(name);
			if (this->character == CHAR_NONE) {
				this->playerImagebox->setImageTexture("no_char.png");
			}else if (this->character == CHAR_RANDOM) {
				this->playerImagebox->setImageTexture("char_random.png");
			}else if (this->character == CHAR_0) {
				this->playerImagebox->setImageTexture("char_0.png");
			}else if (this->character == CHAR_1) {
				this->playerImagebox->setImageTexture("char_1.png");
			}else if (this->character == CHAR_2) {
				this->playerImagebox->setImageTexture("char_2.png");
			}else if (this->character == CHAR_3) {
				this->playerImagebox->setImageTexture("char_3.png");
			}else if (this->character == CHAR_MONSTER_0) {
				this->playerImagebox->setImageTexture("char_monster_0.png");
			}else if (this->character == CHAR_MONSTER_1) {
				this->playerImagebox->setImageTexture("char_monster_1.png");
			}else if (this->character == CHAR_MONSTER_2) {
				this->playerImagebox->setImageTexture("char_monster_2.png");
			}else if (this->character == CHAR_MONSTER_3) {
				this->playerImagebox->setImageTexture("char_monster_3.png");
			}
		}

		string getName() {
			return this->name;
		}

		CharacterType getCharacter() {
			return this->character;
		}

		int getSpawnPointId() {
			return this->spawnPointId;
		}

		UdpAddressPtr getUdpAddress() {
			return this->udpAddress;
		}

		virtual ~PlayerInfo() {}

	protected:		
		string name;
		CharacterType character;
		int spawnPointId;
		UdpAddressPtr udpAddress;
		MyGUI::ImageBox* playerImagebox;
		MyGUI::TextBox* playerNameTextbox;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->name;
			archive & this->character;
			archive & this->spawnPointId;
		}
	};

	typedef SharedPointer<PlayerInfo> PlayerInfoPtr;
}

#endif