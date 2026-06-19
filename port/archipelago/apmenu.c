#include <PR/ultratypes.h>
#include "bss.h"
#include "data.h"
#include "game/menu.h"
#include "game/player.h"
#include "lib/rng.h"
#include "archipelago.h"
#include "apconfig.h"

extern u32 unlockedMissions[NUM_SOLOSTAGES][3];
extern u32 unlockedChallenges[30];
extern u32 unlockedWeapons[94];

extern u32 completedMissions[NUM_SOLOSTAGES][3];
extern u32 completedAgentObjectives[NUM_SOLOSTAGES][3];
extern u32 completedSpecialAgentObjectives[NUM_SOLOSTAGES][4];
extern u32 completedPerfectAgentObjectives[NUM_SOLOSTAGES][5];
extern u32 completedChallenges[30];

bool randomMusic;
bool randomCharacters;

u32 randomJoHead;
u32 randomVDHead;
u32 randomElvisHead;
u32 randomMaianHead;
u32 randomMrBlondeHead;

u32 randomCombatBody;
u32 randomTrenchBody;
u32 randomFrockRippedBody;
u32 randomFrockBody;
u32 randomLeatherBody;
u32 randomDeepSeaBody;
u32 randomWetSuitBody;
u32 randomSnowBody;
u32 randomLabBody;
u32 randomStewardessBody;
u32 randomNegotiatorBody;
u32 randomMrBlondeBody;
u32 randomMaianBody;

extern int completionGoal;
extern int requiredMissionStars;

extern int weaponProgressionType;
extern int allowProgWeaponInChallenges;

extern int hasChallenges;
extern int hasWeaponTraining;
extern int hasDeviceTraining;
extern int hasHolotraining;
extern int hasUnlockCheats;

extern int deathLink;

static MenuItemHandlerResult menuhandlerMissionChecklist(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (completedMissions[item->param][item->param3] == 0) {
			return false;
		}
		return true;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerChallengeChecklist(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (completedChallenges[item->param] == 0) {
			return false;
		}
		return true;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerAgentChecklist(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (completedAgentObjectives[item->param][item->param3] == 0) {
			return false;
		}
		return true;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerSpecialAgentChecklist(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (completedSpecialAgentObjectives[item->param][item->param3] == 0) {
			return false;
		}
		return true;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerPerfectAgentChecklist(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (completedPerfectAgentObjectives[item->param][item->param3] == 0) {
			return false;
		}
		return true;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerTest(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (unlockedMissions[item->param][item->param3] == 0) {
			return false;
		}
		return true;
	case MENUOP_SET:
		if (unlockedMissions[item->param][item->param3] == 0) {
			unlockedMissions[item->param][item->param3] = 1;
		}
		else {
			unlockedMissions[item->param][item->param3] = 0;
		}
		
		break;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerTestChallenges(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (unlockedChallenges[item->param] == 0) {
			return false;
		}
		return true;
	case MENUOP_SET:
		if (unlockedChallenges[item->param] == 0) {
			unlockedChallenges[item->param] = 1;
		}
		else {
			unlockedChallenges[item->param] = 0;
		}
		
		break;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerTestWeapons(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GET:
		if (unlockedWeapons[item->param] == 0) {
			return false;
		}
		return true;
	case MENUOP_SET:
		if (unlockedWeapons[item->param] == 0) {
			unlockedWeapons[item->param] = 1;
		}
		else {
			unlockedWeapons[item->param] = 0;
		}
		
		break;
	}

	return 0;
}

struct menuitem g_MissionChecklistMenuItems[] = {
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFECTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Defection: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFECTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Defection: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFECTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Defection: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INVESTIGATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Investigation: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INVESTIGATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Investigation: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INVESTIGATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Investigation: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_EXTRACTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Extraction: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_EXTRACTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Extraction: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_EXTRACTION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Extraction: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_VILLA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Villa: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_VILLA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Villa: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_VILLA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Villa: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CHICAGO,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Chicago: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CHICAGO,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Chicago: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CHICAGO,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Chicago: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_G5BUILDING,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"G5 Building: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_G5BUILDING,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"G5 Building: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_G5BUILDING,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"G5 Building: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INFILTRATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Infiltration: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INFILTRATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Infiltration: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_INFILTRATION,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Infiltration: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_RESCUE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Rescue: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_RESCUE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Rescue: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_RESCUE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Rescue: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ESCAPE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Escape: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ESCAPE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Escape: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ESCAPE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Escape: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRBASE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Base: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRBASE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Base: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRBASE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Base: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRFORCEONE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Force One: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRFORCEONE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Force One: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_AIRFORCEONE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Air Force One: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CRASHSITE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Crash Site: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CRASHSITE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Crash Site: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_CRASHSITE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Crash Site: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_PELAGIC,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Pelagic II: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_PELAGIC,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Pelagic II: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_PELAGIC,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Pelagic II: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEEPSEA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Deep Sea: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEEPSEA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Deep Sea: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEEPSEA,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Deep Sea: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFENSE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Institute: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFENSE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Institute: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DEFENSE,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Carrington Institute: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ATTACKSHIP,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Attack Ship: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ATTACKSHIP,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Attack Ship: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_ATTACKSHIP,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Attack Ship: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_SKEDARRUINS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Skedar Ruins: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_SKEDARRUINS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Skedar Ruins: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_SKEDARRUINS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Skedar Ruins: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MBR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Mr. Blonde's Revenge: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MBR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Mr. Blonde's Revenge: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MBR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Mr. Blonde's Revenge: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MAIANSOS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Maian SOS: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MAIANSOS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Maian SOS: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_MAIANSOS,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Maian SOS: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_WAR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"WAR!: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_WAR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"WAR!: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_WAR,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"WAR!: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DUEL,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"The Duel: Agent",
		0,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DUEL,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"The Duel: Special Agent",
		1,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		SOLOSTAGEINDEX_DUEL,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"The Duel: Perfect Agent",
		2,
		menuhandlerMissionChecklist,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_MissionChecklistMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Mission Checklist",
	g_MissionChecklistMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

struct menuitem g_ChallengeChecklistMenuItems[] = {
	{
		MENUITEMTYPE_CHECKBOX,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 1",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		1,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 2",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		2,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 3",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		3,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 4",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		4,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 5",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		5,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 6",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		6,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 7",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		7,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 8",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		8,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 9",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		9,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 10",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		10,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 11",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		11,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 12",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		12,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 13",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		13,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 14",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		14,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 15",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		15,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 16",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		16,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 17",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		17,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 18",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		18,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 19",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		19,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 20",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		20,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 21",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		21,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 22",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		22,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 23",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		23,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 24",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		24,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 25",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		25,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 26",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		26,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 27",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		27,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 28",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		28,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 29",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		29,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenge 30",
		0,
		menuhandlerChallengeChecklist,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_ChallengeChecklistMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Challenge Checklist",
	g_ChallengeChecklistMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

struct menuitem g_ConnectionInfoMenuItems[] = {
	{
		MENUITEMTYPE_LABEL,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Server Address:               \n",
		(uintptr_t)GetServerAddress,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		1,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Slot name: \n",
		(uintptr_t)GetSlotName,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		2,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Password: \n",
		(uintptr_t)GetPassword,
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		2,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Status: \n",
		(uintptr_t)GetStatus,
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_ConnectionInfoMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Connection Info",
	g_ConnectionInfoMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

const char *GetOption(struct menuitem *item)
{
	int option = 0;
	switch (item->param) {
		case 0:
			if (completionGoal == 0) {
				return "Complete Skedar Ruins  \n";
			}
			else {
				sprintf(g_StringPointer, "%d Mission Stars  \n", requiredMissionStars);
				return g_StringPointer;
			}
		case 1:
			if (weaponProgressionType == WEAPONPROG_DISABLED) {
				return "Disabled ";
			}
			else if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
				return "All Guns ";
			}
			else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
				return "One Gun ";
			}
		case 2:
			option = allowProgWeaponInChallenges;
			break;
		case 3:
			option = hasChallenges;
			break;
		case 4:
			option = hasWeaponTraining;
			break;
		case 5:
			option = hasDeviceTraining;
			break;
		case 6:
			option = hasHolotraining;
			break;
		case 7:
			option = hasUnlockCheats;
			break;
		case 8:
			option = deathLink;
			break;
	}

	if (option == 1) {
		return "Enabled ";
	}

	return "Disabled ";
}

struct menuitem g_GameOptionsMenuItems[] = {
	{
		MENUITEMTYPE_LABEL,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Completion Goal:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		1,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Weapon Progression:                 \n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		2,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Prog Wpns in Challenges:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		3,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Challenges:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		4,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Firing Range:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		5,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Device Training:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		6,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Holotraining:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		7,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Unlock Cheat Checks:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_LABEL,
		8,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Death Link:\n",
		(uintptr_t)&GetOption,
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_GameOptionsMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Gameplay Options",
	g_GameOptionsMenuItems,
	NULL,
	MENUDIALOGFLAG_0002 | MENUDIALOGFLAG_DISABLERESIZE | MENUDIALOGFLAG_0400 | MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

MenuItemHandlerResult menuhandlerRandomMusic(s32 operation, struct menuitem *item, union handlerdata *data)
{
	s32 val;

	switch (operation) {
	case MENUOP_GET:
		return randomMusic;
	case MENUOP_SET:
		randomMusic = data->checkbox.value;
	}

	return 0;
}

MenuItemHandlerResult menuhandlerRandomCharacters(s32 operation, struct menuitem *item, union handlerdata *data)
{
	s32 val;

	switch (operation) {
	case MENUOP_GET:
		return randomCharacters;
	case MENUOP_SET:
		randomCharacters = data->checkbox.value;
	}

	return 0;
}

u32 getRandomBody() {
	// Get a random body
	u32 randomBody = (rngRandom() % 68);

	// This is if the suit outfits get picked
	if (randomBody <= 3) {
		return randomBody + 0x00;
	}

	// s32 randomBody = (random() % 64) + 0x56;
	randomBody = (randomBody - 4) + 0x56;
	while (randomBody == BODY_SKEDAR 
		|| randomBody == BODY_DRCAROLL 
		|| randomBody == BODY_EYESPY
		|| randomBody == BODY_TESTCHR 
		|| randomBody == BODY_CHICROB 
		|| randomBody == BODY_MINISKEDAR
		|| randomBody == BODY_SKEDARKING) {
		randomBody = (rngRandom() % 64) + 0x56;
	}

	return randomBody;
}

u32 getRandomBodyExcludeShort() {
	// Get a random body
	u32 randomBody = (rngRandom() % 68);

	// This is if the suit outfits get picked
	if (randomBody <= 3) {
		return randomBody + 0x00;
	}

	// s32 randomBody = (random() % 64) + 0x56;
	randomBody = (randomBody - 4) + 0x56;
	while (randomBody == BODY_ELVIS1
		|| randomBody == BODY_SKEDAR
		|| randomBody == BODY_THEKING 
		|| randomBody == BODY_DRCAROLL 
		|| randomBody == BODY_EYESPY
		|| randomBody == BODY_TESTCHR 
		|| randomBody == BODY_CHICROB 
		|| randomBody == BODY_MINISKEDAR
		|| randomBody == BODY_SKEDARKING
		|| randomBody == BODY_ELVISWAISTCOAT) {
		randomBody = (rngRandom() % 64) + 0x56;
	}

	return randomBody;
}

u32 getRandomHead() {

	// Get a number between 0x04 and 0x55
	u32 randomHead = (rngRandom() % 81) + 0x04;

	return randomHead;
}

MenuItemHandlerResult menuhandlerRandomize(s32 operation, struct menuitem *item, union handlerdata *data)
{
	s32 bodynum;
	s32 headnum;

	if (operation == MENUOP_SET) {
		// Randomize the heads of playable characters
		randomJoHead = getRandomHead();
		randomVDHead = getRandomHead();
		randomElvisHead = getRandomHead();
		randomMaianHead = getRandomHead();
		randomMrBlondeHead = getRandomHead();

		// Randomize the bodies of playable characters
		randomCombatBody = getRandomBody();
		randomTrenchBody = getRandomBodyExcludeShort();
		randomFrockRippedBody = getRandomBody();
		randomFrockBody = getRandomBodyExcludeShort();
		randomLeatherBody = getRandomBody();
		randomDeepSeaBody = getRandomBody();
		randomWetSuitBody = getRandomBody();
		randomSnowBody = getRandomBody();
		randomLabBody = getRandomBody();
		randomStewardessBody = getRandomBody();
		randomNegotiatorBody = getRandomBody();
		randomMrBlondeBody = getRandomBody();
		randomMaianBody = getRandomBody();

		func0f0f820c(NULL, -7);
	}

	return 0;
}

struct menuitem g_RandomizeCharactersMenuItems[] = {
	{
		MENUITEMTYPE_LABEL,
		0,
		MENUITEMFLAG_LITERAL_TEXT | MENUITEMFLAG_00000002 | MENUITEMFLAG_LESSLEFTPADDING,
		(uintptr_t)"Do you want to randomize your characters?\n", // "Do you want to load another agent?"
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		0,
		L_OPTIONS_190, // "Yes"
		0,
		menuhandlerRandomize,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_191, // "No"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_RandomizeCharactersMenuDialog = {
	MENUDIALOGTYPE_DANGER,
	L_OPTIONS_188, // "Warning"
	g_RandomizeCharactersMenuItems,
	NULL,
	0,
	NULL,
};

MenuItemHandlerResult randomizeCharactersMenuDialog(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_SET:
		menuPopDialog();
		menuPushDialog(&g_RandomizeCharactersMenuDialog);
		break;
	case MENUOP_CHECKDISABLED:
		if (randomCharacters == false) {
			return true;
		}
	}

	return 0;
}

struct menuitem g_ArchipelagoMenuItems[] = {
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Connection Info\n",
		0,
		(void *)&g_ConnectionInfoMenuDialog,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Gameplay Options\n",
		0,
		(void *)&g_GameOptionsMenuDialog,
	},
	// {
	// 	MENUITEMTYPE_SELECTABLE,
	// 	0,
	// 	MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
	// 	(uintptr_t)"Mission Checklist\n",
	// 	0,
	// 	(void *)&g_MissionChecklistMenuDialog,
	// },
	// {
	// 	MENUITEMTYPE_SELECTABLE,
	// 	0,
	// 	MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
	// 	(uintptr_t)"Challenge Checklist\n",
	// 	0,
	// 	(void *)&g_ChallengeChecklistMenuDialog,
	// },
	{
		MENUITEMTYPE_CHECKBOX,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Random Music\n",
		0,
		menuhandlerRandomMusic,
	},
	{
		MENUITEMTYPE_CHECKBOX,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Random Characters\n",
		0,
		menuhandlerRandomCharacters,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Randomize\n",
		0,
		randomizeCharactersMenuDialog,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_ArchipelagoMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Archipelago",
	g_ArchipelagoMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

PD_CONSTRUCTOR static void APConfigRandomOptionsInit(void)
{
    APConfigRegisterUInt("RandomOptions.RandomMusic", &randomMusic, 0, 1);
	APConfigRegisterUInt("RandomOptions.RandomCharacters", &randomCharacters, 0, 1);
	APConfigRegisterUInt("RandomOptions.JoannaHead", &randomJoHead, 4, 85);
	APConfigRegisterUInt("RandomOptions.VelvetHead", &randomVDHead, 4, 85);
	APConfigRegisterUInt("RandomOptions.ElvisHead", &randomElvisHead, 4, 85);
	APConfigRegisterUInt("RandomOptions.MaianHead", &randomMaianHead, 4, 85);
	APConfigRegisterUInt("RandomOptions.MrBlondeHead", &randomMrBlondeHead, 4, 85);
	APConfigRegisterUInt("RandomOptions.CombatCostume", &randomCombatBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.TrenchCostume", &randomTrenchBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.RippedFrockCostume", &randomFrockRippedBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.FrockCostume", &randomFrockBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.LeatherCostume", &randomLeatherBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.DeepSeaCostume", &randomDeepSeaBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.WetSuitCostume", &randomWetSuitBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.SnowCostume", &randomSnowBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.LabCostume", &randomLabBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.StewardessCostume", &randomStewardessBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.NegotiatorCostume", &randomNegotiatorBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.MrBlondeCostume", &randomMrBlondeBody, 0, 150);
	APConfigRegisterUInt("RandomOptions.MaianCostume", &randomMaianBody, 0, 150);
}