#ifndef TERROR_CONTROLS_H
#define TERROR_CONTROLS_H

#include "Config.h"

namespace terror {
	struct Controls {
		friend class boost::serialization::access;

		void loadDefaults() {
			this->moveForward = BC_W;
			this->moveBack = BC_S;
			this->moveLeft = BC_A;
			this->moveRight = BC_D;
			this->slowWalk = BC_LMENU;
			this->crouch = BC_LCONTROL;
			this->run = BC_LSHIFT;
			this->pushToTalk = BC_C;
			this->pause = BC_ESCAPE;
			this->transform = BC_E;
			this->grab = BC_MOUSE_LEFT;
			this->switchFlashlight = BC_MOUSE_RIGHT;
			this->attack = BC_MOUSE_LEFT;
			this->teleport = BC_MOUSE_RIGHT;
			this->start = BC_RETURN;
			this->spectateNext = BC_MOUSE_LEFT;
			this->guiAccept = BC_MOUSE_LEFT;
			this->guiReturn = BC_ESCAPE;
			this->tab = BC_TAB;

			this->mouseSensibility = 1.25f;
			this->invertMouseX = false;
			this->invertMouseY = false;
		}

		ButtonCode moveForward;
		ButtonCode moveBack;
		ButtonCode moveLeft;
		ButtonCode moveRight;
		ButtonCode slowWalk;
		ButtonCode crouch;
		ButtonCode run;
		ButtonCode pause;
		ButtonCode screenshot;
		ButtonCode pushToTalk;
		ButtonCode transform;
		ButtonCode grab;
		ButtonCode switchFlashlight;
		ButtonCode attack;
		ButtonCode teleport;
		ButtonCode start;
		ButtonCode spectateNext;
		ButtonCode guiAccept;
		ButtonCode guiReturn;
		ButtonCode tab;

		float mouseSensibility;
		bool invertMouseX;
		bool invertMouseY;

	private:
		template<class Archive_>
		void serialize(Archive_& archive, const unsigned int version) {
			archive & this->moveForward;
			archive & this->moveBack;
			archive & this->moveLeft;
			archive & this->moveRight;
			archive & this->slowWalk;
			archive & this->crouch;
			archive & this->run;
			archive & this->pushToTalk;
			archive & this->pause;
			archive & this->transform;
			archive & this->grab;
			archive & this->switchFlashlight;
			archive & this->attack;
			archive & this->teleport;
			archive & this->start;
			archive & this->spectateNext;
			archive & this->guiAccept;
			archive & this->guiReturn;
			archive & this->tab;

			archive & this->mouseSensibility;
			archive & this->invertMouseX;
			archive & this->invertMouseY;
		}
	};

	typedef SharedPointer<Controls> ControlsPtr;
}

#endif