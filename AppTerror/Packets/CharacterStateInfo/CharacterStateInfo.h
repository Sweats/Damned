#ifndef TERROR_CHARACTER_STATE_INFO_H
#define TERROR_CHARACTER_STATE_INFO_H

#include "Packets/Vector3dInfo/Vector3dInfo.h"
#include "Packets/QuaternionInfo/QuaternionInfo.h"

namespace terror {
	class CharacterStateInfo {
	public:
		friend class boost::serialization::access;

		CharacterStateInfo() : visible(true), crouched(false), idled(true), sprinting(false), running(false), leftMove(0.0f), frontMove(0.0f), state(0) {}

		void setState(int state) {
			this->state = state;
		}

		int getState() {
			return this->state;
		}

		void setTransform(const Vector3d& position, const Quaternion& orientation) {
			this->position.setVector3d(position);
			this->orientation.setQuaternion(orientation);
		}

		void setPosition(const Vector3d& position) {
			this->position.setVector3d(position);
		}

		void setCrouched(bool crouched) {
			this->crouched = crouched;
		}

		void setVisible(bool visible) {
			this->visible = visible;
		}

		void idle() {
			this->idled = true;
			this->running = false;
			this->sprinting = false;
			this->leftMove = 0.0f;
			this->frontMove = 0.0f;
		}

		void walk(float leftMove, float frontMove) {
			this->idled = false;
			this->running = false;
			this->sprinting = false;
			this->leftMove = leftMove;
			this->frontMove = frontMove;
		}

		void run(float leftMove, float frontMove) {
			this->idled = false;
			this->running = true;
			this->sprinting = false;
			this->leftMove = leftMove;
			this->frontMove = frontMove;
		}

		void sprint(float leftMove, float frontMove) {
			this->idled = false;
			this->running = false;
			this->sprinting = true;
			this->leftMove = leftMove;
			this->frontMove = frontMove;
		}

		Vector3d getPosition() const {
			return this->position.buildVector3d();
		}

		Quaternion getOrientation() const {
			return this->orientation.buildQuaternion();
		}

		bool isVisible() const {
			return this->visible;
		}

		bool isCrouched() const {
			return this->crouched;
		}

		bool isIdle() const {
			return this->idled;
		}

		bool isRunning() const {
			return this->running;
		}

		bool isSprinting() const {
			return this->sprinting;
		}

		float getLeftMove() const {
			return this->leftMove;
		}

		float getFrontMove() const {
			return this->frontMove;
		}

		virtual ~CharacterStateInfo() {}

	protected:
		Vector3dInfo position;
		QuaternionInfo orientation;
		bool visible;
		bool crouched;
		bool idled;
		bool running;
		bool sprinting;
		float leftMove;
		float frontMove;
		int state;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->position;
			archive & this->orientation;
			archive & this->visible;
			archive & this->crouched;
			archive & this->idled;
			archive & this->running;
			archive & this->sprinting;
			archive & this->leftMove;
			archive & this->frontMove;
			archive & this->state;
		}
	};

	typedef SharedPointer<CharacterStateInfo> CharacterStateInfoPtr;
}

#endif