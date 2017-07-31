#ifndef TERROR_PARTICLE_FX_HANDLER_H
#define TERROR_PARTICLE_FX_HANDLER_H

#include "TerrorSave/TerrorSave.h"

namespace terror {
	class ParticleFxHandler {
	public:
		ParticleFxHandler(SceneManagerPtr sceneManager, TerrorSavePtr terrorSave) 
			: sceneManager(sceneManager), terrorSave(terrorSave) {}

		void enable() {
			if (this->actor) {
				ParticleSystemPtr dust = this->sceneManager->createParticleSystem("Dust", "Dust");
				dust->setCastShadows(false); //in order to make point billboards work on deferred.
				this->actor->getNode()->attachObject(dust);
			}
		}

		void disable() {
			if (this->actor) {
				this->sceneManager->destroyParticleSystem("Dust");
			}
		}

		void setLocalActor(ActorPtr actor) {
			this->actor = actor;
			if (this->terrorSave->particleFx) {
				this->enable();
			}			
		}

		void resetLocalActor() {
			if (this->terrorSave->particleFx) {
				this->disable();
			}
			this->actor.reset();			
		}

		virtual ~ParticleFxHandler() {}

	protected:
		SceneManagerPtr sceneManager;
		TerrorSavePtr terrorSave;
		ActorPtr actor;
	};

	typedef SharedPointer<ParticleFxHandler> ParticleFxHandlerPtr;
}

#endif