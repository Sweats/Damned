#ifndef TERROR_LOCAL_PLAYER_STATUE_CONTROLLER_H
#define TERROR_LOCAL_PLAYER_STATUE_CONTROLLER_H 

#include "StatueClonesSystem/StatueClonesSystem.h"
#include "MonsterControllers/StatueController/LocalStatueController/LocalStatueController.h"
#include "ScareTrapSystem/ScareTrapSystem.h"
#include "ManipulationSystem/ManipulationSystem.h"
#include "SpectatorSystem/SpectatorSystem.h"
#include "DeathSystem/DeathSystem.h"
#include "TerrorSave/TerrorSave.h"

namespace terror {
	class LocalStatueObjectViewComponent : public entities::Component {
	public:
		LocalStatueObjectViewComponent() {}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<PovChangedMessage>()) {
				PovChangedMessagePtr povChangedMessage = static_ptr_cast<PovChangedMessage>(message);
				ObjectPtrListPtr specialObjects = EntitySystem::getInstance()->getObjectsWithMaskBetween(1, 505);
				foreach (ObjectPtr specialObject, *specialObjects) {
					if (specialObject->getMask() == 500 || specialObject->getMask() == 501) {
						continue;
					}
					specialObject->hide();
				}
			}
		}

		virtual ~LocalStatueObjectViewComponent() {}
	};
	typedef SharedPointer<LocalStatueObjectViewComponent> LocalStatueObjectViewComponentPtr;

	class LocalStatueHumansViewComponent : public entities::Component {
	public:
		typedef PointerList<Human> HumanPtrList;

		LocalStatueHumansViewComponent() {
			this->addDependency<StatueStateComponent>();
		}

		virtual void initialize() {
			this->stateComponent = this->getOwner()->getComponent<StatueStateComponent>();
			this->handleHumansView();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<HumanAddedMessage>()) {
				HumanAddedMessagePtr humanAddedMessage = static_ptr_cast<HumanAddedMessage>(message);
				HumanPtr human = humanAddedMessage->getAddedHuman();
				if (!this->humans.contains(human)) {
					this->humans.add(human);
					this->handleHumansView();
				}
			}
			if (hashByType(message) == hashByType<PovChangedMessage>() || hashByType(message) == hashByType<StateChangedMessage>()) {
				this->handleHumansView();
			}
		}

		virtual ~LocalStatueHumansViewComponent() {}

	private:
		StatueStateComponentPtr stateComponent;
		HumanPtrList humans;

	private:
		void handleHumansView() {
			StatueState state = this->stateComponent->getState();
			switch (state) {
			case TRAPPER:
				foreach (HumanPtr human, this->humans) {
					human->hide();
				}
				break;

			case SEEKER:
				foreach (HumanPtr human, this->humans) {
					//Tentative change, Survivors can hear her, this should allow for some strategic plays
					human->show();
				}
				break;

			case DESTROYER:
				foreach (HumanPtr human, this->humans) {
					human->show();
				}
				break;
			}
		}
	};
	typedef SharedPointer<LocalStatueHumansViewComponent> LocalStatueHumansViewComponentPtr;

	class LocalStatueComponent : public entities::Component {
	public:
		LocalStatueComponent(StatuePtr statue, ScareTrapSystemPtr scareTrapSystem) : statue(statue), scareTrapSystem(scareTrapSystem) 
		{
			this->addDependency<StageComponent>();
			this->seekerSound = Core::getInstance()->createSound("monster_ready.ogg");
		}

		virtual void initialize() {
			this->stageComponent = this->getOwner()->getComponent<StageComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				if (stateChangedMessage->getCurrentState() == TRAPPER) {
					this->statue->releaseObject();
					if (this->stageComponent->getStageName() == "Rose_Crimson_Hotel") {
						this->scareTrapSystem->reset(0.3f);
					} else if (this->stageComponent->getStageName() == "Factory") {
						this->scareTrapSystem->reset(0.8f);
					} else if (this->stageComponent->getStageName() == "Warehouse") {
						this->scareTrapSystem->reset(0.45f);
					} else if (this->stageComponent->getStageName() == "Pog_Champ_Hotel") {
						//Tentative change
						this->scareTrapSystem->reset(0.5f);
					} else {
						this->scareTrapSystem->reset(0.7f);
					}
				}
				if (stateChangedMessage->getCurrentState() == SEEKER) {
					this->seekerSound->play();
				}
			}
		}

		virtual ~LocalStatueComponent() {}

	private:
		StageComponentPtr stageComponent;
		StatuePtr statue;
		ScareTrapSystemPtr scareTrapSystem;
		SoundPtr seekerSound;
	};
	typedef SharedPointer<LocalStatueComponent> LocalStatueComponentPtr;

	class SeekerLogicComponent : public entities::Component {
	public:
		typedef PointerList<Human> HumanPtrList;

		SeekerLogicComponent(StatuePtr statue, float range) : statue(statue), active(false), range(range), triggerTransform(false), timeSinceTrigger(0.0f), sequence(new EventSequence)
		{
			this->addDependency<StatueStateComponent>();			
		}

		virtual void initialize() {
			this->stateComponent = this->getOwner()->getComponent<StatueStateComponent>();

			EventDelayerPtr delayer(new EventDelayer(2.0f));
			this->sequence->addEvent(delayer);

			EventFunctionPtr function(new EventFunction(Bind(&StatueStateComponent::setState, this->stateComponent, DESTROYER)));
			this->sequence->addEvent(function);
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<HumanAddedMessage>()) {
				HumanAddedMessagePtr humanAddedMessage = static_ptr_cast<HumanAddedMessage>(message);
				HumanPtr human = humanAddedMessage->getAddedHuman();
				if (!this->humans.contains(human)) {
					this->humans.add(human);
				}
			}

			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				if (stateChangedMessage->getCurrentState() == SEEKER) {
					this->active = true;
				}
			}
		}

		virtual void update(float diffTime) {
			if (!this->active) {
				return;
			}
			this->runLogic();
			//this instead of EventSequence to go from Seeker to Destroyer
			if (this->triggerTransform == true){
				this->timeSinceTrigger+= diffTime;
				if (this->timeSinceTrigger >= 2.0f) {
					this->stateComponent->setState(DESTROYER);
					this->triggerTransform = false;
					this->timeSinceTrigger = 0.0f;
				}
			}
		}

		virtual void reset() {
			Core::getInstance()->cancelEvent(this->sequence);
			this->active = false;
		}

		virtual EventSequencePtr getSequence(){
			return this->sequence;
		}

		virtual ~SeekerLogicComponent() {}

	private:
		StatueStateComponentPtr stateComponent;		
		StatuePtr statue;
		HumanPtrList humans;
		EventSequencePtr sequence;
		float range;
		bool active;
		bool triggerTransform;
		float timeSinceTrigger;

	private:
		void runLogic() {
			RenderCameraPtr camera = Core::getInstance()->getGraphicsSystem()->getCamera();
			Vector3d camDirection = toAlgebra(camera->getDirection());
			Vector3d camPosition = toAlgebra(camera->getPosition());
			foreach (HumanPtr human, this->humans) {
				if (human->isDead()) {
					continue;
				}

				Vector3d humanPosition = human->getActor()->getPosition();
				Vector3d camToHuman = humanPosition - camPosition;

				if (camToHuman.horizontalNorm() > this->range) {
					continue;
				}
				
				camToHuman.normalize();
				bool isInFov = (dot(camDirection, camToHuman) > 0.9f) ? true : false;
				if (!isInFov) {
					continue;
				}
				ClosestRayResultCallback callback;
				callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
				if (!Core::getInstance()->getCollisionTester()->rayTest(toBullet(camPosition), toBullet(humanPosition), callback)) {
					this->active = false;
					StartTransformMessagePtr message(new StartTransformMessage);
					this->getOwner()->queueMessage(message);
					this->triggerTransform = true;
					//Core::getInstance()->runEvent(this->sequence);
					return;
				}
			}
		}
	};
	typedef SharedPointer<SeekerLogicComponent> SeekerLogicComponentPtr;

	class DestroyerLogicComponent : public entities::Component {
	public:
		DestroyerLogicComponent(float timeInDestroyerMode) 
			: active(false), timeInDestroyerMode(timeInDestroyerMode), timeSinceTransformation(0.0f), lastPhasing(0.0f), triggerPhasing(false)
		{
			this->addDependency<StatueStateComponent>();
		}

		virtual void initialize() {
			this->stateComponent = this->getOwner()->getComponent<StatueStateComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<DestroyerEndedMessage>()) {
				this->active = false;
			}
			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				if (stateChangedMessage->getCurrentState() == DESTROYER && this->active == false) {
					//don't reset timer when coming out of phasing
					this->active = true;
					this->timeSinceTransformation = 0.0f;
					this->lastPhasing = 0.0f;
				}
			}
		}

		virtual void update(float diffTime) {
			if (!this->active) {
				return;
			}
			this->timeSinceTransformation += diffTime;
			if (this->triggerPhasing == true) {
				if (this->timeSinceTransformation >= this->lastPhasing+3.0f){
					this->stateComponent->setState(DESTROYER);
					this->triggerPhasing = false;
				}
			}
			if (this->timeSinceTransformation >= this->timeInDestroyerMode) {
				this->active = false;
				DestroyerEndedMessagePtr message(new DestroyerEndedMessage);
				this->getOwner()->queueMessage(message);
				this->stateComponent->setState(TRAPPER);
				this->lastPhasing = 0.0f;
			}
		}

		virtual float getMaxP3Time(){
			return this->timeInDestroyerMode;
		}
		virtual float getCurrP3Time(){
			return this->timeSinceTransformation;
		}
		virtual float getLastPhasing(){
			return this->lastPhasing;
		}
		virtual void setLastPhasing(float setLastPhasing){
			this->lastPhasing = setLastPhasing;
		}
		virtual void doTriggerPhasing(){
			this->triggerPhasing = true;
		}

		virtual ~DestroyerLogicComponent() {}

	private:
		StatueStateComponentPtr stateComponent;
		bool active;
		bool triggerPhasing;
		float timeInDestroyerMode;
		float timeSinceTransformation;
		float lastPhasing;
	};
	typedef SharedPointer<DestroyerLogicComponent> DestroyerLogicComponentPtr;

	class LocalStatueMessagesComponent : public entities::Component {
	public:
		LocalStatueMessagesComponent(ControlsPtr controls) : controls(controls), p3_active(false) {}

		virtual void handleMessage(entities::MessagePtr message) {
			if (hashByType(message) == hashByType<DestroyerEndedMessage>()) {
				this->p3_active = false;
			}
			if (hashByType(message) == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				switch (stateChangedMessage->getCurrentState()) {
				case TRAPPER:
					if (this->p3_active == false){
						writeMessage("Arm traps to collect energy to enter next mode and produce statue clones");
					}
					break;
				case SEEKER:
					writeMessage("Find a prey to hunt");
					break;
				case DESTROYER:
					//Tentative change
					if (this->p3_active == false){
						string button = Core::getInstance()->getButtonTranslator()->translateButton(this->controls->transform);
						string message = string("Press ") + button + string(" to momentarily phase out of killing Mode!");
						writeMessage(message);
						this->p3_active = true;
					}
					break;
				}
			}
		}

		virtual ~LocalStatueMessagesComponent() {}

	private:
		ControlsPtr controls;
		bool p3_active;
	};
	typedef SharedPointer<LocalStatueMessagesComponent> LocalStatueMessagesComponentPtr;

	class StatueCloneSpawner : public entities::Component {
	public:
		StatueCloneSpawner(StatueClonesSystemPtr statueClonesSystem) 
			: statueClonesSystem(statueClonesSystem), collisionTester(Core::getInstance()->getCollisionTester()),
			trapsToSpawnClone(2), trapsArmedSinceLastSpawn(0) 
		{
			this->addDependency<StatuePhysicsComponent>();
		}

		virtual void initialize() {
			this->physicsComponent = this->getOwner()->getComponent<StatuePhysicsComponent>();
		}

		virtual void handleMessage(entities::MessagePtr message) {
			std::type_index messageIndex = hashByType(message);
			if (messageIndex == hashByType<TrapArmedMessage>()) {
				this->handleTrapArmed();
			}
			if (messageIndex == hashByType<StateChangedMessage>()) {
				StateChangedMessagePtr stateChangedMessage = static_ptr_cast<StateChangedMessage>(message);
				if (stateChangedMessage->getCurrentState() == TRAPPER) {
					this->trapsArmedSinceLastSpawn = 0;
				}
			}
		}

		virtual ~StatueCloneSpawner() {}

	private:
		CollisionTesterPtr collisionTester;
		StatuePhysicsComponentPtr physicsComponent;
		StatueClonesSystemPtr statueClonesSystem;
		int trapsToSpawnClone;
		int trapsArmedSinceLastSpawn;

	private:
		void handleTrapArmed() {
			++this->trapsArmedSinceLastSpawn;
			if (this->trapsArmedSinceLastSpawn >= this->trapsToSpawnClone) {
				Vector3d statuePosition = this->findGoodStatuePosition();

				Vector3d orientationDirection(randomNumber(-1.0f, 1.0f), 0.0f, randomNumber(-1.0f, 1.0f));
				orientationDirection.normalize();
				Quaternion orientation(orientationDirection);

				float timePosition = static_cast<float>(randomNumber(0, 600))*1.0f/30.0f;
				this->statueClonesSystem->requestSpawnClone(statuePosition, orientation, timePosition);

				this->trapsArmedSinceLastSpawn = 0;
			}
		}

		Vector3d findGoodStatuePosition() {
			ClosestConvexResultCallback callback;
			callback.addIgnoreObject(this->physicsComponent->getCollisionObject().get());
			callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;

			ConvexShape* shape = static_cast<ConvexShape*>(this->physicsComponent->getCollisionObject()->getCollisionShape());

			Vector3d statuePosition = this->physicsComponent->getPosition() + 0.1f*Vector3d::yUnit;
			Vector3d positionOffset;

			int attempts = 50;
			for (int i = 0; i < attempts; ++i) {
				positionOffset = Vector3d(randomNumber(-1.0f, 1.0f), 0.0f, randomNumber(-1.0f, 1.0f));
				positionOffset.normalize();
				positionOffset *= 4.0f;

				if (this->collisionTester->convexSweepTest(shape, toBulletTransform(statuePosition), toBulletTransform(statuePosition + positionOffset), callback)) {
					positionOffset *= callback.m_closestHitFraction;
				}
				if (positionOffset.norm() >= 1.0f) {
					return statuePosition + positionOffset - 0.1f*Vector3d::yUnit;
				}
			}
			return statuePosition + getUnitary(positionOffset) - 0.1f*Vector3d::yUnit;
		}
	};
	typedef SharedPointer<StatueCloneSpawner> StatueCloneSpawnerPtr;

	class LocalPlayerStatueController : public LocalStatueController, public InputHandler, public EventHandler {
	public:
		LocalPlayerStatueController(StatuePtr monster, StatueClonesSystemPtr statueClonesSystem, ScareTrapSystemPtr scareTrapSystem, DeathSystemPtr deathSystem, ManipulationSystemPtr manipulationSystem, SpectatorSystemPtr spectatorSystem, 
			RenderCameraPtr camera, ControlsPtr controls)
			: LocalStatueController(monster), EventHandler(60), statueClonesSystem(statueClonesSystem), scareTrapSystem(scareTrapSystem), deathSystem(deathSystem), manipulationSystem(manipulationSystem), 
			controls(controls), statue(monster)
		{
			LocalStatueComponentPtr localStatueComponent(new LocalStatueComponent(monster, scareTrapSystem));
			this->statue->getEntity()->addComponent(localStatueComponent);

			LocalStatueObjectViewComponentPtr objectViewComponent(new LocalStatueObjectViewComponent);
			this->statue->getEntity()->addComponent(objectViewComponent);

			LocalStatueHumansViewComponentPtr humansViewComponent(new LocalStatueHumansViewComponent);
			this->statue->getEntity()->addComponent(humansViewComponent);
			//Tentative change, Original: 10
			float transformRange = 6.0f;
			//SeekerLogicComponentPtr seekerLogicComponent(new SeekerLogicComponent(monster, transformRange));
			//this->statue->getEntity()->addComponent(seekerLogicComponent);
			this->p2Point = SeekerLogicComponentPtr(new SeekerLogicComponent(monster, transformRange));
			this->statue->getEntity()->addComponent(this->p2Point);

			//Tentative change, Original: 7
			float timeInDestroyerMode = 30.0f;
			//DestroyerLogicComponentPtr destroyerLogicComponent(new DestroyerLogicComponent(timeInDestroyerMode));
			//this->statue->getEntity()->addComponent(destroyerLogicComponent);
			this->p3Point = DestroyerLogicComponentPtr (new DestroyerLogicComponent(timeInDestroyerMode));
			this->statue->getEntity()->addComponent(this->p3Point);

			LocalStatueMessagesComponentPtr localStatueMessagesComponent(new LocalStatueMessagesComponent(controls));
			this->statue->getEntity()->addComponent(localStatueMessagesComponent);

			StatueCloneSpawnerPtr cloneSpawner(new StatueCloneSpawner(statueClonesSystem));
			this->statue->getEntity()->addComponent(cloneSpawner);
			this->statue->setPovType(FIRST_PERSON);

			this->armedTrapConnection = scareTrapSystem->connectScareTrapArmedEvent(Bind(&LocalPlayerStatueController::notifyTrapArmed, this, _1));

			camera->setOrientation(Ogre::Quaternion::IDENTITY);
			ActorPtr actor = this->monster->getActor();

			Vector3d offset(0.0f, 0.0f, 0.1f);
			BonePtr headBone = actor->getBone("bn_camera");
			this->cameraPositioner = CameraPositionerPtr(new BoneCameraPositioner(camera, actor, offset, headBone));

			CameraRotatorPtr cameraRotator(new CameraRotator(camera, actor, 80.0f));
			this->cameraController = CameraControllerPtr(new CameraController(cameraRotator, 0.005f, 1.0f));
			this->cameraController->setMouse(this->mouse);
			this->cameraController->setJoysticks(this->joysticks);

			MouseCursorPtr mouseCursor = MouseCursor::getInstance();

			StageComponentPtr stageComponent = this->statue->getEntity()->getComponent<StageComponent>();
			if (stageComponent->getStageName() == "Rose_Crimson_Hotel") {
				this->scareTrapSystem->reset(0.3f);
			} else if (stageComponent->getStageName() == "Factory") {
				this->scareTrapSystem->reset(0.6f);
			} else if (stageComponent->getStageName() == "Warehouse") {
				this->scareTrapSystem->reset(0.25f);
			} else if (stageComponent->getStageName() == "Pog_Champ_Hotel") {
				//Tentative change
				this->scareTrapSystem->reset(0.35f);
			}else {
				this->scareTrapSystem->reset(0.5f);
			}

			spectatorSystem->disable();
		}		

		virtual void destroy() {			
			this->monster->destroy();
			static_ptr_cast<Statue>(this->monster)->cleanup();
			this->cameraConnection.disconnect();
			this->armedTrapConnection.disconnect();
		}

		virtual ~LocalPlayerStatueController() {}

	protected:
		Connection armedTrapConnection;
		StatuePtr statue;
		CameraPositionerPtr cameraPositioner;
		CameraControllerPtr cameraController;
		StatueClonesSystemPtr statueClonesSystem;
		ScareTrapSystemPtr scareTrapSystem;
		DeathSystemPtr deathSystem;
		ManipulationSystemPtr manipulationSystem;
		Connection cameraConnection;
		ControlsPtr controls;

	private:
		//Expose Components to allow retrieval with getFunctions
		DestroyerLogicComponentPtr p3Point;
		SeekerLogicComponentPtr p2Point;
		void notifyTrapArmed(ScareTrapPtr scareTrap) {
			this->statue->incrementTrapsArmed();
		}

		virtual MonsterControllerType getType() {
			return LOCAL_MONSTER_CONTROLLER;
		}

		virtual bool handleInput(float diffTime) {
			const MouseState& mouseState = this->mouse->getMouseState();
			int xMouseRelative = (this->controls->invertMouseX) ? -mouseState.X.rel : mouseState.X.rel;
			int yMouseRelative = (this->controls->invertMouseY) ? -mouseState.Y.rel : mouseState.Y.rel;
			Vector3d cameraPosition(this->controls->mouseSensibility*static_cast<float>(xMouseRelative), this->controls->mouseSensibility*static_cast<float>(yMouseRelative), 0.0f);
			foreach (JoystickPtrMap::value_type entry, this->joysticks) {
				JoystickPtr joystick = entry.second;
				cameraPosition += joystick->getRightAxisPosition();
			}
			this->cameraController->update(static_cast<int>(cameraPosition.getX()), static_cast<int>(cameraPosition.getY()));	
			this->monster->updateManipulation(diffTime, cameraPosition.getX());

			bool move = false;
			float front = 0.0f;
			if (this->isButtonDown(this->controls->moveForward)) {
				move = true;
				front = 1.0f;
			}else if(this->isButtonDown(this->controls->moveBack)) {
				move = true;
				front = -0.5f;
			}
			float left = 0.0f;
			if (this->isButtonDown(this->controls->moveLeft)) {
				move = true;
				left = 1.0f;
			}else if(this->isButtonDown(this->controls->moveRight)) {
				move = true;
				left = -1.0f;
			}

			foreach(JoystickPtrMap::value_type entry, this->joysticks) {
				if (fabs(front) > 1.0e-3f || fabs(left) > 1.0e-3f) {
					break;
				}
				JoystickPtr joystick = entry.second;
				Vector3d position = joystick->getLeftAxisPosition();
				if (fabs(position.getX()) > 1.0e-3f) {
					if (position.getX() > 0.0f) {
						move = true;
						left = -1.0f;
					}else {
						move = true;
						left = 1.0f;
					}
				}
				if (fabs(position.getY()) > 1.0e-3f) {
					if (position.getY() > 0.0f) {
						move = true;
						front = -1.0f;
					}else {
						move = true;
						front = 1.0f;
					}
				}
			}

			if (!move) {				
				this->monster->idle();
				this->monsterActionRequest->stateInfo->idle();
				return true;
			}

			this->monster->walk(left, front);
			this->monsterActionRequest->stateInfo->walk(left, front);
			return true;
		}

		virtual void handlePause() {
			this->monster->idle();
			this->monsterActionRequest->stateInfo->idle();
		}

		virtual bool handleEvent(float diffTime) {
			this->monster->localUpdate(diffTime);
			return true;
		}

		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (button == this->controls->grab) {
				if ((this->statue->getState() == TRAPPER || this->statue->getState() == SEEKER) && this->monster->tryPickingItem()) {
					this->scareTrapSystem->requestArmTrap(this->monster->getItemId());
					return;
				}
				if (this->statue->getState() == DESTROYER) {
					if (this->monster->tryGrabbingObject()) {
						return;
					}
				}
			}
			if (button == this->controls->attack && this->statue->getState() == DESTROYER) {
				if (this->monster->tryAttacking()) {
					this->deathSystem->requestKillCharacter(CharacterType(this->monster->getAttackActorId()));
					return;
				}
			}
			if (button == this->controls->transform && this->statue->getState() == DESTROYER) {
				float tmpCurr = p3Point->getCurrP3Time();
				float tmpMax = p3Point->getMaxP3Time();
				float lastPhasing = p3Point->getLastPhasing();

				if (lastPhasing == 0.0f || tmpCurr >= lastPhasing+8.0f){
					if(tmpCurr+3.0f < tmpMax){
						p3Point->setLastPhasing(tmpCurr);
						this->statue->setState(TRAPPER);
						p3Point->doTriggerPhasing();
						//Core::getInstance()->runEvent(p2Point->getPhaseSequence());
						//this->statue->setState(DESTROYER);
					}
				}
			}

			//FOR DEBUG PURPOSES
			/*
			if (button == this->controls->switchFlashlight){
				if(this->statue->getState() == DESTROYER){
					this->statue->setState(TRAPPER);
				} else if(this->statue->getState() == TRAPPER){
					this->statue->setState(DESTROYER);
				}
			}
			if (button == this->controls->crouch){
				float tmpCurr = p3Point->getCurrP3Time();
				float lastPhasing = p3Point->getLastPhasing();
				std::cout << "\nCurrent Form Time: " << tmpCurr;
				std::cout << "\nLast Phasing: " << lastPhasing;
			}
			*/
		}

		virtual void buttonReleased(const ButtonCode button, const InputDevice device) {
			if (button == this->controls->grab) {
				this->monster->releaseObject();
			}
		}
	};

	typedef SharedPointer<LocalPlayerStatueController> LocalPlayerStatueControllerPtr;
}

#endif