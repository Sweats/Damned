#ifndef TERROR_GHOST_H
#define TERROR_GHOST_H

#include "Monsters/Energy.h"

namespace terror {
	class Ghost {
	public:
		Ghost(ActorPtr actor, SoundPtr changeStateSound) 
			: actor(actor), changeStateSound(changeStateSound), energy(new Energy(100.0f, 100.0f/50.0f, 100.0f/40.0f)), visible(true) 
		{
			this->becomeInvisible();			
		}

		Connection connectBecomeInvisibleEvent(const BasicSignal::slot_type& becomeInvisibleEvent) {
			Connection connection = this->becomeInvisibleSignal.connect(becomeInvisibleEvent);
			if (!this->visible) {
				this->becomeInvisibleSignal();				
			}
			return connection;
		}

		Connection connectBecomeVisibleEvent(const BasicSignal::slot_type& becomeVisibleEvent) {
			Connection connection = this->becomeVisibleSignal.connect(becomeVisibleEvent);
			if (this->visible) {
				this->becomeVisibleSignal();				
			}
			return connection;
		}

		void connectEnergyMaxEvent(const BasicSignal::slot_type& energyMaxEvent) {
			this->energyMaxSignal.connect(energyMaxEvent);
		}

		void update(float diffTime) {
			if (!this->visible) {
				if (!this->energy->isMax()) {
					this->energy->regenerate(diffTime);
					if (this->energy->isMax()) {
						this->energyMaxSignal();
					}
				}
				return;
			}
			this->energy->consume(diffTime);
			if (this->energy->isNull()) {
				this->becomeInvisible();
			}
		}

		bool isVisible() {
			return this->visible;
		}

		bool becomeInvisible() {
			if (!this->visible) {
				return false;
			}
			this->actor->hide();
			this->actor->enableGhostMode();
			this->energy->spend(45.0f);
			//this->energy->consumeAll();
			//this->energy->setRegenerationRate(randomNumber(10.0f/100.0f, 10.0f/80.0f));
			//this->energy->setRegenerationRate(10.0f);			
			if (this->changeStateSound) {
				this->changeStateSound->stop();
				this->changeStateSound->play();
			}
			this->becomeInvisibleSignal();
			this->visible = false;
			return true;
		}

		bool becomeVisible(bool force = false) {
			if (this->visible) {
				return false;
			}
			if (!force && !this->energy->isMax()) {
				return false;
			}
			this->actor->show();
			this->actor->disableGhostMode();
			//this->energy->setConsumptionRate(randomNumber(10.0f/50.0f, 10.0f/30.0f));
			//this->energy->setConsumptionRate(0.01f);			
			if (this->changeStateSound) {
				this->changeStateSound->stop();
				this->changeStateSound->play();
			}
			this->becomeVisibleSignal();
			this->visible = true;
			return true;
		}

		virtual ~Ghost() {}

	protected:
		ActorPtr actor;
		EnergyPtr energy;
		SoundPtr changeStateSound;
		bool visible;
		BasicSignal becomeInvisibleSignal;
		BasicSignal becomeVisibleSignal;		
		BasicSignal energyMaxSignal;		
	};

	typedef SharedPointer<Ghost> GhostPtr;
}

#endif