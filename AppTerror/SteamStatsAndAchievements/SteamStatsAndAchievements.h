#ifndef TERROR_STEAM_STATS_AND_ACHIEVEMENTS_H
#define TERROR_STEAM_STATS_AND_ACHIEVEMENTS_H

#include "Config.h"

namespace terror {
	void initializeSteamStatsAndAchievements() {
		SteamSystemPtr steamSystem = SteamSystem::getInstance();
		steamSystem->addStat("games_won");
		steamSystem->addStat("doors_unlocked");
		steamSystem->addStat("survivors_killed");
		steamSystem->addStat("traps_activated");
		steamSystem->addStat("barricades_broken");
		steamSystem->addStat("batteries_picked");
		steamSystem->addStat("stages_won");
		steamSystem->addStat("characters_killed");

		steamSystem->addAchievement("pick_a_silver_key");
		steamSystem->addAchievement("pick_a_hammer_or_crowbar");
		steamSystem->addAchievement("Black_Lake_Asylum");
		steamSystem->addAchievement("Bodom_Hotel");
		steamSystem->addAchievement("Bright_Hotel");
		steamSystem->addAchievement("Factory");
		steamSystem->addAchievement("Hund_Hills_Asylum");
		steamSystem->addAchievement("Pines_Grill_Restaurant");
		steamSystem->addAchievement("Rose_Crimson_Hotel");
		steamSystem->addAchievement("Serenity_Asylum");
		steamSystem->addAchievement("Warehouse");
		steamSystem->addAchievement("win_all_stages");
		steamSystem->addAchievement("win_10_games");
		steamSystem->addAchievement("win_50_games");
		steamSystem->addAchievement("life_zero");
		steamSystem->addAchievement("last_man_standing");
		steamSystem->addAchievement("no_man_stands_alone");
		steamSystem->addAchievement("unlock_25_doors");
		steamSystem->addAchievement("unlock_100_doors");
		steamSystem->addAchievement("break_6_barricades");
		steamSystem->addAchievement("hold_more_than_10_items");
		steamSystem->addAchievement("pick_30_batteries");
		steamSystem->addAchievement("Lurker");
		steamSystem->addAchievement("Phantom");
		steamSystem->addAchievement("Mary");
		steamSystem->addAchievement("no_fun_allowed");
		steamSystem->addAchievement("i_see_you");
		steamSystem->addAchievement("berserker");
		steamSystem->addAchievement("activate_25_traps");
		steamSystem->addAchievement("activate_100_traps");
		steamSystem->addAchievement("John");
		steamSystem->addAchievement("Scott");
		steamSystem->addAchievement("Mia");
		steamSystem->addAchievement("Jacob");
		steamSystem->addAchievement("kill_all_characters");
		steamSystem->addAchievement("kill_15_survivors");
		steamSystem->addAchievement("kill_30_survivors");

		steamSystem->requestStats();
	}
}

#endif