#include "itemhandler.h"
#include <ultra64.h>
#include <stdio.h>
#include <stdint.h>
#include "constants.h"
#include "game/bondgun.h"
#include "game/chraction.h"
#include "game/hudmsg.h"
#include "game/inv.h"
#include "game/menu.h"
#include "game/playermgr.h"
#include "archipelago.h"

extern struct g_vars g_Vars;

#define AP_ITEM_AGENT_START 95
#define AP_ITEM_SPECIAL_AGENT_START 116
#define AP_ITEM_PERFECT_AGENT_START 137
#define AP_ITEM_CHALLENGE_START 158
#define AP_ITEM_PROGRESSIVE_WEAPON 188
#define AP_ITEM_CHEAT_START 189
#define AP_ITEM_CLASSIC_WEAPON_CHEAT_START 223
#define AP_ITEM_FILLER 231
#define AP_ITEM_MISSION_STAR 233
#define AP_ITEM_VICTORY 234

#define AP_AGENT_OBJ_OFFSET 1
#define AP_SPECIAL_AGENT_OBJ_OFFSET 62
#define AP_PERFECT_AGENT_OBJ_OFFSET 144
#define AP_MISSION_OFFSET 247
#define AP_CHALLENGE_OFFSET 310
#define AP_FIRING_RANGE_OFFSET 340
#define AP_DEVICE_TRAINING_OFFSET 439
#define AP_HOLOTRAINING_OFFSET 449
#define AP_COMPLETE_CHEAT_OFFSET 456
#define AP_TIMED_CHEAT_OFFSET 473
#define AP_CLASSIC_WEAPON_CHEAT_OFFSET 490

u32 completedMissions[NUM_SOLOSTAGES][3];
u32 completedAgentObjectives[NUM_SOLOSTAGES][3];
u32 completedSpecialAgentObjectives[NUM_SOLOSTAGES][4];
u32 completedPerfectAgentObjectives[NUM_SOLOSTAGES][5];
u32 completedChallenges[30];
u32 completedTrainingMedals[32][3];

u32 unlockedMissions[NUM_SOLOSTAGES][3];
u32 unlockedChallenges[30];
u32 unlockedWeapons[94];
u32 unlockedCheats[42];

int missionStars = 0;
extern int requiredMissionStars;

int progressiveWeapon = 0;
extern int weaponProgressionType;
extern int allowProgWeaponInChallenges;

extern int hasChallenges;
extern int hasWeaponTraining;
extern int hasDeviceTraining;
extern int hasHolotraining;
extern int hasUnlockCheats;

int progressiveWeaponNumbers[43] = {
    WEAPON_UNARMED,
    WEAPON_COMBATKNIFE,
    WEAPON_PSYCHOSISGUN,
    WEAPON_TRANQUILIZER,
    WEAPON_KL01313,
    WEAPON_LASER,
    WEAPON_CROSSBOW,
    WEAPON_SNIPERRIFLE,
    WEAPON_FALCON2,
    WEAPON_FALCON2_SILENCER,
    WEAPON_FALCON2_SCOPE,
    WEAPON_PP9I,
    WEAPON_CC13,
    WEAPON_MAGSEC4,
    WEAPON_DY357MAGNUM,
    WEAPON_SHOTGUN,
    WEAPON_KF7SPECIAL,
    WEAPON_DMC,
    WEAPON_ZZT,
    WEAPON_CMP150,
    WEAPON_DRAGON,
    WEAPON_REAPER,
    WEAPON_AR34,
    WEAPON_CYCLONE,
    WEAPON_LAPTOPGUN,
    WEAPON_TIMEDMINE,
    WEAPON_PROXIMITYMINE,
    WEAPON_GRENADE,
    WEAPON_SLAYER,
    WEAPON_REMOTEMINE,
    WEAPON_NBOMB,
    WEAPON_K7AVENGER,
    WEAPON_CALLISTO,
    WEAPON_AR53,
    WEAPON_ROCKETLAUNCHER,
    WEAPON_DEVASTATOR,
    WEAPON_SUPERDRAGON,
    WEAPON_MAULER,
    WEAPON_PHOENIX,
    WEAPON_RCP45,
    WEAPON_RCP120,
    WEAPON_DY357LX,
    WEAPON_FARSIGHT,
};

// Orders weapons based on their progression, with better on top
int progWeaponInvPosition[] = {
	0,      // WEAPON_NONE
	1,      // WEAPON_UNARMED
	36,     // WEAPON_FALCON2
	35,     // WEAPON_FALCON2_SILENCER
	34,     // WEAPON_FALCON2_SCOPE
	31,     // WEAPON_MAGSEC4
	7,      // WEAPON_MAULER
	6,      // WEAPON_PHOENIX
	30,     // WEAPON_DY357MAGNUM
	3,      // WEAPON_DY357LX
	25,     // WEAPON_CMP150
	21,     // WEAPON_CYCLONE
	12,     // WEAPON_CALLISTO
	4,      // WEAPON_RCP120
	20,     // WEAPON_LAPTOPGUN
	24,     // WEAPON_DRAGON
	13,     // WEAPON_K7AVENGER
	22,     // WEAPON_AR34
	8,     // WEAPON_SUPERDRAGON
	29,     // WEAPON_SHOTGUN
	23,     // WEAPON_REAPER
	37,     // WEAPON_SNIPERRIFLE
	2,      // WEAPON_FARSIGHT
	9,      // WEAPON_DEVASTATOR
	10,      // WEAPON_ROCKETLAUNCHER
	16,     // WEAPON_SLAYER
	43,     // WEAPON_COMBATKNIFE
	38,      // WEAPON_CROSSBOW
	41,     // WEAPON_TRANQUILIZER
	39,     // WEAPON_LASER
	17,     // WEAPON_GRENADE
	14,     // WEAPON_NBOMB
	19,     // WEAPON_TIMEDMINE
	18,     // WEAPON_PROXIMITYMINE
	15,     // WEAPON_REMOTEMINE
	44,     // WEAPON_COMBATBOOST
	33,     // WEAPON_PP9I
	32,     // WEAPON_CC13
	40,     // WEAPON_KL01313
	28,     // WEAPON_KF7SPECIAL
	26,     // WEAPON_ZZT
	27,     // WEAPON_DMC
	11,     // WEAPON_AR53
	5,      // WEAPON_RCP45
	42,     // WEAPON_PSYCHOSISGUN
    100,    // WEAPON_NIGHTVISION
    101,    // WEAPON_EYESPY
    102,    // WEAPON_XRAYSCANNER
    103,    // WEAPON_IRSCANNER
    104,    // WEAPON_CLOAKINGDEVICE
    105,    // WEAPON_HORIZONSCANNER
    106,    // WEAPON_TESTER
	107,    // WEAPON_ROCKETLAUNCHER_34
	108,    // WEAPON_ECMMINE
	109,    // WEAPON_DATAUPLINK
	110,    // WEAPON_RTRACKER
	111,    // WEAPON_PRESIDENTSCANNER
	112,    // WEAPON_DOORDECODER
	113,    // WEAPON_AUTOSURGEON
	114,    // WEAPON_EXPLOSIVES
	115,    // WEAPON_SKEDARBOMB
    116,    // WEAPON_COMMSRIDER
    117,    // WEAPON_TRACERBUG
    118,    // WEAPON_TARGETAMPLIFIER
    119,    // WEAPON_DISGUISE40
    120,    // WEAPON_DISGUISE41
    121,    // WEAPON_FLIGHTPLANS
    122,    // WEAPON_RESEARCHTAPE
    123,    // WEAPON_BACKUPDISK
    124,    // WEAPON_KEYCARD45
    125,    // WEAPON_KEYCARD46
    126,    // WEAPON_KEYCARD47
    127,    // WEAPON_KEYCARD48
    128,    // WEAPON_KEYCARD49
    129,    // WEAPON_KEYCARD4A
    130,    // WEAPON_KEYCARD4B
    131,    // WEAPON_KEYCARD4C
    132,    // WEAPON_SUITCASE
    133,    // WEAPON_BRIEFCASE
    134,    // WEAPON_SHIELDTECHITEM
    135,    // WEAPON_NECKLACE
    136,    // WEAPON_HAMMER
    137,    // WEAPON_SCREWDRIVER
    138,    // WEAPON_ROCKET
    139,    // WEAPON_HOMINGROCKET
    140,    // WEAPON_GRENADEROUND
    141,    // WEAPON_BOLT
    142,    // WEAPON_BRIEFCASE2
    143,    // WEAPON_SKROCKET
    144,    // WEAPON_CHOPPERGUN
    145,    // WEAPON_WATCHLASER
    146,    // WEAPON_MPSHIELD
    147,    // WEAPON_DISABLED
    148,    // WEAPON_SUICIDEPILL
};

void resetAP()
{
	s32 i;
	s32 j;

	for (i = 0; i < NUM_SOLOSTAGES; i++) {
		for (j = 0; j < 3; j++) {
			unlockedMissions[i][j] = 0;
			completedMissions[i][j] = 0;
			completedAgentObjectives[i][j] = 0;
		}

		for (j = 0; j < 4; j++) {
			completedSpecialAgentObjectives[i][j] = 0;
		}

		for (j = 0; j < 5; j++) {
			completedPerfectAgentObjectives[i][j] = 0;
		}
	}

	for (i = 0; i < ARRAYCOUNT(unlockedChallenges); i++) {
		unlockedChallenges[i] = 0;
		completedChallenges[i] = 0;
	}

	for (i = 0; i < ARRAYCOUNT(unlockedWeapons); i++) {
		unlockedWeapons[i] = 0;
	}

    for (i = 0; i < ARRAYCOUNT(unlockedCheats); i++) {
		unlockedCheats[i] = 0;
	}

    func0f0f820c(NULL, -7);
}

void collectMissionItem(u8 missionIndex, u8 difficulty)
{
    uint64_t location = 3 * missionIndex + difficulty + AP_MISSION_OFFSET;
    InternalCollectAPItem(location);
}

void collectObjectiveItem(u8 missionIndex, u8 difficulty, u8 objIndex)
{
    int APOffset;
    int difficultyOffset;
    switch (difficulty) {
        case 0:
            APOffset = AP_AGENT_OBJ_OFFSET;
            difficultyOffset = 3;
            break;
        case 1:
            APOffset = AP_SPECIAL_AGENT_OBJ_OFFSET;
            difficultyOffset = 4;
            break;
        case 2:
            APOffset = AP_PERFECT_AGENT_OBJ_OFFSET;
            difficultyOffset = 5;
            break;
    }

    uint64_t location = difficultyOffset * missionIndex + objIndex + APOffset;
    InternalCollectAPItem(location);
    
}

void collectChallengeItem(u32 challengeIndex)
{
    uint64_t location = challengeIndex + AP_CHALLENGE_OFFSET;
	if (hasChallenges == 1) {
		InternalCollectAPItem(location);
	}
}

void collectFiringRangeItem(s32 weaponIndex, s32 difficulty)
{
    uint64_t location = 3 * (weaponIndex - 2) + difficulty + AP_FIRING_RANGE_OFFSET;
	if (hasWeaponTraining == 1) {
		InternalCollectAPItem(location);
	}
}

void collectDeviceTrainingItem(s32 deviceIndex)
{
    uint64_t location = deviceIndex + AP_DEVICE_TRAINING_OFFSET;
	if (hasDeviceTraining == 1) {
		InternalCollectAPItem(location);
	}
}

void collectHolotrainingItem(s32 holoIndex)
{
    uint64_t location = holoIndex + AP_HOLOTRAINING_OFFSET;
	if (hasHolotraining == 1) {
		InternalCollectAPItem(location);
	}
}

void collectCompleteCheatItem(u8 missionIndex)
{
    uint64_t location = missionIndex + AP_COMPLETE_CHEAT_OFFSET;
	if (hasUnlockCheats == 1) {
		InternalCollectAPItem(location);
	}
}

void collectTimedCheatItem(u8 missionIndex)
{
    uint64_t location = missionIndex + AP_TIMED_CHEAT_OFFSET;
	if (hasUnlockCheats == 1) {
		InternalCollectAPItem(location);
	}
}

void collectClassicWeaponCheatItem(u8 weaponIndex)
{
    uint64_t location = (weaponIndex - WEAPON_PP9I) + AP_CLASSIC_WEAPON_CHEAT_OFFSET;
	if (hasUnlockCheats == 1) {
		InternalCollectAPItem(location);
	}
}

char* CheckString(const char* string)
{
    char *buffer;
    buffer = malloc(strlen(string) + 1);
    strcpy(buffer, string);

    for (s32 i = 0; i < strlen(string); i++) {
        if (string[i] > 127) {
            buffer[i] = 63;
        }
    }

    return buffer;
}

void printSentItemMessage(const char* itemname, const char* recipient, const char* location)
{
    char buffer[300] = "";

    itemname = CheckString(itemname);
    recipient = CheckString(recipient);
    location = CheckString(location);

    sprintf(buffer, "Sent %s to %s (%s)\n", itemname, recipient, location);
	hudmsgCreate(buffer, HUDMSGTYPE_DEFAULT);
}

void handleItem(int itemID, const char* itemname, const char* sender, const char* location)
{
    char buffer[300] = "";

    itemname = CheckString(itemname);
    sender = CheckString(sender);
    location = CheckString(location);

    for (s32 i = 0; i < PLAYERCOUNT(); i++) {
		setCurrentPlayerNum(i);
        sprintf(buffer, "Got %s from %s (%s)\n", itemname, sender, location);
        hudmsgCreate(buffer, HUDMSGTYPE_DEFAULT);
    }

    // Progressive Weapon
    if (weaponProgressionType != WEAPONPROG_DISABLED && itemID == AP_ITEM_PROGRESSIVE_WEAPON) {
        progressiveWeapon += 1;

        unlockedWeapons[progressiveWeaponNumbers[progressiveWeapon]] = 1;

        for (s32 i = 0; i < PLAYERCOUNT(); i++) {
			setCurrentPlayerNum(i);
            if (g_Vars.stagenum != STAGE_CITRAINING && !g_Vars.normmplayerisrunning) {
                if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
                    invGiveSingleWeapon(progressiveWeaponNumbers[progressiveWeapon]);
                    
                    if (g_Vars.currentplayer->gunctrl.weaponnum != WEAPON_UNARMED) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
                else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
                    if (progressiveWeapon > 1) {
                        invRemoveItemByNum(progressiveWeaponNumbers[progressiveWeapon - 1]);
                    }

                    invGetProgressiveWeapons();

                    if (g_Vars.currentplayer->gunctrl.weaponnum == progressiveWeaponNumbers[progressiveWeapon - 1]) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
            }
            else if (g_Vars.stagenum != STAGE_CITRAINING
                        && g_Vars.normmplayerisrunning 
                        && allowProgWeaponInChallenges == 1) {
                if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
                    for (s32 i = 0; i < PLAYERCOUNT(); i++) {
                        setCurrentPlayerNum(i);
                        invGiveSingleWeapon(progressiveWeaponNumbers[progressiveWeapon]);
                        
                        if (g_Vars.currentplayer->gunctrl.weaponnum != WEAPON_UNARMED) {
                            bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                        }
                    }
                }
                else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
                    for (s32 i = 0; i < PLAYERCOUNT(); i++) {
                        setCurrentPlayerNum(i);
                        if (progressiveWeapon > 1) {
                            invRemoveItemByNum(progressiveWeaponNumbers[progressiveWeapon - 1]);
                        }

                        invGetProgressiveWeapons();

                        if (g_Vars.currentplayer->gunctrl.weaponnum == progressiveWeaponNumbers[progressiveWeapon - 1]) {
                            bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                        }
                    }
                }
            }
		}

        return;
    }

    // Filler
	if (itemID == AP_ITEM_FILLER) {
        return;
	}

	// Mission Star
	if (itemID == AP_ITEM_MISSION_STAR) {
        missionStars += 1;

        if (missionStars >= requiredMissionStars) {
            ReachedGoal();
        }

        return;
	}

    // Victory
    if (itemID == AP_ITEM_VICTORY) {
        ReachedGoal();
        return;
    }

    if (itemID < AP_ITEM_AGENT_START) {
        unlockedWeapons[itemID - 1] = 1;

        if ((itemID - 1) == WEAPON_CROSSBOW) {
            unlockedWeapons[WEAPON_BOLT] = 1;
        }
    }
    else if (itemID < AP_ITEM_SPECIAL_AGENT_START && itemID >= AP_ITEM_AGENT_START) {
        // Agent
        unlockedMissions[itemID - AP_ITEM_AGENT_START][0]= 1;
    }
    else if (itemID < AP_ITEM_PERFECT_AGENT_START && itemID >= AP_ITEM_SPECIAL_AGENT_START) {
        // Special Agent
        unlockedMissions[itemID - AP_ITEM_SPECIAL_AGENT_START][1]= 1;
    }
    else if (itemID < AP_ITEM_CHALLENGE_START && itemID >= AP_ITEM_PERFECT_AGENT_START) {
        // Perfect Agent
        unlockedMissions[itemID - AP_ITEM_PERFECT_AGENT_START][2]= 1;
    }
    else if (itemID < AP_ITEM_CHEAT_START  && itemID >= AP_ITEM_CHALLENGE_START) {
        unlockedChallenges[itemID - AP_ITEM_CHALLENGE_START] = 1;
    }
    else if (itemID < AP_ITEM_FILLER && itemID >= AP_ITEM_CHEAT_START) {
        unlockedCheats[itemID - AP_ITEM_CHEAT_START] = 1;

        // Classic weapon cheats
        if (itemID >= AP_ITEM_CLASSIC_WEAPON_CHEAT_START) {
            unlockedWeapons[itemID - 186] = 1;
        }
    }
}
