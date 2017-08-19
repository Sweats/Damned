#ifndef APP_TERROR_COMPONENTS_H
#define APP_TERROR_COMPONENTS_H

#include "MessageSystem/MessageSystem.h"
#include "CommonMessages.h"

namespace terror {
	class StartTransformMessage : public entities::Message {
	public:
		StartTransformMessage() {}

		virtual ~StartTransformMessage() {}
	};
	typedef SharedPointer<StartTransformMessage> StartTransformMessagePtr;

	class PovComponent : public entities::Component {
	public:
		PovComponent(PovType povType) : povType(povType) {}

		PovType getPovType() {
			return this->povType;
		}

		void setPovType(PovType povType) {
			this->povType = povType;
			PovChangedMessagePtr povChangedMessage(new PovChangedMessage(this->povType));
			this->getOwner()->queueMessage(povChangedMessage);
		}

		virtual ~PovComponent() {}

	private:
		PovType povType;
	};
	typedef SharedPointer<PovComponent> PovComponentPtr;	

	class StatueStateComponent : public entities::Component {
	public:
		//StatueStateComponent(int trapsToBecomeSeeker) : trapsToBecomeSeeker(randomNumber(9, 12)), trapsArmed(0), state(TRAPPER) {}
		//Tentative change, Original: randomNumber(9, 12)
		StatueStateComponent(int trapsToBecomeSeeker) : trapsToBecomeSeeker(10), trapsArmed(0), state(TRAPPER) {}

		StatueState getState() {
			return this->state;
		}

		void setState(StatueState state) {
			this->state = state;
			if (this->state == TRAPPER) {
				this->trapsArmed = 0;
				//this->trapsToBecomeSeeker = randomNumber(9, 12);
				//Tentative change, Original: randomNumber(9, 12)
				this->trapsToBecomeSeeker = 10;
			}

			StateChangedMessagePtr message(new StateChangedMessage(this->state));
			this->getOwner()->queueMessage(message);
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<TrapArmedMessage>()) {
				++this->trapsArmed;
				if (this->trapsArmed >= this->trapsToBecomeSeeker) {
					this->setState(SEEKER);
				}
			}
		}

		virtual ~StatueStateComponent() {}

	private:
		StatueState state;
		int trapsToBecomeSeeker;
		int trapsArmed;
	};
	typedef SharedPointer<StatueStateComponent> StatueStateComponentPtr;	

	class StatueMusicsAndSoundsComponent : public entities::Component {
	public:
		StatueMusicsAndSoundsComponent(ActorPtr actor) : actor(actor), p3_active(false) {
			this->addDependency<PovComponent>();
			this->addDependency<StatueStateComponent>();
		}

		virtual void initialize() {
			this->povComponent = this->getOwner()->getComponent<PovComponent>();
			this->stateComponent = this->getOwner()->getComponent<StatueStateComponent>();

			this->statueMusic = Core::getInstance()->createSound("statue_music.ogg", "AmbientMusic");
			this->statueMusic->setRepeat(true);

			this->statueKillMusic = Core::getInstance()->createSound("statue_kill_mode.ogg", "AmbientMusic");
			this->statueKillMusic->setRepeat(true);

			this->survivorMusic = Core::getInstance()->createSound("statue_survivor_kill.ogg", "AmbientMusic");
			this->survivorMusic->setRepeat(true);

			this->snarlSound = this->actor->getSoundSource()->createSound("monster_snarl.ogg");

			this->killSound = this->actor->getSoundSource()->createSound("statue_near.ogg");
			this->killSound->setRepeat(true);
		}

		virtual void reset() {
			this->statueMusic->stop();
			this->statueKillMusic->stop();
			this->survivorMusic->stop();
			this->killSound->stop();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			std::type_index messageIndex = hashByType(message);
			if (messageIndex == hashByType<DestroyerEndedMessage>()) {
				this->p3_active = false;
			}
			if (messageIndex == hashByType<StartTransformMessage>()) {
				this->snarlSound->play();
			}
			if (messageIndex == hashByType<PovChangedMessage>() || messageIndex == hashByType<StateChangedMessage>()) {
				this->handleSounds();
			}
		}

		virtual ~StatueMusicsAndSoundsComponent() {}

	private:
		ActorPtr actor;
		PovComponentPtr povComponent;
		StatueStateComponentPtr stateComponent;
		SoundPtr statueMusic;
		SoundPtr statueKillMusic;
		SoundPtr survivorMusic;
		SoundPtr snarlSound;
		SoundPtr killSound;
		bool p3_active;

	private:
		void handleSounds() {
			StatueState state = this->stateComponent->getState();
			if (state == TRAPPER && this->p3_active == false) {
				if (this->povComponent->getPovType() == FIRST_PERSON) {
					this->statueKillMusic->stop();
					this->survivorMusic->stop();
					this->statueMusic->play();
				}
				if (this->povComponent->getPovType() == THIRD_PERSON) {
					this->statueKillMusic->stop();
					this->survivorMusic->stop();
					this->statueMusic->stop();					
				}
			}
			if (state == SEEKER) {
				if (this->povComponent->getPovType() == THIRD_PERSON) {
					this->killSound->play();
				}
			}
			if (state == DESTROYER && this->p3_active == false) {
				this->p3_active = true;
				if (this->povComponent->getPovType() == FIRST_PERSON) {
					this->statueMusic->stop();
					this->survivorMusic->stop();
					this->statueKillMusic->play();
				}
				if (this->povComponent->getPovType() == THIRD_PERSON) {
					this->statueKillMusic->stop();
					this->statueMusic->stop();
					this->survivorMusic->play();					
					this->killSound->stop();
				}
			}
		}
	};
	typedef SharedPointer<StatueMusicsAndSoundsComponent> StatueMusicsAndSoundsComponentPtr;
	
	class ActorViewComponent : public entities::Component {
	public:
		ActorViewComponent(ActorPtr actor) : actor(actor) {
			PostProcessingSystemPtr postProcessingSystem = Core::getInstance()->getGraphicsSystem()->getPostProcessingSystem();
			this->trapView = postProcessingSystem->addPostProcessingEffect("TrapperView");
			this->trapView->setEnabled(false);

			this->seekerView = postProcessingSystem->addPostProcessingEffect("SeekerView");
			this->seekerView->setEnabled(false);

			this->destroyerView = postProcessingSystem->addPostProcessingEffect("DestroyerView");
			this->destroyerView->setEnabled(false);

			this->addDependency<PovComponent>();
			this->addDependency<StatueStateComponent>();
		}

		virtual void initialize() {
			this->povComponent = this->getOwner()->getComponent<PovComponent>();
			this->stateComponent = this->getOwner()->getComponent<StatueStateComponent>();
			this->handleActorView();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<PovChangedMessage>() || hashByType(message) == hashByType<StateChangedMessage>()) {
				this->handleActorView();
			}
		}

		virtual void reset() {
			this->trapView->setEnabled(false);
			this->seekerView->setEnabled(false);
			this->destroyerView->setEnabled(false);
		}

		virtual ~ActorViewComponent() {}

	private:
		PovComponentPtr povComponent;
		StatueStateComponentPtr stateComponent;
		ActorPtr actor;

		PostProcessingEffectPtr trapView;
		PostProcessingEffectPtr seekerView;
		PostProcessingEffectPtr destroyerView;

	private:
		void handleActorView() {
			if (this->povComponent->getPovType() == FIRST_PERSON) {
				this->setFirstPersonView();
			}else {
				this->setThirdPersonView();
			}
		}

		void setFirstPersonView() {
			StatueState state = this->stateComponent->getState();
			switch (state) {
			case TRAPPER:
				writeMessage("Arm traps to generate statue clones and gather energy");
				this->trapView->setEnabled(true);
				this->seekerView->setEnabled(false);
				this->destroyerView->setEnabled(false);
				this->actor->show();
				break;

			case SEEKER:
				writeMessage("Seek humans to kill");
				this->trapView->setEnabled(false);
				this->seekerView->setEnabled(true);
				this->destroyerView->setEnabled(false);
				this->actor->show();
				break;

			case DESTROYER:
				writeMessage("Kill them all!");
				this->trapView->setEnabled(false);
				this->seekerView->setEnabled(false);
				this->destroyerView->setEnabled(true);
				this->actor->show();
				break;
			}
		}

		void setThirdPersonView() {
			StatueState state = this->stateComponent->getState();
			switch (state) {
			case TRAPPER:
				this->actor->hide();
				break;

			case SEEKER:
				this->actor->hide();
				break;

			case DESTROYER:
				this->actor->show();
				break;
			}
		}
	};
	typedef SharedPointer<ActorViewComponent> ActorViewComponentPtr;	

	class StageComponent : public entities::Component {
	public:
		StageComponent(const string& stageName) : stageName(stageName) {}

		bool isForestStage() {
			return (this->stageName == "Black_Lake_Woods");
		}

		string getStageName() {
			return this->stageName;
		}

		virtual ~StageComponent() {}

	private:
		string stageName;
	};
	typedef SharedPointer<StageComponent> StageComponentPtr;

	class StatueSpeedComponent : public entities::Component {
	public:
		StatueSpeedComponent(MovePtr walk) : walk(walk) {}

		virtual void initialize() {
			this->stageComponent = this->getOwner()->getComponent<StageComponent>();

			this->trapperSpeedMultiplier = 1.0;
			if (this->stageComponent && this->stageComponent->isForestStage()) {
				this->seekerSpeedMultiplier = 3.5f/3.25f;
				this->destroyerSpeedMultiplier = 2.75f/3.25f;
				this->transformSpeedMultiplier = 2.0f/3.25f;
			} else if (this->stageComponent && this->stageComponent->getStageName() == "Factory") {
				this->seekerSpeedMultiplier = 3.0f/2.75f;
				this->destroyerSpeedMultiplier = 2.55f/2.75f;
				this->transformSpeedMultiplier = 2.0f/2.75f;
			}else {
				this->seekerSpeedMultiplier = 3.0f/2.55f;
				this->destroyerSpeedMultiplier = 2.55f/2.55f;
				this->transformSpeedMultiplier = 2.0f/2.55f;
			}
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				switch (stateChangedMessage->getCurrentState()) {
				case TRAPPER:
					this->walk->setSpeedMultiplier(this->trapperSpeedMultiplier);					
					break;

				case SEEKER:
					this->walk->setSpeedMultiplier(this->seekerSpeedMultiplier);
					break;

				case DESTROYER:
					this->walk->setSpeedMultiplier(this->destroyerSpeedMultiplier);
					break;
				}
			}

			if (hashByType(message) == hashByType<StartTransformMessage>()) {
				this->walk->setSpeedMultiplier(this->transformSpeedMultiplier);
			}
		}

		virtual ~StatueSpeedComponent() {}

	private:
		StageComponentPtr stageComponent;
		MovePtr walk;
		float trapperSpeedMultiplier;
		float seekerSpeedMultiplier;
		float transformSpeedMultiplier;
		float destroyerSpeedMultiplier;
	};
	typedef SharedPointer<StatueSpeedComponent> StatueSpeedComponentPtr;

	class StatuePhysicsComponent : public entities::Component {
	public:
		StatuePhysicsComponent(ActorPtr actor) : actor(actor) {}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				switch (stateChangedMessage->getCurrentState()) {
				case TRAPPER: case SEEKER:
					this->actor->enableGhostMode();
					break;

				case DESTROYER:
					this->actor->disableGhostMode();
					break;
				}
			}
		}

		CollisionObjectPtr getCollisionObject() {
			return this->actor->getCollisionObject();
		}

		Vector3d getPosition() {
			return this->actor->getPosition();
		}

		virtual ~StatuePhysicsComponent() {}

	private:
		ActorPtr actor;
	};
	typedef SharedPointer<StatuePhysicsComponent> StatuePhysicsComponentPtr;	
}

#endif