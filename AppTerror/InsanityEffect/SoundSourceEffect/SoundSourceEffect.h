#ifndef TERROR_SOUND_SOURCE_EFFECT_H
#define TERROR_SOUND_SOURCE_EFFECT_H

#include "InsanityEffect/InsanityEffect.h"
#include "CharacterHandler/CharacterHandler.h"

namespace terror {
	typedef IntPointerMap<Sound> IntSoundPtrMap;

	class SoundSourceEffect : public InsanityEffect {
	public:
		SoundSourceEffect(SoundSourcePtr soundSource, CharacterHandlerPtr characterHandler, TerrorClientPtr terrorClient)
			: InsanityEffect(50.0f, 90.0f, 1.0f, 800.0f), soundSource(soundSource), characterHandler(characterHandler), terrorClient(terrorClient), 
			extensionPeeker(new ExtensionPeeker("./DamnedData/Resources/Sounds/Traps/", ".ogg"))
		{
			this->initializeSounds();
		}

		void addSound(string soundName) {
			int id = this->sounds.size();
			this->sounds[id] = this->soundSource->createSound(soundName);
			this->sounds[id]->setRepeat(false);
		}

		virtual ~SoundSourceEffect(){}

	protected:
		SoundSourcePtr soundSource;
		CharacterHandlerPtr characterHandler;
		TerrorClientPtr terrorClient;
		IntSoundPtrMap sounds;
		ExtensionPeekerPtr extensionPeeker;
		int quadrant;
		Vector3d quadrantVector0;
		Vector3d quadrantVector1;


	private:
		virtual void doStartExecution() {
			if (this->sounds.size() <= 0) {
				return;
			}
			this->placeSoundSource();
			int soundId = randomNumber(0, this->sounds.size() - 1);
			this->sounds[soundId]->play(); 
		}

		virtual void doExecute(float diffTime) {}

		virtual void doEndExecution() {}

		void initializeSounds() {
			StringListPtr soundNames = this->extensionPeeker->peek();
			foreach(string soundName, *soundNames){
				this->addSound(soundName);
			}
		}

		void placeSoundSource() {
			HumanPtr localHuman = this->getLocalHuman();
			if (!localHuman) {
				return;
			}

			float placeCoefficient = this->computePlaceCoefficient();
			this->findQuadrantVectors(placeCoefficient);
			Vector3d direction = this->computeSoundSourceDirection(placeCoefficient);
			float distance = this->computeDistance();

			Vector3d humanPosition = localHuman->getActor()->getPosition();
			this->soundSource->setPosition(humanPosition + distance*direction);
		}

		HumanPtr getLocalHuman() {
			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo) {
				throw std::runtime_error("Tried to retrieve an invalid local player.");
			}
			CharacterType thisChar = playerInfo->getCharacter();
			return this->characterHandler->getHumanAlive(thisChar);
		}

		float computePlaceCoefficient() {
			return randomNumber(0.0f, 4.0f);
		}

		void findQuadrantVectors(float placeCoefficient) {
			this->quadrant = static_cast<int>(placeCoefficient);
			switch (this->quadrant) {
				case 0:
					this->quadrantVector0 = Vector3d::zUnit;
					this->quadrantVector1 = Vector3d::xUnit;
					break;
				case 1:
					this->quadrantVector0 = Vector3d::xUnit;
					this->quadrantVector1 = -1*Vector3d::zUnit;
					break;
				case 2:
					this->quadrantVector0 = -1*Vector3d::zUnit;
					this->quadrantVector1 = -1*Vector3d::xUnit;
					break;
				case 3:
					this->quadrantVector0 = -1*Vector3d::xUnit;
					this->quadrantVector1 = Vector3d::zUnit;
					break;
				default:
					this->quadrantVector0 = Vector3d::zUnit;
					this->quadrantVector1 = Vector3d::xUnit;
			}
		}

		Vector3d computeSoundSourceDirection(float placeCoefficient) {
			float interpolatorFactor = placeCoefficient - (float)this->quadrant;
			return slerp(this->quadrantVector0, this->quadrantVector1, interpolatorFactor);
		}

		float computeDistance() {
			return randomNumber(1.0f, 6.0f);
		}
	};

	typedef SharedPointer<SoundSourceEffect> SoundSourceEffectPtr;
}

#endif