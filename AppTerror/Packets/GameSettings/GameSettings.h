#ifndef TERROR_GAME_SETTINGS_H
#define TERROR_GAME_SETTINGS_H

#include "StageDatabase/StageDatabase.h"
#include "Monsters/Monster.h"
#include "Packets/PacketsEnum.h"

namespace terror {
	enum CharacterSelectionMode {
		SELECT_CHARACTERS,
		ALL_RANDOM_CHARACTERS
	};

	std::string toCharacterSelectionModeText(CharacterSelectionMode characterSelectionMode) {
		switch (characterSelectionMode) {
			case SELECT_CHARACTERS:
				return "Selected";
			case ALL_RANDOM_CHARACTERS:
				return "All Random";
			default:
				return "Unknown";
		}
	}

	CharacterSelectionMode toCharacterSelectionMode(const std::string& characterSelectionModeText) {
		if (characterSelectionModeText == "Selected") {
			return SELECT_CHARACTERS;
		}
		if (characterSelectionModeText == "All Random") {
			return ALL_RANDOM_CHARACTERS;
		}
		throw runtime_error("Unknown Character Selection Mode");
	}

	enum VoiceChatMode {
		VOIP_3D_POSITIONAL,
		VOIP_SIMPLE,
		VOIP_DISABLED
	};

	std::string toVoiceChatModeText(VoiceChatMode voiceChatMode) {
		switch (voiceChatMode) {
			case VOIP_3D_POSITIONAL:
				return "3D Attenuated";
			case VOIP_SIMPLE:
				return "Normal";
			case VOIP_DISABLED:
				return "Disabled";
			default:
				return "Unknown";
		}
	}

	VoiceChatMode toVoiceChatMode(const std::string& voiceChatModeText) {
		if (voiceChatModeText == "3D Attenuated") {
			return VOIP_3D_POSITIONAL;
		}
		if (voiceChatModeText == "Normal") {
			return VOIP_SIMPLE;
		}
		if (voiceChatModeText == "Disabled") {
			return VOIP_DISABLED;
		}
		throw runtime_error("Unknown Voice Chat Mode");
	}

	enum StageSelectionMode {
		RANDOM,
		CHOOSE_ONE
	};

	std::string toStageSelectionModeText(StageSelectionMode stageSelectionMode) {
		switch (stageSelectionMode) {
			case RANDOM:
				return "Random";
			case CHOOSE_ONE:
				return "Choose one";
			default:
				return "Unknown";
		}
	}

	StageSelectionMode toStageSelectionMode(const std::string& stageSelectionModeText) {
		if (stageSelectionModeText == "Random") {
			return RANDOM;
		}
		if (stageSelectionModeText == "Choose one") {
			return CHOOSE_ONE;
		}
		throw runtime_error("Unknown stage selection mode");
	}

	enum GameModeType {
		NORMAL_MODE,
		LIFE_MODE
	};

	std::string toGameModeText(GameModeType gameModeType) {
		switch (gameModeType) {
			case NORMAL_MODE:
				return "Normal mode";
			case LIFE_MODE:
				return "Life mode";
			default:
				return "Unknown";
		}
	}

	GameModeType toGameMode(const std::string& gameModeText) {
		if (gameModeText == "Normal mode") {
			return NORMAL_MODE;
		}
		if (gameModeText == "Life mode") {
			return LIFE_MODE;
		}
		throw runtime_error("Unknown game mode");
	}

	typedef std::vector<bool> BoolVector;

	class GameSettings : public Sendable {
	public:
		friend class boost::serialization::access;
		typedef SharedPointer<GameSettings> GameSettingsPtr;

		static GameSettingsPtr getInstance() {
			if (!GameSettings::instance) {
				GameSettings::instance = GameSettingsPtr(new GameSettings);
			}
			return GameSettings::instance;
		}

		virtual int getLabel() const {
			return GAME_SETTINGS;
		}

		void connectUpdatedEvent(const BasicSignal::slot_type& updatedEvent) {
			this->updatedSignal.connect(updatedEvent);
		}

		void setStagesIds(const IntVector& stagesIds) {
			this->stagesIds = stagesIds;
		}

		IntVector& getStagesIds() {
			return this->stagesIds;
		}

		BoolVector& getMonsters() {
			return this->monsters;
		}

		void setVoiceChatMode(const std::string& voiceChatModeText) {
			this->voiceChatMode = toVoiceChatMode(voiceChatModeText);
		}

		VoiceChatMode getVoiceChatMode() {
			return this->voiceChatMode;
		}

		void setCharacterSelectionMode(const std::string& characterSelectionModeText) {
			this->characterSelectionMode = toCharacterSelectionMode(characterSelectionModeText);
		}

		CharacterSelectionMode getCharacterSelectionMode() {
			return this->characterSelectionMode;
		}

		void setStageSelectionMode(const std::string& stageSelectionModeText) {
			this->stageSelectionMode = toStageSelectionMode(stageSelectionModeText);
		}

		StageSelectionMode getStageSelectionMode() {
			return this->stageSelectionMode;
		}

		void setNumberOfLives(const std::string& numberOfLivesText) {
			this->numberOfLives = lexical_cast<int>(numberOfLivesText);
		}

		int getNumberOfLives() {
			return this->numberOfLives;
		}

		void allowSpectator() {
			this->spectator = true;
		}

		void denySpectator() {
			this->spectator = false;
		}

		bool isSpectatorAllowed() {
			return this->spectator;
		}

		void allowInsanity() {
			this->insanity = true;
		}

		void denyInsanity() {
			this->insanity = false;
		}

		bool isInsanityAllowed() {
			return this->insanity;
		}

		void setGameMode(const std::string& gameModeText) {
			this->gameModeType = toGameMode(gameModeText);
		}

		GameModeType getGameModeType() {
			return this->gameModeType;
		}

		void notifyUpdate() {
			this->updatedSignal();
		}

		void reset() {
			this->characterSelectionMode = SELECT_CHARACTERS;
			this->voiceChatMode = VOIP_SIMPLE;
			this->stageSelectionMode = RANDOM;
			this->spectator = true;
			this->insanity = true;
			this->gameModeType = NORMAL_MODE;
			this->numberOfLives = 1;

			this->monsters.resize(N_MONSTERS, true);
			for (int monster = 0; monster < N_MONSTERS; monster++) {
				this->monsters[monster] = true;
			}

			StageDatabasePtr stageDatabase = StageDatabase::getInstance();
			StringListPtr stagesNames = stageDatabase->getStagesNames();
			this->stagesIds.clear();
			foreach (std::string& stageName, *stagesNames) {
				this->stagesIds.push_back(stageDatabase->getStageId(stageName));				
			}
		}

		virtual ~GameSettings() {}

	protected:
		BasicSignal updatedSignal;
		IntVector stagesIds;
		BoolVector monsters;
		CharacterSelectionMode characterSelectionMode;
		StageSelectionMode stageSelectionMode;
		VoiceChatMode voiceChatMode;
		GameModeType gameModeType;
		int numberOfLives;
		bool spectator;
		bool insanity;

	private:
		static GameSettingsPtr instance;

		GameSettings() {
			this->reset();
		}

		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->stagesIds;
			archive & this->monsters;
			archive & this->characterSelectionMode;
			archive & this->voiceChatMode;
			archive & this->stageSelectionMode;
			archive & this->gameModeType;
			archive & this->spectator;			
			archive & this->numberOfLives;
			archive & this->insanity;
			this->updatedSignal();
		}
	};

	typedef GameSettings::GameSettingsPtr GameSettingsPtr;

	GameSettingsPtr GameSettings::instance = GameSettingsPtr();
}

#endif