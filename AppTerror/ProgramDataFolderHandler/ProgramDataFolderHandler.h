#ifndef TERROR_PROGRAM_DATA_FOLDER_HANDLER_H
#define TERROR_PROGRAM_DATA_FOLDER_HANDLER_H

#include "Config.h"

namespace terror {
	class ProgramDataFolderHandler {
	public:
		ProgramDataFolderHandler() {
			string programDataPath = getenv("PROGRAMDATA");
			path baseDir(programDataPath + "/Damned");
			this->configDir = path(programDataPath + "/Damned/Setup");
			this->saveDir = path(programDataPath + "/Damned/Save");
			this->screenshotsDir = path(programDataPath + "/Damned/Screenshots");
			this->createSetupDirsAndFiles(baseDir);
		}

		string getConfigDir() {
			return this->configDir.string();
		}

		string getSaveDir() {
			return this->saveDir.string();
		}

		string getScreenshotsDir() {
			return this->screenshotsDir.string();
		}

		virtual ~ProgramDataFolderHandler() {}

	protected:
		path configDir;
		path saveDir;
		path screenshotsDir;

	private:
		void createSetupDirsAndFiles(const path& baseDir) {
			if (!is_directory(baseDir)) {
				create_directory(baseDir);
			}
			if (!is_directory(this->configDir)) {
				create_directory(this->configDir);
			}
			if (!is_directory(this->saveDir)) {
				create_directory(this->saveDir);
			}
			if (!is_directory(this->screenshotsDir)) {
				create_directory(this->screenshotsDir);
			}

			path setupFile = path(this->configDir.string() + "/plugins.cfg");
			if (!exists(setupFile)) {
				ofstream file(setupFile.c_str());
				file << "PluginFolder=." << endl;
				file << "Plugin=RenderSystem_Direct3D9" << endl;
				file << "Plugin=Plugin_ParticleFX" << endl;
				file << "Plugin=Plugin_CgProgramManager" << endl;
				file << "Plugin=Plugin_OctreeSceneManager" << endl;
			}
		}
	};
	typedef SharedPointer<ProgramDataFolderHandler> ProgramDataFolderHandlerPtr;
}

#endif