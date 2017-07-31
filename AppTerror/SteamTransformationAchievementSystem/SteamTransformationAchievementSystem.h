#ifndef TERROR_STEAM_TRANSFORMATION_ACHIEVEMENTS_SYSTEM_H
#define TERROR_STEAM_TRANSFORMATION_ACHIEVEMENTS_SYSTEM_H

#include "Packets/PlayersState/PlayersState.h"
#include "CharacterLoader/CharacterLoader.h"

namespace terror {
	class SteamTransformationAchievementsSystem {
	public:
		SteamTransformationAchievementsSystem(TerrorClientPtr terrorClient) 
			: terrorClient(terrorClient), firstTransformationStarted(false), firstTransformationEnded(false), monsterIsLocal(false) {}

		void notifyStageUnloaded() {
			this->firstTransformationStarted = false;
			this->firstTransformationEnded = false;
			this->monsterIsLocal = false;
			this->monsterReturnedToNormalStateConnection.disconnect();
			this->charactersDeadInATransformation.clear();
		}

		void notifyLocalPlayerMonsterLoaded(MonsterPtr monster) {
			this->monsterIsLocal = true;
			if (monster->getType() == SPECTRUM) {
				this->monsterTransformedConnection = 
					static_ptr_cast<Spectrum>(monster)->connectBecomeVisibleEvent(Bind(&SteamTransformationAchievementsSystem::notifyMonsterTransformed, this));
				this->monsterReturnedToNormalStateConnection = 
					static_ptr_cast<Spectrum>(monster)->connectBecomeInvisibleEvent(Bind(&SteamTransformationAchievementsSystem::notifyMonsterReturnedToNormalState, this));
			}else if (monster->getType() == BERSERKER) {
				this->monsterTransformedConnection = 
					static_ptr_cast<Berserker>(monster)->connectBecomeBerserkerEvent(Bind(&SteamTransformationAchievementsSystem::notifyMonsterTransformed, this));
				this->monsterReturnedToNormalStateConnection = 
					static_ptr_cast<Berserker>(monster)->connectBecomeGhostEvent(Bind(&SteamTransformationAchievementsSystem::notifyMonsterReturnedToNormalState, this));
			}
		}

		void notifyHumanDeath(HumanPtr human) {
			if (!this->monsterIsLocal) {
				return;
			}
			int maxNumberOfCharacters = 4;
			this->charactersDeadInATransformation.insert(human->getCharacterType());
			if (this->firstTransformationStarted && !this->firstTransformationEnded) {
				SteamSystem::getInstance()->setAchievement("no_fun_allowed");
			}
			if (this->charactersDeadInATransformation.size() == maxNumberOfCharacters) {
				SteamSystem::getInstance()->setAchievement("berserker");
			}
		}

		virtual ~SteamTransformationAchievementsSystem() {}

	protected:
		TerrorClientPtr terrorClient;
		Connection monsterTransformedConnection;
		Connection monsterReturnedToNormalStateConnection;
		bool firstTransformationStarted;
		bool firstTransformationEnded;
		bool monsterIsLocal;
		std::set<CharacterType> charactersDeadInATransformation;

	private:
		void notifyMonsterReturnedToNormalState() {
			this->firstTransformationEnded = true;
			this->charactersDeadInATransformation.clear();
		}

		void notifyMonsterTransformed() {
			if (!this->firstTransformationStarted) {
				this->firstTransformationStarted = true;
				this->firstTransformationEnded = false;
			}
		}
	};

	typedef SharedPointer<SteamTransformationAchievementsSystem> SteamTransformationAchievementsSystemPtr;
}

#endif