#ifndef TERROR_MENU_SCENE_LOADER_H
#define TERROR_MENU_SCENE_LOADER_H

#include "Config.h"

namespace terror {
	class MenuSceneLoader /*Temp*/ : public EventHandler /**/ {
	public:
		MenuSceneLoader(CorePtr core, SceneFactoryPtr sceneFactory, SceneDataPtr menuSceneData)
			: EventHandler(30), core(core), sceneFactory(sceneFactory), menuSceneData(menuSceneData), light(0), isInMenu(false), timeToStartAmbientMusic(0.0f) {}

		void loadMenuScene() {
			if (this->isInMenu) {
				return;
			}
			this->isInMenu = true;

			this->menuScene = this->sceneFactory->create(this->menuSceneData);
			this->menuScene->stopAmbientMusic();
			this->timeToStartAmbientMusic = 0.5f;

			RenderCameraPtr camera = core->getGraphicsSystem()->getCamera();
			SpawnPointPtr spawnPoint = this->menuScene->getSpawnPoint(0);
			camera->setPosition(toOgre(spawnPoint->getPosition()));
			camera->setOrientation(toOgre(spawnPoint->getOrientation()));
			camera->yaw(Ogre::Degree(180.0f));

			/**/
			this->dust = this->core->getSceneManager()->createParticleSystem("MainMenuDust", "Dust");
			this->dust->setCastShadows(false); //in order to make point billboards work on deferred.
			this->dustNode = this->core->getSceneManager()->getRootSceneNode()->createChildSceneNode();
			this->dustNode->attachObject(this->dust);	
			this->dustNode->setPosition(toOgre(spawnPoint->getPosition()));
			this->dustNode->setOrientation(toOgre(spawnPoint->getOrientation()));

			this->light = this->core->getSceneManager()->createLight();
			this->light->setType(Light::LT_SPOTLIGHT);
			this->light->setDiffuseColour(ColourValue(1.0f, 0.760784388f, 0.517647088f));
			this->light->setSpecularColour(ColourValue(1.0f, 0.760784388f, 0.517647088f));
			this->light->setAttenuation(12.0f, 10.173404f, 0.0f, 0.0f);
			this->light->setSpotlightRange(Ogre::Degree(90.0f), Ogre::Degree(120.0f), 1.0f);
			this->light->setDirection(0.724776626f, -0.481752932f, 0.492557734f);
			this->light->setPosition(-4.080299f, 0.0f, -10.054008f);
			/**/
		}

		void unloadMenuScene() {
			if (!this->isInMenu) {
				return;
			}
			this->isInMenu = false;

			this->menuScene->destroy();

			/**/
			Core::getInstance()->destroySceneNode(this->dustNode);

			this->core->getSceneManager()->destroyLight(this->light);			
			this->light = 0;
			/**/
		}

		virtual ~MenuSceneLoader() {}

	protected:
		CorePtr core;
		SceneFactoryPtr sceneFactory;
		SceneDataPtr menuSceneData;		
		ScenePtr menuScene;
		bool isInMenu;
		float timeToStartAmbientMusic;

		/**/
		ParticleSystemPtr dust;
		SceneNodePtr dustNode;
		LightPtr light;
		/**/

		/**/
	private:
		virtual bool handleEvent(float diffTime) {
			if (this->timeToStartAmbientMusic > 0.0f) {
				this->timeToStartAmbientMusic -= diffTime;
				if (this->timeToStartAmbientMusic < 0.0f) {
					this->menuScene->startAmbientMusic();
				}
			}

			static bool transition = false;
			static bool state = true;
			static float alpha = 1.0f;
			static float timeToShift = 0.05f;
			static float timeSinceLastShift = 0.0f;
			static float timeSinceLastRandom = 0.0f;	

			if (!this->light) {
				return true;
			}

			if (transition) {
				if (state) {
					alpha -= diffTime/timeToShift;
					if (alpha <= 0.0f) {
						alpha = 0.0f;
						state = false;
						transition = false;
						timeSinceLastShift = 0.0f;
					}					
				}else {
					alpha += diffTime/timeToShift;
					if (alpha >= 1.0f) {
						alpha = 1.0f;
						state = true;
						transition = false;
						timeSinceLastShift = 0.0f;
					}
				}
				this->light->setDiffuseColour(ColourValue(alpha*1.0f, alpha*0.760784388f, alpha*0.517647088f));
				this->light->setSpecularColour(ColourValue(alpha*1.0f, alpha*0.760784388f, alpha*0.517647088f));
				return true;
			}	

			timeSinceLastShift += diffTime;
			timeSinceLastRandom += diffTime;
			if (timeSinceLastShift > 0.1f && timeSinceLastRandom > 0.1f) {
				timeSinceLastRandom = 0.0f;
				float rand = randomNumber(1, 10);				
				if (state && rand <= 1 || !state && rand <= 4) {
					timeToShift = 0.05f + randomNumber(0.0f, 0.1f);
					transition = true;
				}
			}

			return true;
		}
		/**/
	};

	typedef SharedPointer<MenuSceneLoader> MenuSceneLoaderPtr;
}

#endif