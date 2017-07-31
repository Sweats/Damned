#ifndef TERROR_HUMAN_H
#define TERROR_HUMAN_H

#include "Packets/HumanInfo/HumanInfo.h"
#include "Inventory/Inventory.h"
#include "Flashlight/Flashlight.h"
#include "Actions/Manipulate/Manipulate.h"
#include "Actions/PickDead/PickDead.h"
#include "Actions/PickItem/PickItem.h"
#include "Actions/Sprint/Sprint.h"
#include "Actions/Die/Die.h"
#include "MathModels/Interpolator/LinearInterpolator/LinearInterpolator.h"

namespace terror {
	enum MovementState {
		IDLE,
		WALKING,
		RUNNING,
		SPRINTING
	};

	struct HumanAnimationNames : public CharacterAnimationNames {
		string sprint;
		string diagonalRightSprint;
		string diagonalLeftSprint;
		string die;
	};

	typedef SharedPointer<HumanAnimationNames> HumanAnimationNamesPtr;

	struct HumanActions {
		PickItemPtr pickItem;
		PickItemPtr pickBattery;
		PickDeadPtr pickDead;
		ManipulatePtr manipulate;
		SprintPtr sprint;
		DiePtr die;
	};

	typedef SharedPointer<HumanActions> HumanActionsPtr;	

	class Human : public Character {
	public:
		Human(CharacterDataPtr characterData, CharacterType character, CharacterActionsPtr characterActions, HumanActionsPtr humanActions, FlashlightPtr flashlight, 
			InventoryPtr inventory)
			: Character(characterData, characterActions), humanActions(humanActions), flashlight(flashlight), inventory(inventory), humanInfo(new HumanInfo(character)), movementState(IDLE),
			flashlightSwitched(false), neckBone(characterData->actor->getBone("bn_head01")), armBone(characterData->actor->getBone("be_R_shld01")), neckBoneOrientation(1.0f, 0.0f, 0.0f, 0.0f), 
			armBoneOrientation(1.0f, 0.0f, 0.0f, 0.0f), neckInterpolator(new QuaternionInterpolator), armInterpolator(new QuaternionInterpolator), monsterInRange(false)
		{
			//this->actions->crouchedWalk->setAnimationSpeed(1.0f);
			this->actions->walk->setAnimationSpeed(1.0f);			
			this->actions->run->setAnimationSpeed(2.0f);			
			this->actions->crouchedRun->setAnimationSpeed(1.0f);
			this->actions->crouch->setMeshOffset(Vector3d(0.0f, 0.4f, 0.0f));
			this->humanActions->sprint->setAnimationSpeed(4.0f);
			
			Ogre::TagPoint* meshTagPoint = this->getActor()->getMesh()->attachObjectToBone("be_flashlightHand", this->flashlight->getMesh(),
				Ogre::Quaternion(0.707107f, 0.0f, 0.707107f, 0.0f));
			meshTagPoint->setInheritScale(false);

			this->getActor()->connectUpdatedEvent(Bind(&Human::updateFlashlight, this));			

			this->setEyeDirection(Vector3d(0.0f, 0.0f, 1.0f));
			this->neckBone->setManuallyControlled(true);
			this->armBone->setManuallyControlled(true);

			this->additionalFlashlightEnergyConsumptionRate = 0.17*this->flashlight->getBattery()->getConsumptionRate();
		}

		void localUpdate() {
			this->humanActions->pickItem->execute();
			this->humanActions->pickBattery->execute();
			this->humanActions->pickDead->execute();
			if (!this->humanActions->manipulate->isManipulating()) {	
				this->humanActions->manipulate->execute();
			}
		}

		void updateManipulation(float diffTime, int xMouseRelative) {
			this->humanActions->manipulate->update(diffTime, xMouseRelative);
		}

		void updateBones(float diffTime) {
			if (this->neckInterpolator->needUpdate()) {
				this->neckBoneOrientation = this->neckInterpolator->update(diffTime);
			}
			this->neckBone->setOrientation(toOgre(this->neckBoneOrientation));

			if (this->armInterpolator->needUpdate()) {
				this->armBoneOrientation = this->armInterpolator->update(diffTime);				
			}
			this->armBone->setOrientation(toOgre(this->armBoneOrientation));			
		}

		void updateFlashlightBattery(float diffTime) {
			BatteryPtr battery = this->flashlight->getBattery(); 
			battery->update(diffTime);
			this->humanInfo->flashlightBatteryCharge = battery->getCharge();
		}

		void addChargeToFlashlightBattery(float charge) {
			this->setFlashlightBatteryCharge(charge + this->flashlight->getBattery()->getCharge());
		}

		void setFlashlightBatteryCharge(float charge) {
			this->flashlight->getBattery()->setCharge(charge);
			this->humanInfo->flashlightBatteryCharge = this->flashlight->getBattery()->getCharge();
		}

		float getFlashlightBatteryCharge() {
			return this->flashlight->getBattery()->getCharge();
		}

		void updateFlashlightIntensityFactor(float diffTime) {
			this->flashlight->updateLightIntensityFactor(diffTime, this->monsterInRange);
		}

		void setFlashlightIntensityFactor(float lightIntensityFactor) {
			this->flashlight->setLightIntensityFactor(lightIntensityFactor);
		}

		float getFlashlightIntensityFactor() {
			return this->flashlight->getLightIntensityFactor();
		}

		void die(bool fastForward = false) {
			this->releaseObject();
			if (this->data->dead) {				
				return;
			}
			
			if (fastForward) {
				this->humanActions->die->fastForward();
			}else {
				this->humanActions->die->execute();	
			}

			this->neckBone->setOrientation(toOgre(Quaternion(1.0f, 0.0f, 0.0f, 0.0f)));
			this->armBone->setOrientation(toOgre(Quaternion(1.0f, 0.0f, 0.0f, 0.0f)));
			this->neckBone->setManuallyControlled(false);
			this->armBone->setManuallyControlled(false);
		}

		void sprint(float left, float front) {
			if (this->data->dead) {
				return;
			}
			this->humanActions->sprint->setMovement(left, front);
			this->humanActions->sprint->execute();
		}

		void consumeAllStamina() {
			this->humanActions->sprint->setStamina(0.0f);
		}

		void stopSprint() {
			this->humanActions->sprint->stop();
		}

		bool canSprint() {
			return this->humanActions->sprint->canExecute();
		}

		bool isSprinting() {
			return this->humanActions->sprint->isSprinting();
		}

		void setSpeedMultiplier(float multiplier) {
			if (multiplier < 0.0f) {
				multiplier = 0.0f;
			}
			this->actions->walk->setSpeedMultiplier(multiplier);
			this->actions->run->setSpeedMultiplier(multiplier);
		}

		void show() {
			this->getActor()->show();
			this->flashlight->notifyVisible();
			if (this->data->dead) {
				this->getActor()->getCollisionObject()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);	
			}
		}

		void hide() {			
			this->getActor()->hide();
			this->flashlight->notifyInvisible();
		}

		bool isVisible() {
			return this->getActor()->isVisible();
		}

		void switchFlashlight(bool synchronizeState = true, bool playSound = true) {
			if (!this->flashlight) {
				return;
			}
			this->flashlightSwitched = true;
			this->flashlight->setOn(!this->flashlight->isOn(), this->getActor()->isVisible(), playSound);
			if (synchronizeState) {
				this->humanInfo->flashlightState = this->flashlight->isOn();
			}
		}

		void setFlashlightState(bool flashlightState, bool synchronizeState = true) {
			if (this->flashlight->isOn() != flashlightState) {
				this->flashlightSwitched = true;
			}
			this->flashlight->setOn(flashlightState, this->getActor()->isVisible());
			if (synchronizeState) {
				this->humanInfo->flashlightState = flashlightState;
			}	
		}

		bool hasFlashlightSwitched() {
			if (!this->flashlightSwitched) {
				return false;
			}
			this->flashlightSwitched = false;
			return true;
		}

		bool getFlashlightState() {
			return this->humanInfo->flashlightState;
		}

		bool tryGrabbingObject() {
			this->humanActions->manipulate->grabObject();
			return this->humanActions->manipulate->isManipulating();
		}

		void releaseObject() {
			this->humanActions->manipulate->releaseObject();
		}

		bool tryPickingItem() {			
			if (this->humanActions->pickItem->getObjectId() == -1) {			
				return this->humanActions->pickDead->getDeadId() != -1;
			}
			return true;
		}

		bool tryPickingBattery() {			
			return this->humanActions->pickBattery->getObjectId() != -1;
		}

		int getItemId() {
			return this->humanActions->pickItem->getObjectId();
		}

		int getBatteryId() {
			return this->humanActions->pickBattery->getObjectId();
		}

		int getDeadId() {
			return this->humanActions->pickDead->getDeadId();
		}

		InventoryPtr getInventory() {
			return this->inventory;
		}

		void addItem(ItemPtr item, bool playSound = true) {
			this->inventory->addItem(item, (playSound && this->getActor()->isVisible()));
		}

		bool hasItem(int mask) {
			return this->inventory->hasItem(mask);
		}		

		HumanInfoPtr getInfo() {
			return this->humanInfo;
		}

		void updateBasedOnInfo(HumanInfoPtr humanInfo) {
			this->humanInfo = humanInfo;
			this->updateStateInfo(humanInfo->stateInfo);
		}

		void updateInfo(HumanInfoPtr humanInfo) {
			this->humanInfo = humanInfo;
		}

		CharacterType getCharacterType() {
			return this->humanInfo->character;
		}

		void setEyeDirection(const Vector3d& eyeDirection) {
			this->eyeDirection = eyeDirection;

			Vector3d eyeX(1.0f, 0.0f, 0.0f);
			Vector3d eyeY = cross(eyeDirection, eyeX);
			Quaternion nextArmBoneOrientation(eyeX, eyeY, eyeDirection);
			this->armInterpolator->setData(this->armBoneOrientation, nextArmBoneOrientation, 0.05f);

			this->eyeDirection.setY((this->eyeDirection.getY() > 0.25f) ? 0.25f : (this->eyeDirection.getY() < -0.25f) ? -0.25f : this->eyeDirection.getY());
			this->eyeDirection.normalize();
			eyeY = cross(this->eyeDirection, eyeX);
			Quaternion nextNeckBoneOrientation = Quaternion(0.707107f, 0.0f, 0.707107f, 0.0f)*Quaternion(this->eyeDirection, eyeY, -1.0f*eyeX);
			this->neckInterpolator->setData(this->neckBoneOrientation, nextNeckBoneOrientation, 0.05f);
		}

		Vector3d getEyeDirection() {
			return this->eyeDirection;
		}

		void setMovementState(MovementState movementState) {
			this->movementState = movementState;
		}

		MovementState getMovementState() {
			return this->movementState;
		}

		void notifyMonsterInRange() {
			this->monsterInRange = true;
			this->flashlight->getBattery()->increaseConsumptionRate(this->additionalFlashlightEnergyConsumptionRate);
		}

		void notifyMonsterNotInRange() {
			this->monsterInRange = false;
			this->flashlight->getBattery()->restoreDefaultConsumptionRate();
		}

		bool isMonsterInRange() {
			return this->monsterInRange;
		}

		void destroy() {
			this->getActor()->destroy();
			this->flashlight->destroy();
			this->inventory->clear();
		}

		virtual ~Human() {}

	protected:
		HumanActionsPtr humanActions;
		FlashlightPtr flashlight;
		InventoryPtr inventory;		
		HumanInfoPtr humanInfo;
		MovementState movementState;
		bool flashlightSwitched;

		Vector3d eyeDirection;
		BonePtr neckBone;
		BonePtr armBone;
		Quaternion neckBoneOrientation;
		Quaternion armBoneOrientation;
		QuaternionInterpolatorPtr neckInterpolator;
		QuaternionInterpolatorPtr armInterpolator;

		bool monsterInRange;
		float additionalFlashlightEnergyConsumptionRate;

	private:
		void updateStateInfo(CharacterStateInfoPtr stateInfo) {
			Vector3d oldPosition = this->getActor()->getPosition();
			float distance = (stateInfo->getPosition() - oldPosition).norm();
			if (distance > 10.0f) {
				this->getActor()->setPosition(stateInfo->getPosition());
				this->getActor()->setOrientation(stateInfo->getOrientation());
			}else{
				this->getActor()->setPosition(stateInfo->getPosition(), 0.1f);
				this->getActor()->setOrientation(stateInfo->getOrientation(), 0.1f);
			}

			if (stateInfo->isCrouched() && !this->isCrouched()) {
				this->crouch();
			}
			if (!stateInfo->isCrouched() && this->isCrouched()) {
				this->stand();
			}
			if (stateInfo->isIdle()) {
				this->stopSprint();
				this->idle();
				this->movementState = IDLE;
			}else if (stateInfo->isSprinting()) {
				this->sprint(stateInfo->getLeftMove(), stateInfo->getFrontMove());
				this->movementState = SPRINTING;
			}else if (stateInfo->isRunning()) {
				this->stopSprint();
				this->run(stateInfo->getLeftMove(), stateInfo->getFrontMove());
				this->movementState = RUNNING;
			}else {
				this->stopSprint();
				this->walk(stateInfo->getLeftMove(), stateInfo->getFrontMove());
				this->movementState = WALKING;
			}
		}

		void updateFlashlight() {
			BonePtr bone = this->getActor()->getBone("be_flashlightHand");
			this->flashlight->updatePosition(this->getActor()->getBonePosition(bone), this->getActor()->getBoneOrientation(bone));
			this->flashlight->updateLightIntensity();
		}
	};

	typedef SharedPointer<Human> HumanPtr;
}

#endif