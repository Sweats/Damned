#ifndef TERROR_FLASHLIGHT_H
#define TERROR_FLASHLIGHT_H

#include "Config.h"
#include "Human/Flashlight/Battery/Battery.h" 
#include "MathModels/Interpolator/ExponentialInterpolator/ExponentialInterpolator.h" 

namespace terror {
	class Flashlight {
	public:

		Flashlight(MeshInstancePtr mesh, LightPtr light, SoundPtr switchSound, RenderCameraPtr camera, ColourValue baseColor = ColourValue(1.0f,1.0f,1.0f)) 
			: mesh(mesh), light(light), flare(0), flareNode(0), switchSound(switchSound), camera(camera), on(false), battery(BatteryPtr(new Battery)), 
			lightIntensityModel(new ExponentialInterpolator(0.0f, 0.0f, 1.0f, 1.0f, 10.0f)), baseColor(baseColor), lightIntensityFactor(1.0f), timeForLightTransition(1.0f), smallestLightIntensityFactor(0.2)
		{
			Ogre::TagPoint* lightTagPoint = this->mesh->attachObjectToBone("be_flashlight", this->light,
				Ogre::Quaternion(0.707107f, 0.0f, -0.707107f, 0.0f));
			lightTagPoint->setInheritScale(false);
			this->initializeLight();
			this->updateMaterialName();
		}

		Flashlight(MeshInstancePtr mesh, LightPtr light, Ogre::BillboardSet* flare, SoundPtr switchSound, RenderCameraPtr camera, ColourValue baseColor = ColourValue(1.0f,1.0f,1.0f)) 
			: mesh(mesh), light(light), flare(flare), switchSound(switchSound), camera(camera), on(false), battery(BatteryPtr(new Battery)), 
			lightIntensityModel(new ExponentialInterpolator(0.0f, 0.0f, 1.0f, 1.0f, 10.0f)), baseColor(baseColor), lightIntensityFactor(1.0f), timeForLightTransition(1.0f), smallestLightIntensityFactor(0.2) 
		{
			Ogre::TagPoint* lightTagPoint = this->mesh->attachObjectToBone("be_flashlight", this->light,
				Ogre::Quaternion(0.707107f, 0.0f, -0.707107f, 0.0f));
			lightTagPoint->setInheritScale(false);
			
			this->initializeLight();

			this->flareNode = this->flare->_getManager()->getRootSceneNode()->createChildSceneNode();
			this->flareNode->attachObject(this->flare);
			this->flareNode->setVisible(this->on);

			this->updateMaterialName();
		}

		void updateLightIntensity() {
			ColourValue updatedColor = this->lightIntensityFactor*this->lightIntensityModel->interpolate(this->battery->getCharge())*this->baseColor; 
			this->light->setDiffuseColour(updatedColor);
			this->light->setSpecularColour(updatedColor);
		}

		void updateLightIntensityFactor(float diffTime, bool monsterInRange){
			if (monsterInRange) {
				this->lightIntensityFactorInterpolator.setData(this->lightIntensityFactor, this->smallestLightIntensityFactor, this->timeForLightTransition);
			}else {
				this->lightIntensityFactorInterpolator.setData(this->lightIntensityFactor, 1.0, this->timeForLightTransition);
			}
			this->lightIntensityFactor = this->lightIntensityFactorInterpolator.update(diffTime);
		}

		void updatePosition(const Vector3d& bonePosition, const Quaternion& boneOrientation) { 
			if (!this->flareNode) {
				return;
			}
			if (!this->light->getVisible()) {
				this->flareNode->setVisible(false);
				return;
			}
			Vector3d position = bonePosition + 0.1f*boneOrientation.getXAxis();
			Vector3d cameraPosition = toAlgebra(this->camera->getPosition());
			Vector3d flareToCam = cameraPosition - position;
			flareToCam.normalize();
			this->flareNode->setPosition(toOgre(position + 0.1f*flareToCam));			

			Vector3d boneDirection = boneOrientation.getXAxis();
			float camDotFlare = dot(flareToCam, boneDirection);
			if (camDotFlare < 0.5f) {
				this->flareNode->setVisible(false);
			}else {
				this->flareNode->setVisible(true);
				float size = 0.75f*(camDotFlare - 0.1f)/0.9f;
				float lightIntensity = this->lightIntensityFactor*this->lightIntensityModel->interpolate(this->battery->getCharge());
				this->flare->getBillboard(0)->setDimensions(size*lightIntensity, size*lightIntensity);
			}
		}

		MeshInstancePtr getMesh() {
			return this->mesh;
		}

		LightPtr getLight() {
			return this->light;
		}

		BatteryPtr getBattery() {
			return this->battery;
		}

		bool isOn() {
			return this->on;
		}

		void setOn(bool on, bool visible, bool playSound = true) {
			if (this->on == on) {
				return;
			}
			this->on = on;
			if (visible) {
				this->light->setVisible(this->on);
				this->updateBatteryState();
				this->updateMaterialName();
				if (playSound && this->switchSound) {
					this->switchSound->play();
				}
				if (this->flareNode) {
					this->flareNode->setVisible(this->on);
				}				
			}else {
				this->light->setVisible(false);
				this->battery->stop();
				if (this->flareNode) {
					this->flareNode->setVisible(false);
				}
			}
		}

		void notifyVisible() {
			this->light->setVisible(this->on);	
			this->updateBatteryState();
			this->updateMaterialName();
			if (this->flareNode) {
				this->flareNode->setVisible(this->on);
			}			
		}

		void notifyInvisible() {
			this->light->setVisible(false);
			this->battery->stop();
			if (this->flareNode) {
				this->flareNode->setVisible(false);
			}
		}

		void setLightIntensityFactor(float lightIntensityFactor) {
			this->lightIntensityFactor = clamp(lightIntensityFactor, 0.0f, 1.0f);
		}

		float getLightIntensityFactor() {
			return this->lightIntensityFactor;
		}

		void destroy() {
			if (this->flareNode) {
				Core::getInstance()->destroySceneNode(this->flareNode);
			}
			this->light->_getManager()->destroyLight(this->light);
		}

		virtual ~Flashlight() {}

	protected:
		MeshInstancePtr mesh;
		LightPtr light;
		Ogre::BillboardSet* flare;
		SceneNodePtr flareNode;
		SoundPtr switchSound;
		RenderCameraPtr camera;
		BatteryPtr battery;
		ColourValue baseColor;
		InterpolatorPtr lightIntensityModel;
		float lightIntensityFactor;
		float timeForLightTransition;
		float smallestLightIntensityFactor;
		FloatInterpolator lightIntensityFactorInterpolator;
		bool on;

	private:
		void updateMaterialName() {
			if (this->on) {
				this->mesh->setMaterialName("flashlight_on_mat");
			}else {
				this->mesh->setMaterialName("flashlight_mat");
			}
		}

		void initializeLight() {
			this->light->setType(Light::LT_SPOTLIGHT);
			this->light->setDiffuseColour(this->baseColor);
			this->light->setSpecularColour(this->baseColor);
			this->light->setAttenuation(20.0f, 10.107298f, 0.0f, 0.0f);
			this->light->setSpotlightRange(Ogre::Degree(59.0f), Ogre::Degree(60.0f), 1.0f);
			this->light->setDirection(0.0f, 0.0f, -1.0f);
			this->light->setVisible(this->on);		
		}

		void updateBatteryState() {
			if (this->on) {
				this->battery->start();
			}else {
				this->battery->stop();
			}
		}
	};

	typedef SharedPointer<Flashlight> FlashlightPtr;
}

#endif