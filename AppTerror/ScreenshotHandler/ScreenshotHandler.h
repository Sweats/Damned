#ifndef TERROR_SCREENSHOT_HANDLER_H
#define TERROR_SCREENSHOT_HANDLER_H

#include "Config.h"

namespace terror {
	class ScreenshotHandler : public InputHandler {
	public:
		ScreenshotHandler(RenderWindowPtr renderWindow, const string& screenshotDir) 
			: renderWindow(renderWindow), screenshotDir(screenshotDir) {}

		virtual ~ScreenshotHandler() {}

	protected:
		RenderWindowPtr renderWindow;
		string screenshotDir;

	private:
		virtual void buttonPressed(const ButtonCode button, const InputDevice device, const char buttonText) {
			if (button == BC_SYSRQ) {
				time_t now = time(0);
				struct tm tstruct;
				char buf[1024];
				tstruct = *localtime(&now);
				strftime(buf, sizeof(buf), (this->screenshotDir + "/Screenshot-%Y-%m-%d-%Hh%Mm%Ss.png").c_str(), &tstruct);
				std::string filename(buf);
				this->renderWindow->writeContentsToFile(filename);
			}
		}
	};
	typedef SharedPointer<ScreenshotHandler> ScreenshotHandlerPtr;
}

#endif