#ifndef TERROR_STATUE_H
#define TERROR_STATUE_H

#include "Components/Components.h"
#include "Monsters/Monster.h"
#include "Monsters/Energy.h"
#include "StatueClonesSystem/StatueClonesSystem.h"

namespace terror {	
	class EntityUpdater : public EventHandler {
	public: 
		EntityUpdater(entities::EntityPtr entity) : entity(entity) {}

		virtual ~EntityUpdater() {}

	private:
		entities::EntityPtr entity;

	private:
		virtual bool handleEvent(float diffTime) {
			this->entity->update(diffTime);
			return true;
		}
	};
	typedef SharedPointer<EntityUpdater> EntityUpdaterPtr;

	class Statue : public Monster {
	public:
		Statue(CharacterDataPtr characterData, CharacterActionsPtr characterActions, MonsterActionsPtr monsterActions, StatueClonesSystemPtr statueClonesSystem, const string& stageName) 
			: Monster(characterData, characterActions, monsterActions), statueClonesSystem(statueClonesSystem), timeToUpdate(1.0f)
		{
			if (stageName == "Black_Lake_Woods") {
				this->statueClonesSystem->setMaxClones(16);
			}else {
				this->statueClonesSystem->setMaxClones(8);
			}

			this->animations.add(characterData->actor->getAnimation("idle"));
			this->animations.add(characterData->actor->getAnimation("walking"));
			this->animations.add(characterData->actor->getAnimation("walk45Right"));
			this->animations.add(characterData->actor->getAnimation("walk45Left"));
			this->animations.add(characterData->actor->getAnimation("walkStrafeRight"));
			this->animations.add(characterData->actor->getAnimation("walkStrafeLeft"));
			characterData->actor->enableGhostMode();
			{
				this->entity = entities::EntityPtr(new entities::Entity);

				PovComponentPtr pov(new PovComponent(THIRD_PERSON));
				this->entity->addComponent(pov);

				int trapsToBecomeSeeker = 16;
				StatueStateComponentPtr state(new StatueStateComponent(trapsToBecomeSeeker));
				this->entity->addComponent(state);

				ActorViewComponentPtr actorViewComponent(new ActorViewComponent(characterData->actor));
				this->entity->addComponent(actorViewComponent);

				StatueSpeedComponentPtr speedComponent(new StatueSpeedComponent(this->actions->walk));
				this->entity->addComponent(speedComponent);

				StatuePhysicsComponentPtr physicsComponent(new StatuePhysicsComponent(characterData->actor));
				this->entity->addComponent(physicsComponent);

				StageComponentPtr stageComponent(new StageComponent(stageName));
				this->entity->addComponent(stageComponent);

				StatueMusicsAndSoundsComponentPtr statueMusicsAndSoundsComponent(new StatueMusicsAndSoundsComponent(characterData->actor));
				this->entity->addComponent(statueMusicsAndSoundsComponent);

				this->entity->initializeComponents();

				this->updater = EntityUpdaterPtr(new EntityUpdater(this->entity));
			}
			Core::getInstance()->addEventHandler(this->updater, "Ingame");
			this->statueClonesSystem->setStatueFirstPersonEnabled(false);
		}

		void incrementTrapsArmed() {
			TrapArmedMessagePtr trapArmedMessage(new TrapArmedMessage);
			this->entity->queueMessage(trapArmedMessage);
		}

		void notifyHumanAdded(HumanPtr human) {
			HumanAddedMessagePtr humanAddedMessage(new HumanAddedMessage(human));
			this->entity->queueMessage(humanAddedMessage);
			this->statueClonesSystem->notifyHumanAdded(human);
		}

		void setPovType(PovType povType) {
			this->entity->getComponent<PovComponent>()->setPovType(povType);
			this->statueClonesSystem->setStatueFirstPersonEnabled(povType == FIRST_PERSON);
		}

		StatueState getState() {
			return this->entity->getComponent<StatueStateComponent>()->getState();
		}

		void setState(StatueState state) {
			return this->entity->getComponent<StatueStateComponent>()->setState(state);
		}

		entities::EntityPtr getEntity() {
			return this->entity;
		}

		virtual MonsterType getType() {
			return STATUE;
		}

		void cleanup() {
			Core::getInstance()->removeEventHandler(this->updater);
			this->entity->resetComponents();
		}

		virtual void updateStatueInfo(MonsterInfoPtr monsterInfo) {
			this->monsterInfo = monsterInfo;
			this->updateStatueStateInfo(monsterInfo->stateInfo);
			this->updateAdditionalInfo(monsterInfo->stateInfo);
		}

		virtual MonsterInfoPtr getInfo() {
			this->monsterInfo->stateInfo->setState(int(this->getState()));
			return this->monsterInfo;
		}

		virtual void updateInfo(MonsterActionRequestPtr monsterActionRequest) {
			this->monsterInfo->update(monsterActionRequest);
			this->updateStatueStateInfo(monsterActionRequest->stateInfo);
			this->updateAdditionalInfo(monsterActionRequest->stateInfo);
		}

		void setTimeToUpdate(float timeToUpdate) {
			this->timeToUpdate = timeToUpdate;
		}

		virtual ~Statue() {}

	protected:
		StatueClonesSystemPtr statueClonesSystem;
		EntityUpdaterPtr updater;
		entities::EntityPtr entity;
		AnimationPtrList animations;
		float timeToUpdate;

	private:
		void updateStatueStateInfo(CharacterStateInfoPtr stateInfo) {
			static TimerPtr animTimer;
			static TimerPtr timer;
			if (!timer) {
				timer = TimerPtr(new Timer);
				timer->start();

				animTimer = TimerPtr(new Timer);
				animTimer->start();
			}
			if (timer->getElapsed(SECONDS) < this->timeToUpdate) {
				return;
			}

			bool backwards = false;
			if (stateInfo->getFrontMove() < -1.0e-4f) {
				backwards = true;
			}
			bool forward = false;
			if (stateInfo->getFrontMove() > 1.0e-4f) {
				forward = true;
			}
			bool leftMove = false;
			if (stateInfo->getLeftMove() > 1.0e-4f) {
				leftMove = true;
			}
			bool rightMove = false;
			if (stateInfo->getLeftMove() < -1.0e-4f) {
				rightMove = true;
			}

			AnimationPtr animation;
			if (forward) {
				if (leftMove) {
					animation = this->data->actor->getAnimation("walk45Left");
				}
				else if (rightMove) {
					animation = this->data->actor->getAnimation("walk45Right");
				}
				else {
					animation = this->data->actor->getAnimation("walking");
				}
			}
			else if (backwards) {
				if (leftMove) {
					animation = this->data->actor->getAnimation("walk45Right");
				}
				else if (rightMove) {
					animation = this->data->actor->getAnimation("walk45Left");
				}
				else {
					animation = this->data->actor->getAnimation("walking");
				}
			}
			else {
				if (leftMove) {
					animation = this->data->actor->getAnimation("walkStrafeLeft");
				}
				else if (rightMove) {
					animation = this->data->actor->getAnimation("walkStrafeRight");
				}
				else {
					animation = this->data->actor->getAnimation("idle");
				}
			}

			this->getActor()->setPosition(stateInfo->getPosition());
			this->getActor()->setOrientation(stateInfo->getOrientation());
			this->getActor()->queueAnimation(animation);
			
			Vector3d position = stateInfo->getPosition();
			
			float time = animTimer->getElapsed(SECONDS);
			foreach(AnimationPtr animation, this->animations) {
				animation->setSpeedFactor(0.0f);
				animation->setTime(time);
			}
			
			timer->start();
		}

		void updateAdditionalInfo(CharacterStateInfoPtr stateInfo) {
			if (stateInfo->getState() != int(this->getState())) {
				this->setState(StatueState(stateInfo->getState()));
			}
			/*if (stateInfo->isVisible() && this->getState() != TRAPPER) {
				//attempt to remedy choppy animation
				this->actions->walk->setAnimationSpeed(1.0f);
			}
			if (!stateInfo->isVisible() && this->getState() == TRAPPER) {
				//attempt to remedy choppy animation
				this->actions->walk->setAnimationSpeed(0.25f);
			}*/
		}
	};

	typedef SharedPointer<Statue> StatuePtr;
}

#endif