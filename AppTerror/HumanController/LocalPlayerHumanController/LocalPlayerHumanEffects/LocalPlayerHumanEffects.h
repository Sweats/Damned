#ifndef TERROR_LOCAL_PLAYER_HUMAN_EFFECTS_H
#define TERROR_LOCAL_PLAYER_HUMAN_EFFECTS_H

#include "Monsters/Spectrum/Spectrum.h"
#include "Monsters/Berserker/Berserker.h"

namespace terror {
	typedef std::vector<SoundPtr> SoundStreamPtrVector;
	class LocalPlayerHumanEffects {
	public:
		LocalPlayerHumanEffects(StatueClonesSystemPtr statueClonesSystem, CollisionTesterPtr collisionTester, MonsterPtr monster, SoundPtr monsterAroundSound,
			SoundPtr monsterCloseSound, SoundPtr monsterCloserSound, SoundPtr blindCloseSound, SoundStreamPtrVector ghostThroughSounds, SoundPtr berserkerInBerserkerStateSound, 
			SoundPtr berserkerInGhostStateSound, PostProcessingEffectPtr deathEffect, PostProcessingEffectPtr seeingStatueEffect)
			: collisionTester(collisionTester), monster(monster), monsterAroundSound(monsterAroundSound), monsterCloseSound(monsterCloseSound), 
			monsterCloserSound(monsterCloserSound), blindCloseSound(blindCloseSound), ghostThroughSounds(ghostThroughSounds), berserkerInBerserkerStateSound(berserkerInBerserkerStateSound),
			berserkerInGhostStateSound(berserkerInGhostStateSound), deathEffect(deathEffect), monsterAround(false), monsterClose(false), monsterCloser(false), ghostThrough(false), 
			monsterVisible(true), monsterInBerserkerState(false), dead(false), dieEffectsTime(-1.0f), framesToDiscard(0), statueClonesSystem(statueClonesSystem), seeingStatueEffect(seeingStatueEffect)
		{
			if (monster->getType() == SPECTRUM) {
				SpectrumPtr spectrum = static_ptr_cast<Spectrum>(monster);
				this->notifyMonsterInvisibleConnection = spectrum->connectBecomeInvisibleEvent(Bind(&LocalPlayerHumanEffects::notifyMonsterInvisible, this));
				this->notifyMonsterVisibleConnection = spectrum->connectBecomeVisibleEvent(Bind(&LocalPlayerHumanEffects::notifyMonsterVisible, this));
			}else if (monster->getType() == BERSERKER) {
				BerserkerPtr berserker = static_ptr_cast<Berserker>(monster);
				this->notifyMonsterBecomeBerserkerConnection = berserker->connectBecomeBerserkerEvent(Bind(&LocalPlayerHumanEffects::notifyMonsterBecomeBerserker, this));
				this->notifyMonsterBecomeGhostConnection = berserker->connectBecomeGhostEvent(Bind(&LocalPlayerHumanEffects::notifyMonsterBecomeGhost, this));
			}
		}

		void updateEffects(RenderCameraPtr camera) {
			Vector3d camPosition = toAlgebra(camera->getPosition());
			Vector3d eyeDirection = toAlgebra(camera->getDirection());

			/*{
				bool effectState = false;
				foreach(EntityIntPtrMap::value_type& entry, this->statueClonesSystem->getClonesMap()) {
					entities::EntityPtr clone = entry.second;
					CloneObjectComponentPtr objectComponent = clone->getComponent<CloneObjectComponent>();					
					Vector3d position = objectComponent->getPosition();

					Vector3d direction = position - camPosition;
					if (direction.horizontalNorm() > 30.0f) {
						continue;
					}

					direction.normalize();
					if (dot(eyeDirection, direction) < 0.9) {
						continue;
					}

					ClosestRayResultCallback callback;
					callback.addIgnoreObject(objectComponent->getCollisionObject().get());
					callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
					if (!this->collisionTester->rayTest(toBullet(camPosition), toBullet(position), callback)) {
						effectState = true;
					}
				}
				this->seeingStatueEffect->setEnabled(effectState);
			}*/
			
			Vector3d monsterPosition = this->monster->getActor()->getPosition();
			Vector3d camToMonster = monsterPosition - camPosition;
			float monsterDistance = camToMonster.norm();

			if (!this->monsterVisible) {
				if (monsterDistance > 1.5f) {
					this->ghostThrough = false;
				}
				if (monsterDistance < 1.0f && !this->ghostThrough) {
					this->ghostThrough = true;
					SoundPtr ghostThroughSound = this->ghostThroughSounds[randomNumber(0, this->ghostThroughSounds.size() - 1)];
					if (ghostThroughSound) {
						ghostThroughSound->play();
					}
				}
				return;
			}

			camToMonster.normalize();			
			bool isInFov = (dot(eyeDirection, camToMonster) > 0.9f) ? true : false;
			if (isInFov) {
				ClosestRayResultCallback callback;
				callback.m_collisionFilterMask = btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::StaticFilter;
				if (this->collisionTester->rayTest(toBullet(camPosition), toBullet(monsterPosition), callback)) {
					isInFov = false;
				}
			}

			if (monster->getType() == SPECTRUM) {
				if (isInFov && monsterDistance <= 10.0f && !this->monsterCloser) {
					this->monsterCloser = true;
					this->monsterClose = true;
					this->monsterAround = true;
					if (this->monsterAroundSound) {
						this->monsterAroundSound->stop();
					}
					if (this->monsterCloseSound) {
						this->monsterCloseSound->stop();
					}
					if (this->monsterCloserSound) {
						this->monsterCloserSound->play();
					}
					return;
				}
				if (monsterDistance > 15.0f && this->monsterCloser) {
					this->monsterCloser = false;
					if (this->monsterCloserSound) {
						this->monsterCloserSound->stop();
					}
					if (this->monsterCloseSound) {
						this->monsterCloseSound->play();
					}
					return;
				}
				if (/*isInFov && */monsterDistance <= 15.0f && !this->monsterClose) {
					this->monsterClose = true;
					this->monsterAround = false;
					if (this->monsterAroundSound) {
						this->monsterAroundSound->stop();
					}
					if (this->monsterCloseSound) {
						this->monsterCloseSound->play();
					}
					return;
				}
				/*
				if (monsterDistance > 25.0f && this->monsterClose) {
					this->monsterClose = false;
					if (this->monsterCloseSound) {
						this->monsterCloseSound->stop();
					}
					return;
				}
				*/
				if (!this->monsterAround) {
					this->monsterAround = true;
					return;
				}
			}else if (monster->getType() == BLIND) {
				if (isInFov && monsterDistance <= 10.0f && !this->monsterCloser) {
					if (this->blindCloseSound) {
						this->monsterCloser = true;
						this->blindCloseSound->play();
					}
					if (this->monsterAroundSound) {
						this->monsterAround = false;
						this->monsterAroundSound->stop();
					}
					return;
				}
				if (monsterDistance > 12.0f && this->monsterCloser) {
					if (this->blindCloseSound) {
						this->monsterCloser = false;
						this->blindCloseSound->stop();
					}
					if (!this->monsterAroundSound) {
						this->monsterAround = true;
						this->monsterAroundSound->play();
					}
					return;
				}
				if (monsterDistance <= 13.0f && !this->monsterCloser && !this->monsterAround) {
					if (this->monsterAroundSound) {
						this->monsterAround = true;
						this->monsterAroundSound->play();
					}
					return;
				}
				if (monsterDistance > 15.0f && !this->monsterCloser && this->monsterAround) {
					if (this->monsterAroundSound) {
						this->monsterAround = false;
						this->monsterAroundSound->stop();
					}
					return;
				}
			}else if (monster->getType() == BERSERKER) {
				if (isInFov && monsterDistance <= 7.0f && (!this->monsterClose || this->berserkerStateChanged)) {
					this->berserkerStateChanged = false;
					this->monsterClose = true;
					if (this->monsterInBerserkerState) {
						if (this->berserkerStateChanged) {
							if (this->berserkerInGhostStateSound) {
								this->berserkerInGhostStateSound->stop();
							}
						}
						if (this->berserkerInBerserkerStateSound) {
							this->berserkerInBerserkerStateSound->play();
						}
					}else {
						if (this->berserkerStateChanged) {
							if (this->berserkerInBerserkerStateSound) {
								this->berserkerInBerserkerStateSound->stop();
							}
						}
						if (this->berserkerInGhostStateSound) {
							this->berserkerInGhostStateSound->play();
						}
					}
					return;
				}
				if (monsterDistance > 10.0f && this->monsterClose) {
					this->monsterClose = false;
					if (this->berserkerInBerserkerStateSound) {
						this->berserkerInBerserkerStateSound->stop();
					}
					if (this->berserkerInGhostStateSound) {
						this->berserkerInGhostStateSound->stop();
					}
				}
			}
		}

		void updateDieEffects(float diffTime) {
			if (this->dieEffectsTime <= 0.0f) {
				return;
			}
			if (this->framesToDiscard > 0) {
				--this->framesToDiscard;
				return;
			}

			this->dieEffectsTime -= diffTime;
			if (this->dieEffectsTime > 0.0f) {
				return;
			}	
			this->dieEffectsTime = -1.0f;
		}

		void playDieEffects() {
			if (this->dead) {
				return;
			}
			this->framesToDiscard = 1;
			this->dieEffectsTime = 0.2f;
			this->dead = true;
		}

		void reset() {
			if (this->monsterAroundSound) {
				this->monsterAroundSound->stop();
			}
			if (this->monsterCloseSound) {
				this->monsterCloseSound->stop();
			}
			if (this->monsterCloserSound) {
				this->monsterCloserSound->stop();
			}
			if (this->blindCloseSound) {
				this->blindCloseSound->stop();
			}
			foreach (SoundPtr ghostThroughSound, this->ghostThroughSounds) {
				if (ghostThroughSound) {
					ghostThroughSound->stop();
				}
			}
			if (this->berserkerInBerserkerStateSound) {
				this->berserkerInBerserkerStateSound->stop();
			}
			if (this->berserkerInGhostStateSound) {
				this->berserkerInGhostStateSound->stop();
			}
			this->monsterAround = false;
			this->monsterClose = false;
			this->monsterCloser = false;
			this->ghostThrough = false;

			this->notifyMonsterInvisibleConnection.disconnect();
			this->notifyMonsterVisibleConnection.disconnect();

			this->notifyMonsterBecomeBerserkerConnection.disconnect();
			this->notifyMonsterBecomeGhostConnection.disconnect();
		}

		virtual ~LocalPlayerHumanEffects() {
			Core::getInstance()->getGraphicsSystem()->getPostProcessingSystem()->removePostProcessingEffect("SeeingStatueEffect");
		}

	protected:
		CollisionTesterPtr collisionTester;
		MonsterPtr monster;
		SoundPtr monsterAroundSound;
		SoundPtr monsterCloseSound;
		SoundPtr monsterCloserSound;
		SoundPtr blindCloseSound;
		SoundStreamPtrVector ghostThroughSounds;
		SoundPtr berserkerInGhostStateSound;
		SoundPtr berserkerInBerserkerStateSound;
		bool monsterAround;
		bool monsterClose;
		bool monsterCloser;
		bool ghostThrough;
		bool monsterVisible;
		bool monsterInBerserkerState;
		bool berserkerStateChanged;

		Connection notifyMonsterInvisibleConnection;
		Connection notifyMonsterVisibleConnection;

		Connection notifyMonsterBecomeBerserkerConnection;
		Connection notifyMonsterBecomeGhostConnection;

		StatueClonesSystemPtr statueClonesSystem;
		PostProcessingEffectPtr seeingStatueEffect;

		PostProcessingEffectPtr deathEffect;
		float dieEffectsTime;
		int framesToDiscard;
		bool dead;

	private:
		void notifyMonsterInvisible() {
			if (this->monsterAroundSound) {
				this->monsterAroundSound->stop();
			}
			if (this->monsterCloseSound) {
				this->monsterCloseSound->stop();
			}
			if (this->monsterCloserSound) {
				this->monsterCloserSound->stop();
			}
			this->monsterAround = false;
			this->monsterClose = false;
			this->monsterCloser = false;
			this->monsterVisible = false;
		}

		void notifyMonsterVisible() {
			this->ghostThrough = false;
			this->monsterVisible = true;
		}

		void notifyMonsterBecomeBerserker() {
			if (this->berserkerInGhostStateSound) {
				this->berserkerInGhostStateSound->stop();
			}
			this->monsterInBerserkerState = true;
			this->berserkerStateChanged = true;
		}

		void notifyMonsterBecomeGhost() {
			if (this->berserkerInBerserkerStateSound) {
				this->berserkerInBerserkerStateSound->stop();
			}
			this->monsterInBerserkerState = false;
			this->berserkerStateChanged = true;

		}
	};

	typedef SharedPointer<LocalPlayerHumanEffects> LocalPlayerHumanEffectsPtr;
}

#endif