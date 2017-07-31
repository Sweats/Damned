#ifndef TERROR_DEBUG_INPUT_HANDLER_H
#define TERROR_DEBUG_INPUT_HANDLER_H

#include "Config.h"

namespace terror {
	class DebugInputHandler : public InputHandler {
	public:
		DebugInputHandler(DebugDrawerPtr debugDrawer, DeferredShadingSystemPtr deferredShadingSystem) 
			: debugDrawer(debugDrawer), deferredShadingSystem(deferredShadingSystem) {}

		virtual ~DebugInputHandler() {}

	protected:
		DebugDrawerPtr debugDrawer;
		DeferredShadingSystemPtr deferredShadingSystem;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (button == BC_F5) {
				this->deferredShadingSystem->setMode(DEFERRED_SHADING);
			}
			if (button == BC_F6) {
				this->deferredShadingSystem->setMode(DEBUG_SHOW_COLOR);
			}
			if (button == BC_F7) {
				this->deferredShadingSystem->setMode(DEBUG_SHOW_NORMALS);
			}
			if (button == BC_F8) {
				this->deferredShadingSystem->setMode(DEBUG_SHOW_SPECULAR);
			}
			if (button == BC_F9) {
				MaterialPtr material = debugDrawer->getMaterial();
				material->setDepthCheckEnabled(true);
				material->setDepthWriteEnabled(true);
				if (this->debugDrawer->isEnabled()) {
					this->debugDrawer->disable();
				}else {
					this->debugDrawer->enable();
				}
			}
			if (button == BC_F10) {
				MaterialPtr material = debugDrawer->getMaterial();
				material->setDepthCheckEnabled(false);
				material->setDepthWriteEnabled(false);
				if (this->debugDrawer->isEnabled()) {
					this->debugDrawer->disable();
				}else {
					this->debugDrawer->enable();
				}
			}			
			if (button == BC_F12) {
				Ogre::TextureManager::ResourceMapIterator entry = Ogre::TextureManager::getSingletonPtr()->getResourceIterator();
				while (entry.hasMoreElements()) {
					TexturePtr texture = entry.getNext().staticCast<Texture>();
					if (ends_with(texture->getName(), ".dds")) {
						texture->reload();
					}
				}
			}		
		}
	};
	typedef SharedPointer<DebugInputHandler> DebugInputHandlerPtr;
}

#endif