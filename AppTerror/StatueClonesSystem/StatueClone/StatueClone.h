#ifndef DAMNED_STATUE_CLONE_H
#define DAMNED_STATUE_CLONE_H

#include "Config.h"

namespace terror {
	class CloneNotifyLifeUpdated : public entities::Message {
	public:
		CloneNotifyLifeUpdated(float life, float maxLife) : life(life), maxLife(maxLife) {}

		float getLife() {
			return this->life;
		}

		float getMaxLife() {
			return this->maxLife;
		}

		virtual ~CloneNotifyLifeUpdated() {}

	private:
		float life;
		float maxLife;
	};
	typedef SharedPointer<CloneNotifyLifeUpdated> CloneNotifyLifeUpdatedPtr;

	class CloneStartVanishingMessage : public entities::Message {
	public:
		CloneStartVanishingMessage() {}

		virtual ~CloneStartVanishingMessage() {}
	};
	typedef SharedPointer<CloneStartVanishingMessage> CloneStartVanishingMessagePtr;

	class CloneDeathMessage : public entities::Message {
	public:
		CloneDeathMessage() {}

		virtual ~CloneDeathMessage() {}
	};
	typedef SharedPointer<CloneDeathMessage> CloneDeathMessagePtr;

	class CloneLifeComponent : public entities::Component {
	public:
		CloneLifeComponent(float maxLife) : maxLife(maxLife), life(maxLife) {}
		
		void consumeLife(float amount) {
			if (this->life <= 0.0f) {
				return;
			} 
			this->life -= amount;
			if (this->life <= 0.0f) {
				CloneStartVanishingMessagePtr startVanishingMessage(new CloneStartVanishingMessage);
				this->getOwner()->queueMessage(startVanishingMessage);
			}else {
				CloneNotifyLifeUpdatedPtr cloneNotifyLifeUpdated(new CloneNotifyLifeUpdated(this->life, this->maxLife));
				this->getOwner()->queueMessage(cloneNotifyLifeUpdated);
			}
		}

		void setLife(float life) {
			this->life = life;

			CloneNotifyLifeUpdatedPtr cloneNotifyLifeUpdated(new CloneNotifyLifeUpdated(this->life, this->maxLife));
			this->getOwner()->queueMessage(cloneNotifyLifeUpdated);
		}

		float getMaxLife() {
			return this->maxLife;
		}

		float getLife() {
			return this->life;
		}

		virtual ~CloneLifeComponent() {}

	private:
		float maxLife;
		float life;
	};
	typedef SharedPointer<CloneLifeComponent> CloneLifeComponentPtr;

	class StatueSpawnEvent : public Event {
	public:
		StatueSpawnEvent(ObjectPtr object, float time) : object(object), totalTime(time) {
			float amount = 2.0f;
			this->object->setPosition(object->getPosition() - amount*Vector3d::yUnit);
			this->object->setPosition(object->getPosition() + amount*Vector3d::yUnit, totalTime);
			this->time = 0.0f;
		}

		virtual void update(float diffTime) {
			this->time += diffTime;
			float angle = diffTime*2.0f*pi/this->totalTime;
			Quaternion rotation(cosf(0.5f*angle), 0.0f, sinf(0.5f*angle), 0.0f);
			this->object->setOrientation(rotation*object->getOrientation(), 0.05f);
		}

		virtual bool hasEnded() {
			return this->time >= this->totalTime;
		}

		virtual ~StatueSpawnEvent() {}

	private:
		ObjectPtr object;
		Vector3d initialPosition;
		float totalTime;
		float time;		
	};
	typedef SharedPointer<StatueSpawnEvent> StatueSpawnEventPtr;

	class CloneObjectComponent : public entities::Component {
	public:
		CloneObjectComponent(ObjectPtr object) 
			: object(object), id(CloneObjectComponent::getId()), multiplyFactor(1.0f), alpha(1.0f)
		{
			MeshInstancePtr meshInstance = this->object->getMesh();
			for (int i = 0; i < meshInstance->getNumSubEntities(); ++i) {
				Ogre::SubEntity* subEntity = meshInstance->getSubEntity(i);
				Ogre::MaterialPtr material = subEntity->getMaterial();
				string newMaterialName = string(material->getName()) + string("_") + lexical_cast<string>(this->id);
				Ogre::MaterialPtr newMaterial = material->clone(newMaterialName);
				subEntity->setMaterial(newMaterial);
			}

			StatueSpawnEventPtr statueSpawnEvent(new StatueSpawnEvent(object, 3.0f));
			Core::getInstance()->runEvent(statueSpawnEvent);
		}

		void setMultiplyColor(float multiplyFactor) {
			this->multiplyFactor = multiplyFactor;
			MeshInstancePtr meshInstance = this->object->getMesh();
			for (int i = 0; i < meshInstance->getNumSubEntities(); ++i) {
				Ogre::SubEntity* subEntity = meshInstance->getSubEntity(i);
				Ogre::MaterialPtr material = subEntity->getMaterial();				
				material->setDiffuse(ColourValue(this->multiplyFactor, this->multiplyFactor, this->multiplyFactor, this->alpha));
			}
		}

		void setAlpha(float alpha) {
			this->alpha = alpha;
			MeshInstancePtr meshInstance = this->object->getMesh();
			for (int i = 0; i < meshInstance->getNumSubEntities(); ++i) {
				Ogre::SubEntity* subEntity = meshInstance->getSubEntity(i);
				Ogre::MaterialPtr material = subEntity->getMaterial();				
				material->setDiffuse(ColourValue(this->multiplyFactor, this->multiplyFactor, this->multiplyFactor, this->alpha));
			}
		}

		void enableHighlight() {
			this->object->setMaterial("through_walls_mat");
		}

		void disableHighlight() {
			this->object->restoreMaterials();
		}

		Vector3d getPosition() {
			return this->object->getPosition();
		}		

		CollisionObjectPtr getCollisionObject() {
			return this->object->getCollisionObject();
		}

		virtual void reset() {
			this->object->destroy();
		}

		virtual ~CloneObjectComponent() {}

	private:
		CloneLifeComponentPtr lifeComponent;
		ObjectPtr object;
		int id;

		float multiplyFactor;
		float alpha;

	private:
		static int getId() {
			static int id = -1;
			return ++id;
		}
	};
	typedef SharedPointer<CloneObjectComponent> CloneObjectComponentPtr;

	class FadeToBlackLogic : public entities::Component {
	public:
		FadeToBlackLogic() {
			this->addDependency<CloneObjectComponent>();
		}

		virtual void initialize() {
			this->objectComponent = this->getOwner()->getComponent<CloneObjectComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<CloneNotifyLifeUpdated>()) {
				CloneNotifyLifeUpdatedPtr cloneNotifyLifeUpdated = static_ptr_cast<CloneNotifyLifeUpdated>(message);

				float relativeLife = cloneNotifyLifeUpdated->getLife()/cloneNotifyLifeUpdated->getMaxLife();
				float multiplyFactor = 2.0f*logf(1.0f + relativeLife*(e - 1.0f)) - 1.0f;
				this->objectComponent->setMultiplyColor(multiplyFactor);
			}
		}

		virtual ~FadeToBlackLogic() {}

	private:
		CloneObjectComponentPtr objectComponent;
	};
	typedef SharedPointer<FadeToBlackLogic> FadeToBlackLogicPtr;

	class VanishLogic : public entities::Component {
	public:
		VanishLogic() : active(false) {
			this->addDependency<CloneObjectComponent>();
		}

		virtual void initialize() {
			this->objectComponent = this->getOwner()->getComponent<CloneObjectComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<CloneStartVanishingMessage>()) {
				this->active = true;
				this->time = 1.0f;
				this->maxTime = 1.0f;
			}
		}

		virtual void update(float diffTime) {
			if (!this->active) {
				return;
			}
			this->time -= diffTime;

			float relativeTime = this->time/this->maxTime;
			this->objectComponent->setAlpha(logf(1.0f + relativeTime*(e - 1.0f)));

			if (this->time <= 0.0f) {
				this->active = false;
				CloneDeathMessagePtr cloneDeathMessage(new CloneDeathMessage);
				this->getOwner()->queueMessage(cloneDeathMessage);				
			}
		}

		virtual ~VanishLogic() {}

	private:
		CloneObjectComponentPtr objectComponent;
		float time;
		float maxTime;
		bool active;
	};
	typedef SharedPointer<VanishLogic> VanishLogicPtr;	

	class LifetimeComponent : public entities::Component {
	public:
		LifetimeComponent() : lifetime(0.0f) {}

		float getLifetime() {
			return this->lifetime;
		}

		virtual void update(float diffTime) {
			this->lifetime += diffTime;
		}

		virtual ~LifetimeComponent() {}

	private:
		float lifetime;
	};
	typedef SharedPointer<LifetimeComponent> LifetimeComponentPtr;

	class CloneDataComponent : public entities::Component {
	public:
		CloneDataComponent(const Vector3d& position, const Quaternion& orientation, float timePosition)
			: position(position), orientation(orientation), timePosition(timePosition) {}

		Vector3d getPosition() {
			return this->position;
		}

		Quaternion getOrientation() {
			return this->orientation;
		}

		float getTimePosition() {
			return this->timePosition;
		}

		CloneDataComponent() {}

	private:
		Vector3d position;
		Quaternion orientation;
		float timePosition;
	};
	typedef SharedPointer<CloneDataComponent> CloneDataComponentPtr;

	entities::EntityPtr createStatueClone(const Vector3d& position, const Quaternion& orientation, float timePosition) {
		entities::EntityPtr cloneEntity(new entities::Entity);

		ObjectPtr object = EntitySystem::getInstance()->createObject("statue_clone");
		object->setPosition(position);
		object->setOrientation(orientation);
		object->lock();

		SoundPtr sound = object->getSoundSource()->createSound("statue_creation.ogg");
		sound->play();

		MeshInstancePtr mesh = object->getMesh();
		Ogre::AnimationState* animationState = mesh->getAnimationState("poses");
		animationState->setEnabled(true);
		animationState->setTimePosition(timePosition);

		CloneDataComponentPtr cloneDataComponent(new CloneDataComponent(position, orientation, timePosition));
		cloneEntity->addComponent(cloneDataComponent);

		LifetimeComponentPtr lifetimeComponent(new LifetimeComponent);
		cloneEntity->addComponent(lifetimeComponent);

		CloneObjectComponentPtr objectComponent(new CloneObjectComponent(object));
		cloneEntity->addComponent(objectComponent);

		CloneLifeComponentPtr lifeComponent(new CloneLifeComponent(10.0f));
		cloneEntity->addComponent(lifeComponent);

		FadeToBlackLogicPtr fadeToBlackLogic(new FadeToBlackLogic);
		cloneEntity->addComponent(fadeToBlackLogic);

		VanishLogicPtr vanishLogic(new VanishLogic);
		cloneEntity->addComponent(vanishLogic);

		cloneEntity->initializeComponents();

		return cloneEntity;
	}
}

#endif