#ifndef TERROR_STAGE_DATABASE_H
#define TERROR_STAGE_DATABASE_H

#include "Config.h"

namespace terror {
	typedef unordered_map<int, std::string> StringMap;

	class StageDatabase {
	public:
		typedef SharedPointer<StageDatabase> StageDatabasePtr;

		static StageDatabasePtr getInstance() {
			if (!StageDatabase::instance) {
				StageDatabase::instance = StageDatabasePtr(new StageDatabase);
			}
			return StageDatabase::instance;
		}

		std::string getStageName(int id) {
			StringMap::const_iterator entry = this->stages.find(id);
			if (entry == this->stages.end()) {
				return "Invalid Stage";
			}
			return entry->second;
		}

		int getStageId(const std::string& stageName) {
			foreach (StringMap::value_type& entry, this->stages) {
				if (entry.second == stageName) {
					return entry.first;
				}
			}
			return -1;
		}

		StringListPtr getStagesNames() {
			StringListPtr stagesNames(new StringList);
			foreach (StringMap::value_type& entry, this->stages) {
				stagesNames->push_back(entry.second);
			}
			stagesNames->sort();
			return stagesNames;
		}

		virtual ~StageDatabase() {}

	protected:
		StringMap stages;

	private:
		static StageDatabasePtr instance;

		StageDatabase() {
			ExtensionPeekerPtr extensionPeeker(new ExtensionPeeker("./DamnedData/Resources/Stages", ".stage"));
			StringListPtr stageNames = extensionPeeker->peek();
			stageNames->sort();
			int id = 0;
			foreach (string stageName, *stageNames) {
				if (stageName != "menu_background.stage") {
					erase_last(stageName, ".stage");
					replace_all(stageName, "_", " ");
					this->stages[id] = stageName;
					//Makes adding new maps easier when you know what ID a stage has
					//TODO: Reprogramm into a direct method? Then i won't have to rearrange all ids in the .layout and the .xml file...
					std::cout << "\nStage: " << stageName << "\nID: " << id;
					++id;
				}
			}
		}
	};

	typedef StageDatabase::StageDatabasePtr StageDatabasePtr;

	StageDatabasePtr StageDatabase::instance = StageDatabasePtr();
}

#endif