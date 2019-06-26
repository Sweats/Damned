#ifndef TERROR_MANIPULATION_SYSTEM_H
#define TERROR_MANIPULATION_SYSTEM_H

#include "Packets/Packets.h"
#include "CharacterHandler/CharacterHandler.h"
#include "ItemDatabase/ItemDatabase.h"
#include "MessageSystem/MessageSystem.h"

namespace terror {
	typedef btPoint2PointConstraint Point2PointConstraint;
	typedef unordered_map<int, btPoint2PointConstraint*> Point2PointConstraintPtrMap;
	typedef Signal<void (ObjectPtr)> ObjectSignal;

	class ManipulationSystem : public NetworkObserver {
	public:
		ManipulationSystem(CharacterHandlerPtr characterHandler, PhysicsSystemPtr physicsSystem, TerrorClientPtr terrorClient) 
			: NetworkObserver(false), characterHandler(characterHandler), physicsSystem(physicsSystem), terrorClient(terrorClient) {}

		void notifyStageLoaded(StagePtr stage) {
			this->stage = stage;
		}

		void notifyStageUnloaded() {
			this->stage.reset();
		}

		void requestGrabObject(CharacterType character, int objectId, const Vector3d& handPosition) {
			if (!this->stage) {
				return;
			}

			if (!this->terrorClient->isHost()) {
				GrabObjectRequestPtr grabObjectRequest(new GrabObjectRequest(character, objectId, handPosition));
				this->terrorClient->sendObjectToHost(grabObjectRequest, true);
				return;
			}

			this->grabObject(character, objectId, handPosition);
		}

		void requestManipulateObject(CharacterType character, const Vector3d& handPosition) {
			if (!this->stage) {
				return;
			}

			if (!this->terrorClient->isHost()) {
				ManipulateObjectRequestPtr manipulateObjectRequest(new ManipulateObjectRequest(character, handPosition));
				this->terrorClient->sendObjectToHost(manipulateObjectRequest);
				return;
			}

			this->manipulateObject(character, handPosition);
		}

		void requestReleaseObject(CharacterType character) {
			if (!this->stage) {
				return;
			}

			if (!this->terrorClient->isHost()) {
				ReleaseObjectRequestPtr releaseObjectRequest(new ReleaseObjectRequest(character));
				this->terrorClient->sendObjectToHost(releaseObjectRequest, true);
				return;
			}

			this->releaseObject(character);
		}

		Connection connectLockedObjectEvent(const ObjectSignal::slot_type& lockedObjectEvent) {
			return this->lockedObjectSignal.connect(lockedObjectEvent);
		}

		void reset() {
			this->constraints.clear();
		}

		virtual ~ManipulationSystem() {}

	protected:
		Point2PointConstraintPtrMap constraints;
		CharacterHandlerPtr characterHandler;
		PhysicsSystemPtr physicsSystem;
		TerrorClientPtr terrorClient;
		StagePtr stage;
		ObjectSignal lockedObjectSignal;

	private:
		virtual void handleUdpPacket(UdpPacketPtr udpPacket) {
			if (!this->stage) {
				return;
			}

			// Host
			if (udpPacket->getLabel() == GRAB_OBJECT_REQUEST) {
				GrabObjectRequestPtr grabObjectRequest(new GrabObjectRequest);
				if (!udpPacket->buildObject(grabObjectRequest)) {
					return;
				}

				this->grabObject(grabObjectRequest->getCharacter(), grabObjectRequest->getObjectId(), grabObjectRequest->getHandPosition());
			}
			if (udpPacket->getLabel() == MANIPULATE_OBJECT_REQUEST) {
				ManipulateObjectRequestPtr manipulateObjectRequest(new ManipulateObjectRequest);
				if (!udpPacket->buildObject(manipulateObjectRequest)) {
					return;
				}

				this->manipulateObject(manipulateObjectRequest->getCharacter(), manipulateObjectRequest->getHandPosition());
			}

			if (udpPacket->getLabel() == RELEASE_OBJECT_REQUEST) {
				ReleaseObjectRequestPtr releaseObjectRequest(new ReleaseObjectRequest);
				if (!udpPacket->buildObject(releaseObjectRequest)) {
					return;
				}

				this->releaseObject(releaseObjectRequest->getCharacter());
			}

			// Client
			if (udpPacket->getLabel() == GRAB_OBJECT_RESPONSE) {
				GrabObjectResponsePtr grabObjectResponse(new GrabObjectResponse);
				if (!udpPacket->buildObject(grabObjectResponse)) {
					return;
				}
				
				ObjectPtr object = this->stage->getObject(grabObjectResponse->getObjectId());
				if (!object) {
					return;
				}

				if (grabObjectResponse->getError() == GRAB_OBJECT_LOCKED) {
					this->notifyLockedObject(object);
				}else if (grabObjectResponse->getError() == NO_GRAB_OBJECT_ERROR) {
					if (object->isLocked()) {
						this->unlockObject(object);
						PlayerInfoPtr playerInfo = PlayersState::getInstance()->getPlayerInfo(terrorClient->getId());
						if (playerInfo && grabObjectResponse->getCharacter() == playerInfo->getCharacter()) {
							if (object->getMask() < 2000) {
								SteamSystem::getInstance()->incrementStat("doors_unlocked", 1);
							}else {
								SteamSystem::getInstance()->incrementStat("barricades_broken", 1);
							}
						}
					}
				}
			}
		}

		void grabObject(CharacterType character, int objectId, const Vector3d& handPosition) {			
			ObjectPtr object = this->stage->getObject(objectId);
			if (!object) {
				return;
			}

			if (object->isLocked()) {
				int objectMask = object->getMask();
				HumanPtr human = this->characterHandler->getHumanAlive(character);
				if (isMonsterCharacter(character) || 
					human && !human->hasItem(objectMask%1000) && !human->hasItem(objectMask%1500) && !human->hasItem(objectMask%2000)) 
				{
					this->notifyLockedObject(object);
					GrabObjectResponsePtr grabObjectResponse(new GrabObjectResponse(GRAB_OBJECT_LOCKED, objectId, character));
					this->terrorClient->sendObjectToPlayers(grabObjectResponse, true);
					return;
				}
				this->unlockObject(object);

				PlayerInfoPtr playerInfo = PlayersState::getInstance()->getPlayerInfo(this->terrorClient->getId());
				if (playerInfo && character == playerInfo->getCharacter()) {
					if (object->getMask() < 2000) {
						SteamSystem::getInstance()->incrementStat("doors_unlocked", 1);
					}else {
						SteamSystem::getInstance()->incrementStat("barricades_broken", 1);
					}
				}
			}

			Point2PointConstraint* constraint = this->getConstraint(character);
			if (constraint) {
				this->releaseObject(character);
			}

			object->getBody()->setActivationState(DISABLE_DEACTIVATION);
			btVector3 localPivot = object->getBody()->getCenterOfMassTransform().inverse()*toBullet(handPosition);
			constraint = new btPoint2PointConstraint(*object->getBody(), localPivot);
			constraint->m_setting.m_impulseClamp = 2.0f;
			constraint->m_setting.m_tau = 0.0f;
			constraint->setParam(BT_CONSTRAINT_ERP, 0.2f);
			constraint->setParam(BT_CONSTRAINT_CFM, 1.0f);
			object->getBody()->addConstraintRef(constraint);			

			this->physicsSystem->getDynamicsWorld()->addConstraint(constraint);
			this->constraints[character] = constraint;

			GrabObjectResponsePtr grabObjectResponse(new GrabObjectResponse(NO_GRAB_OBJECT_ERROR, objectId, character));
			this->terrorClient->sendObjectToPlayers(grabObjectResponse, true);
		}

		void manipulateObject(CharacterType character, const Vector3d& handPosition) {
			Point2PointConstraint* constraint = this->getConstraint(character);
			if (!constraint) {
				return;
			}
			constraint->setPivotB(toBullet(handPosition));
		}

		void releaseObject(CharacterType character) {
			Point2PointConstraint* constraint = this->getConstraint(character);
			if (!constraint) {
				return;
			}
			constraint->getRigidBodyA().forceActivationState(ACTIVE_TAG);
			constraint->getRigidBodyA().removeConstraintRef(constraint);
			this->physicsSystem->getDynamicsWorld()->removeConstraint(constraint);
			this->constraints.erase(character);
		}

		void notifyLockedObject(ObjectPtr object) {
			SoundPtr lockedSoundEffect = object->getCollisionSoundEffect();
			if (object->isVisible() && lockedSoundEffect) {
				lockedSoundEffect->play();
			}
			this->lockedObjectSignal(object);
		}

		void unlockObject(ObjectPtr object) {
			object->unlock();

			PlayersStatePtr playersState = PlayersState::getInstance();
			PlayerInfoPtr playerInfo = playersState->getPlayerInfo(this->terrorClient->getId());
			if (!playerInfo || isMonsterCharacter(playerInfo->getCharacter())) {
				return;
			}

			std::string message;
			ItemDatabasePtr itemDatabase = ItemDatabase::getInstance();
			if (object->getMask() < 2000) {
				std::string objectType = object->getMask() < 1500 ? "door" : "drawer";
				int itemMask = object->getMask() < 1500 ? object->getMask()%1000 : object->getMask()%1500;
				ItemDataPtr itemData = itemDatabase->getItemData(itemMask);
				message = std::string("Item ") + itemData->getName() + std::string(" used to unlock a ") + objectType;
			}else {
				int itemMask = object->getMask()%2000;
				ItemDataPtr itemData = itemDatabase->getItemData(itemMask);
				if (itemMask == 155){
					//item 155 is Bolt Cutter and is used for chains on BLW
					message = std::string("Item ") + itemData->getName() + std::string(" used to cut a chain");
					this->stage->destroyObject(object);
				}else {
					message = std::string("Item ") + itemData->getName() + std::string(" used to loose a board");
				}
			}
			writeMessage(message);
		}

		Point2PointConstraint* getConstraint(CharacterType character) {
			Point2PointConstraintPtrMap::const_iterator entry = this->constraints.find(character);
			if (entry == this->constraints.end()) {
				return 0;
			}
			return entry->second;
		}
	};

	typedef SharedPointer<ManipulationSystem> ManipulationSystemPtr;
}

#endif