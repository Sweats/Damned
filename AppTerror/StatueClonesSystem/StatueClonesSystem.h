#ifndef DAMNED_STATUE_CLONES_SYSTEM_H
#define DAMNED_STATUE_CLONES_SYSTEM_H

#include "Packets/Packets.h"
#include "Human/Human.h"
#include "StatueClone/StatueClone.h"

namespace terror {
	typedef PointerList<Human> HumanPtrList;

	typedef IntPointerMap<entities::Entity> EntityIntPtrMap;

	class EnableCloneHighlight :public entities::Message {
	public:
		EnableCloneHighlight() {}

		virtual ~EnableCloneHighlight() {}
	};
	typedef SharedPointer<EnableCloneHighlight> EnableCloneHighlightPtr;

	class DisableCloneHighlight :public entities::Message {
	public:
		DisableCloneHighlight() {}

		virtual ~DisableCloneHighlight() {}
	};
	typedef SharedPointer<DisableCloneHighlight> DisableCloneHighlightPtr;

	class CloneAffectorLogic : public entities::Component {
	public:
		CloneAffectorLogic(HumanPtrList* humans) 
			: humans(humans), collisionTester(Core::getInstance()->getCollisionTester())
		{
			this->addDependency<CloneLifeComponent>();
			this->addDependency<CloneObjectComponent>();
		}

		virtual void initialize() {
			this->lifeComponent = this->getOwner()->getComponent<CloneLifeComponent>();
			this->objectComponent = this->getOwner()->getComponent<CloneObjectComponent>();
		}

		virtual void update(float diffTime) {
			float minDistance = 30.0f;
			float minDistanceLifeConsumption = 3.0f;
			float lifeConsumptionRate = -1.0f;
			Vector3d clonePosition = this->objectComponent->getPosition();

			bool shouldHighlight = false;

			foreach (HumanPtr human, *this->humans) {
				if (human->isDead()) {
					continue;
				}
				Vector3d humanPosition = human->getActor()->getPosition();
				Vector3d humanToClone = clonePosition - humanPosition;
				if (humanToClone.horizontalNorm() > minDistance) {
					continue;
				}

				Vector3d eyeDirection = rotate(human->getActor()->getOrientation(), human->getEyeDirection());
				if (humanToClone.horizontalNorm() > minDistanceLifeConsumption && dot(eyeDirection, getUnitary(humanToClone)) < 0.9f) {
					continue;
				}
					
				ClosestRayResultCallback callback;
				callback.addIgnoreObject(this->objectComponent->getCollisionObject().get());
				callback.addIgnoreObject(human->getActor()->getCollisionObject().get());
				callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
				if (this->collisionTester->rayTest(toBullet(humanPosition), toBullet(clonePosition), callback)) {
					continue;
				}

				shouldHighlight = true;

				if (humanToClone.horizontalNorm() > minDistanceLifeConsumption || dot(eyeDirection, getUnitary(humanToClone)) < 0.9f) {
					continue;
				}

				if (human->getFlashlightState() == false || human->getFlashlightBatteryCharge() < 0.2f) {
					continue;
				}

				if (lifeConsumptionRate < 0.0f) {
					lifeConsumptionRate = 1.25f;
				}else {
					lifeConsumptionRate += 0.25f;
				}
			}
			if (shouldHighlight) {
				EnableCloneHighlightPtr enableHighlight(new EnableCloneHighlight);
				this->getOwner()->queueMessage(enableHighlight);
			}else {
				DisableCloneHighlightPtr disableHighlight(new DisableCloneHighlight);
				this->getOwner()->queueMessage(disableHighlight);
			}

			if (lifeConsumptionRate > 0.0f) {
				this->lifeComponent->consumeLife(diffTime*lifeConsumptionRate);
			}
		}
		
		virtual ~CloneAffectorLogic() {}

	private:
		CloneLifeComponentPtr lifeComponent;
		CloneObjectComponentPtr objectComponent;
		CollisionTesterPtr collisionTester;
		HumanPtrList* humans;
	};
	typedef SharedPointer<CloneAffectorLogic> CloneAffectorLogicPtr;

	class CloneViewComponent : public entities::Component {
	public:
		CloneViewComponent(bool firstPersonEnabled) : firstPersonEnabled(firstPersonEnabled) {
			this->addDependency<CloneObjectComponent>();
		}

		void setStatueFirstPersonEnabled(bool firstPersonEnabled) {
			this->firstPersonEnabled = firstPersonEnabled;
		}

		virtual void initialize() {
			this->objectComponent = this->getOwner()->getComponent<CloneObjectComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {			
			if (!this->firstPersonEnabled) {
				return;
			}

			std::type_index messageHash = hashByType(message);
			if (messageHash == hashByType<EnableCloneHighlight>()) {
				this->objectComponent->enableHighlight();
			}
			if (messageHash == hashByType<DisableCloneHighlight>()) {
				this->objectComponent->disableHighlight();
			}
		}

		virtual ~CloneViewComponent() {}

	private:
		CloneObjectComponentPtr objectComponent;
		bool firstPersonEnabled;
	};
	typedef SharedPointer<CloneViewComponent> CloneViewComponentPtr;

	class CloneEventsNotifierComponent : public entities::Component {
	public:
		typedef Signal<void (float)> FloatSignal;

		CloneEventsNotifierComponent() {}

		void connectCloneDeathEvent(const BasicSignal::slot_type& cloneDeathEvent) {
			this->cloneDeathSignal.connect(cloneDeathEvent);
		}

		void connectCloneVanishEvent(const BasicSignal::slot_type& cloneVanishEvent) {
			this->cloneVanishSignal.connect(cloneVanishEvent);
		}

		void connectCloneLifeUpdatedEvent(const FloatSignal::slot_type& cloneLifeUpdateEvent) {
			this->cloneLifeUpdatedSignal.connect(cloneLifeUpdateEvent);
		}
		
		virtual void handleMessage(entities::MessagePtr message) {
			std::type_index messageHash = hashByType(message);
			if (messageHash == hashByType<CloneDeathMessage>()) {
				this->cloneDeathSignal();
			}
			if (messageHash == hashByType<CloneStartVanishingMessage>()) {
				this->cloneVanishSignal();
			}
			if (messageHash == hashByType<CloneNotifyLifeUpdated>()) {
				CloneNotifyLifeUpdatedPtr cloneNotifyLifeUpdated = static_ptr_cast<CloneNotifyLifeUpdated>(message);
				this->cloneLifeUpdatedSignal(cloneNotifyLifeUpdated->getLife());
			}
		}

		virtual ~CloneEventsNotifierComponent() {}

	private:
		BasicSignal cloneDeathSignal;
		BasicSignal cloneVanishSignal;
		FloatSignal cloneLifeUpdatedSignal;
	};
	typedef SharedPointer<CloneEventsNotifierComponent> CloneEventsNotifierComponentPtr;

	class StatueClonesSystem : public EventHandler, public NetworkObserver {
	public:
		StatueClonesSystem(TerrorClientPtr terrorClient) : EventHandler(20), terrorClient(terrorClient), statueFirstPersonEnabled(false), maxClones(3) {}
		
		void setMaxClones(int maxClones) {
			this->maxClones = maxClones;
		}

		void requestSpawnClone(const Vector3d& position, const Quaternion& orientation, float timePosition) {
			if (!this->terrorClient->isHost()) {
				SpawnStatueCloneRequestPtr request(new SpawnStatueCloneRequest(position, orientation, timePosition));
				this->terrorClient->sendObjectToHost(request, true);
				return;
			}
		
			int id = this->spawnClone(position, orientation, timePosition);

			SpawnStatueClonePtr spawnPacket(new SpawnStatueClone(id, position, orientation, timePosition));
			this->terrorClient->sendObjectToPlayers(spawnPacket, true);
		}

		void notifyHumanAdded(HumanPtr human) {
			this->humans.add(human);
		}

		void setStatueFirstPersonEnabled(float firstPersonEnabled) {
			this->statueFirstPersonEnabled = firstPersonEnabled;
			foreach (EntityIntPtrMap::value_type& entry, this->clones) {
				entities::EntityPtr clone = entry.second;
				clone->getComponent<CloneViewComponent>()->setStatueFirstPersonEnabled(firstPersonEnabled);
			}
		}

		void notifyStageUnloaded() {
			foreach (EntityIntPtrMap::value_type& entry, this->clones) {
				entities::EntityPtr clone = entry.second;
				clone->resetComponents();
			}
			this->clones.clear();
			this->clones.resetIdCounter();
			this->humans.clear();
		}

		EntityIntPtrMap& getClonesMap() {
			return this->clones;
		}

		virtual ~StatueClonesSystem() {}

	private:		
		TerrorClientPtr terrorClient;
		HumanPtrList humans;
		EntityIntPtrMap clones;
		IntList toBeDeleted;
		float statueFirstPersonEnabled;
		int maxClones;

	private:
		virtual bool handleEvent(float diffTime) {
			if (this->clones.size() > this->maxClones) {
				int idToDelete = -1;
				float oldestLifetime = 0.0f;
				foreach(EntityIntPtrMap::value_type& entry, this->clones) {
					entities::EntityPtr clone = entry.second;
					LifetimeComponentPtr lifetimeComponent = clone->getComponent<LifetimeComponent>();
					float lifetime = lifetimeComponent->getLifetime();
					if (lifetime >= oldestLifetime) {
						oldestLifetime = lifetime;
						idToDelete = entry.first;
					}
				}
				this->toBeDeleted.push_back(idToDelete);
			}

			foreach (int id, this->toBeDeleted) {
				entities::EntityPtr clone = this->clones.get(id);
				if (clone) {
					clone->resetComponents();
					this->clones.erase(id);
				}
			}
			this->toBeDeleted.clear();

			foreach (EntityIntPtrMap::value_type& entry, this->clones) {
				entities::EntityPtr clone = entry.second;
				clone->update(diffTime);
			}
			
			return true;
		}

		/*virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == HOST_INFO) {
				foreach (EntityIntPtrMap::value_type& entry, this->clones) {
					entities::EntityPtr clone = entry.second;
					CloneAffectorLogicPtr cloneAffectorLogic(new CloneAffectorLogic(&this->humans));
					clone->addComponent(cloneAffectorLogic);
				}
			}
		}*/

		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			// Host
			if (udpPacket->getLabel() == SPAWN_STATUE_CLONE_REQUEST) {
				SpawnStatueCloneRequestPtr request(new SpawnStatueCloneRequest);
				if (!udpPacket->buildObject(request)) {
					return;
				}

				int id = this->spawnClone(request->getPosition(), request->getOrientation(), request->getTimePosition());

				SpawnStatueClonePtr spawnPacket(new SpawnStatueClone(id, request->getPosition(), request->getOrientation(), request->getTimePosition()));
				this->terrorClient->sendObjectToPlayers(spawnPacket, true);
			}
			if (udpPacket->getLabel() == STAGE_STATE_REQUEST) {
				if (this->clones.empty()) {
					return;
				}
				SpawnStatueClonesPtr spawnStatueClones(new SpawnStatueClones);
				foreach(EntityIntPtrMap::value_type& entry, this->clones) {
					int id = entry.first;
					entities::EntityPtr clone = entry.second;
					CloneDataComponentPtr dataComponent = clone->getComponent<CloneDataComponent>();

					StatueCloneData data;
					data.id = id;
					data.position = dataComponent->getPosition();
					data.orientation = dataComponent->getOrientation();
					data.timePosition = dataComponent->getTimePosition();

					spawnStatueClones->addStatueCloneData(data);
				}
				this->terrorClient->sendObjectTo(spawnStatueClones, udpPacket->getRemoteUdpAddress(), true);
			}

			// Client
			if (udpPacket->getLabel() == SPAWN_STATUE_CLONE) {
				SpawnStatueClonePtr spawnPacket(new SpawnStatueClone);
				if (!udpPacket->buildObject(spawnPacket)) {
					return;
				}

				this->spawnClone(spawnPacket->getPosition(), spawnPacket->getOrientation(), spawnPacket->getTimePosition(), spawnPacket->getId());
			}

			if (udpPacket->getLabel() == SPAWN_STATUE_CLONES) {
				SpawnStatueClonesPtr spawnStatueClones(new SpawnStatueClones);
				if (!udpPacket->buildObject(spawnStatueClones)) {
					return;
				}
				foreach (const StatueCloneData& data, spawnStatueClones->getStatueCloneDatas()) {
					this->spawnClone(data.position, data.orientation, data.timePosition, data.id);
				}
			}

			if (udpPacket->getLabel() == STATUE_CLONE_LIFE_UPDATE) {
				StatueCloneLifeUpdatePtr statueCloneLifeUpdate(new StatueCloneLifeUpdate);
				if (!udpPacket->buildObject(statueCloneLifeUpdate)) {
					return;
				}

				entities::EntityPtr clone = this->clones.get(statueCloneLifeUpdate->getId());
				if (clone) {
					CloneLifeComponentPtr lifeComponent = clone->getComponent<CloneLifeComponent>();
					lifeComponent->setLife(statueCloneLifeUpdate->getLife());
				}
			}

			if (udpPacket->getLabel() == REMOVE_STATUE_CLONE) {
				RemoveStatueClonePtr removeStatueClone(new RemoveStatueClone);
				if (!udpPacket->buildObject(removeStatueClone)) {
					return;
				}
				entities::EntityPtr clone = this->clones.get(removeStatueClone->getId());
				if (clone) {
					CloneStartVanishingMessagePtr cloneStartVanishingMessage(new CloneStartVanishingMessage);
					clone->queueMessage(cloneStartVanishingMessage);
				}
			}
		}

		void notifyLifeUpdate(float life, int id) {
			if (!this->terrorClient->isHost()) {
				return;
			}
			StatueCloneLifeUpdatePtr statueCloneLifeUpdate(new StatueCloneLifeUpdate(id, life));
			this->terrorClient->sendObjectToPlayers(statueCloneLifeUpdate);
		}

		void notifyVanishing(int id) {
			if (!this->terrorClient->isHost()) {
				return;
			}

			RemoveStatueClonePtr removeStatueClone(new RemoveStatueClone(id));
			this->terrorClient->sendObjectToPlayers(removeStatueClone, true);
		}

		void removeClone(int id) {
			this->toBeDeleted.push_back(id);
		}

		int spawnClone(const Vector3d& position, const Quaternion& orientation, float timePosition, int id = -1) {
			entities::EntityPtr clone = createStatueClone(position, orientation, timePosition);
			
			if (id < 0) {
				id = this->clones.add(clone);
			}else {
				this->clones[id] = clone;
			}

			CloneEventsNotifierComponentPtr eventsNotifierComponent(new CloneEventsNotifierComponent);
			eventsNotifierComponent->connectCloneDeathEvent(Bind(&StatueClonesSystem::removeClone, this, id));
			eventsNotifierComponent->connectCloneVanishEvent(Bind(&StatueClonesSystem::notifyVanishing, this, id));
			eventsNotifierComponent->connectCloneLifeUpdatedEvent(Bind(&StatueClonesSystem::notifyLifeUpdate, this, _1, id));
			clone->addComponent(eventsNotifierComponent);

			CloneViewComponentPtr cloneViewComponent(new CloneViewComponent(this->statueFirstPersonEnabled));
			clone->addComponent(cloneViewComponent);

			CloneAffectorLogicPtr cloneAffectorLogic(new CloneAffectorLogic(&this->humans));
			clone->addComponent(cloneAffectorLogic);

			return id;
		}
	};

	typedef SharedPointer<StatueClonesSystem> StatueClonesSystemPtr;
}

#endif