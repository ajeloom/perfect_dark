#include "itemhandler.h"
#include <ultra64.h>
#include <stdio.h>
#include <stdint.h>
#include "bss.h"
#include "game/bondgun.h"
#include "game/hudmsg.h"
#include "game/inv.h"
#include "game/menu.h"
#include "game/pdmode.h"
#include "game/playermgr.h"
#include "game/title.h"
#include "lib/main.h"
#include "archipelago.h"

#define AP_ITEM_AGENT_START 95
#define AP_ITEM_SPECIAL_AGENT_START 116
#define AP_ITEM_PERFECT_AGENT_START 137
#define AP_ITEM_CHALLENGE_START 158
#define AP_ITEM_PROGRESSIVE_WEAPON 188
#define AP_ITEM_CHEAT_START 189
#define AP_ITEM_CLASSIC_WEAPON_CHEAT_START 223
#define AP_ITEM_DATADYNE_MASTER_KEY 231
#define AP_ITEM_G5_BUILDING_MASTER_KEY 232
#define AP_ITEM_AREA_51_MASTER_KEY 233
#define AP_ITEM_AIR_FORCE_ONE_MASTER_KEY 234
#define AP_ITEM_FILLER 235
#define AP_ITEM_MISSION_STAR 237
#define AP_ITEM_CHALLENGE_STAR 238
#define AP_ITEM_SKEDAR_RUINS 239
#define AP_ITEM_VICTORY 240
#define AP_ITEM_CHARACTER_START 246

#define AP_ITEM_PROG_PISTOL 241
#define AP_ITEM_PROG_SMG 242
#define AP_ITEM_PROG_RIFLE 243
#define AP_ITEM_PROG_EXPLOSIVE 244
#define AP_ITEM_PROG_OTHER_WEAPON 245

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
#define AP_COLLECT_ALL_STARS_LOCATION 498
#define AP_MPFEATURE_OFFSET 499

extern int lastReceivedItemIndex;

u32 completedMissions[NUM_SOLOSTAGES][3];
u32 completedAgentObjectives[NUM_SOLOSTAGES][3];
u32 completedSpecialAgentObjectives[NUM_SOLOSTAGES][4];
u32 completedPerfectAgentObjectives[NUM_SOLOSTAGES][5];
u32 completedChallenges[30];
u32 completedTrainingMedals[33][3];
u32 completedLocations[578];

u32 unlockedMissions[NUM_SOLOSTAGES][3];
u32 unlockedChallenges[30];
u32 unlockedWeapons[94];
u32 unlockedCheats[42];
u32 unlockedCharacters[6];

extern int completionGoal;
extern int skedarRequirements;

int missionStars = 0;
extern int requiredMissionStars;

int challengeStars = 0;
extern int requiredChallengeStars;

int progressiveWeapon = 0;
extern int weaponProgressionType;
extern int allowProgWeaponInChallenges;

int progressivePistol = 0;
int progressiveSMG = 0;
int progressiveRifle = 0;
int progressiveExplosive = 0;
int progressiveOtherWeapon = 0;

extern int hasChallenges;
extern int hasWeaponTraining;
extern int hasDeviceTraining;
extern int hasHolotraining;
extern int hasCompletionCheats;
extern int hasTimedCheats;
extern int hasWeaponCheats;

extern int hasNPCs;
extern int hasMPUnlocks;

extern bool showLocationName;

int progressiveWeaponNumbers[43] = {
    WEAPON_UNARMED,
    WEAPON_COMBATKNIFE,
    WEAPON_PSYCHOSISGUN,
    WEAPON_TRANQUILIZER,
    WEAPON_KL01313,
    WEAPON_CC13,
    WEAPON_LASER,
    WEAPON_CROSSBOW,
    WEAPON_SNIPERRIFLE,
    WEAPON_FALCON2,
    WEAPON_FALCON2_SILENCER,
    WEAPON_FALCON2_SCOPE,
    WEAPON_PP9I,
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
	35,     // WEAPON_FALCON2
	34,     // WEAPON_FALCON2_SILENCER
	33,     // WEAPON_FALCON2_SCOPE
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
	36,     // WEAPON_SNIPERRIFLE
	2,      // WEAPON_FARSIGHT
	9,      // WEAPON_DEVASTATOR
	10,      // WEAPON_ROCKETLAUNCHER
	16,     // WEAPON_SLAYER
	43,     // WEAPON_COMBATKNIFE
	37,      // WEAPON_CROSSBOW
	41,     // WEAPON_TRANQUILIZER
	38,     // WEAPON_LASER
	17,     // WEAPON_GRENADE
	14,     // WEAPON_NBOMB
	19,     // WEAPON_TIMEDMINE
	18,     // WEAPON_PROXIMITYMINE
	15,     // WEAPON_REMOTEMINE
	44,     // WEAPON_COMBATBOOST
	32,     // WEAPON_PP9I
	39,     // WEAPON_CC13
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

int progressivePistolNumbers[11] = {
    WEAPON_UNARMED,
    WEAPON_CC13,
    WEAPON_FALCON2,
    WEAPON_FALCON2_SILENCER,
    WEAPON_FALCON2_SCOPE,
    WEAPON_PP9I,
    WEAPON_MAGSEC4,
    WEAPON_DY357MAGNUM,
    WEAPON_MAULER,
    WEAPON_PHOENIX,
    WEAPON_DY357LX,
};

int progressiveSMGNumbers[10] = {
    WEAPON_UNARMED,
    WEAPON_KL01313,
    WEAPON_DMC,
    WEAPON_ZZT,
    WEAPON_CMP150,
    WEAPON_CYCLONE,
    WEAPON_LAPTOPGUN,
    WEAPON_CALLISTO,
    WEAPON_RCP45,
    WEAPON_RCP120,
};

int progressiveRifleNumbers[7] = {
    WEAPON_UNARMED,
    WEAPON_KF7SPECIAL,
    WEAPON_DRAGON,
    WEAPON_AR34,
    WEAPON_K7AVENGER,
    WEAPON_AR53,
    WEAPON_SUPERDRAGON,
};

int progressiveExplosiveNumbers[9] = {
    WEAPON_UNARMED,
    WEAPON_TIMEDMINE,
    WEAPON_PROXIMITYMINE,
    WEAPON_GRENADE,
    WEAPON_SLAYER,
    WEAPON_REMOTEMINE,
    WEAPON_NBOMB,
    WEAPON_ROCKETLAUNCHER,
    WEAPON_DEVASTATOR,
};

int progressiveOtherWeaponNumbers[10] = {
    WEAPON_UNARMED,
    WEAPON_COMBATKNIFE,
    WEAPON_PSYCHOSISGUN,
    WEAPON_TRANQUILIZER,   
    WEAPON_LASER,
    WEAPON_CROSSBOW,
    WEAPON_SNIPERRIFLE,
    WEAPON_SHOTGUN,
    WEAPON_REAPER,
    WEAPON_FARSIGHT,
};

void resetAP(int restartGame)
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

    unlockedWeapons[WEAPON_NONE] = 1;
    unlockedWeapons[WEAPON_UNARMED] = 1;
    unlockedWeapons[WEAPON_DISABLED] = 1;

    for (i = 0; i < ARRAYCOUNT(unlockedCheats); i++) {
		unlockedCheats[i] = 0;
	}

    for (i = 0; i < ARRAYCOUNT(unlockedCharacters); i++) {
		unlockedCharacters[i] = 0;
	}

    completionGoal = 0;
    skedarRequirements = 0;

    hasNPCs = 0;

    if (restartGame) {
        titleSetNextStage(STAGE_CITRAINING);
        setNumPlayers(1);
        titleSetNextMode(TITLEMODE_SKIP);
        mainChangeToStage(STAGE_CITRAINING);
    }
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
	if (hasCompletionCheats == 1) {
		InternalCollectAPItem(location);
	}
}

void collectTimedCheatItem(u8 missionIndex)
{
    uint64_t location = missionIndex + AP_TIMED_CHEAT_OFFSET;
	if (hasTimedCheats == 1) {
		InternalCollectAPItem(location);
	}
}

void collectClassicWeaponCheatItem(u8 weaponIndex)
{
    uint64_t location = (weaponIndex - WEAPON_PP9I) + AP_CLASSIC_WEAPON_CHEAT_OFFSET;
	if (hasWeaponCheats == 1) {
		InternalCollectAPItem(location);
	}
}

void collectMPFeatureItem(u32 mpFeature)
{
    uint64_t location = mpFeature + AP_MPFEATURE_OFFSET;
	if (hasMPUnlocks == 1) {
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

    if (showLocationName) {
        sprintf(buffer, "Sent %s to %s (%s)\n", itemname, recipient, location);
    }
    else {
        sprintf(buffer, "Sent %s to %s\n", itemname, recipient);
    }
    
	hudmsgCreate(buffer, HUDMSGTYPE_DEFAULT);
}

void handleItem(int itemID, const char* itemname, const char* sender, const char* location, int itemIndex)
{
    char buffer[300] = "";

    if (itemIndex > lastReceivedItemIndex) {
        itemname = CheckString(itemname);
        sender = CheckString(sender);
        location = CheckString(location);

        // Print item received message
        for (s32 i = 0; i < PLAYERCOUNT(); i++) {
            setCurrentPlayerNum(i);

            if (showLocationName) {
                sprintf(buffer, "Got %s from %s (%s)\n", itemname, sender, location);
            }
            else {
                sprintf(buffer, "Got %s from %s\n", itemname, sender);
            }
            
            hudmsgCreate(buffer, HUDMSGTYPE_DEFAULT);
        }

        lastReceivedItemIndex = itemIndex;
    }

    // Character
    if (itemID >= AP_ITEM_CHARACTER_START) {
        unlockedCharacters[itemID - AP_ITEM_CHARACTER_START] = 1;
        return;
    }

    // Progressive Weapon
    if ((weaponProgressionType == WEAPONPROG_ALLGUNS || weaponProgressionType == WEAPONPROG_ONEGUN) 
            && itemID == AP_ITEM_PROGRESSIVE_WEAPON) {
        if (progressiveWeapon < ARRAYCOUNT(progressiveWeaponNumbers) - 1) {
            progressiveWeapon += 1;
        }

        unlockedWeapons[progressiveWeaponNumbers[progressiveWeapon]] = 1;

        for (s32 i = 0; i < PLAYERCOUNT(); i++) {
			setCurrentPlayerNum(i);

            if (g_Vars.stagenum != STAGE_CITRAINING && !g_Vars.normmplayerisrunning) {
                // Co-operative
                if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
                    invGiveSingleWeapon(progressiveWeaponNumbers[progressiveWeapon]);
                    
                    if (g_Vars.currentplayer->gunctrl.weaponnum != WEAPON_UNARMED) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
                else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
                    invRemoveLockedWeapons();

                    invGetProgressiveWeapons();

                    // Swap weapons if it is not in your inventory or still holding previous progressive weapon
                    if (g_Vars.currentplayer->gunctrl.weaponnum == progressiveWeaponNumbers[progressiveWeapon - 1]
                            || !invHasSingleWeaponExcAllGuns(g_Vars.currentplayer->gunctrl.weaponnum)) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
            }
            else if (g_Vars.stagenum != STAGE_CITRAINING
                        && g_Vars.normmplayerisrunning 
                        && allowProgWeaponInChallenges == 1) {
                // Allow Progressive Weapons in Challenges
                if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
                    invGiveSingleWeapon(progressiveWeaponNumbers[progressiveWeapon]);
                    
                    if (g_Vars.currentplayer->gunctrl.weaponnum != WEAPON_UNARMED) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
                else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
                    invRemoveLockedWeapons();

                    invGetProgressiveWeapons();

                    // Swap weapons if it is not in your inventory or still holding previous progressive weapon
                    if (g_Vars.currentplayer->gunctrl.weaponnum == progressiveWeaponNumbers[progressiveWeapon - 1]
                            || !invHasSingleWeaponExcAllGuns(g_Vars.currentplayer->gunctrl.weaponnum)) {
                        bgunEquipWeapon2(HAND_RIGHT, progressiveWeaponNumbers[progressiveWeapon]);
                    }
                }
            }
		}

        return;
    }

    // Master Keys
    if (itemID == AP_ITEM_DATADYNE_MASTER_KEY) {
        unlockedWeapons[WEAPON_NECKLACE] = 1;
        unlockedWeapons[WEAPON_KEYCARD4C] = 1;
        return;
    }
    else if (itemID == AP_ITEM_G5_BUILDING_MASTER_KEY) {
        unlockedWeapons[WEAPON_KEYCARD45] = 1;
        unlockedWeapons[WEAPON_KEYCARD46] = 1;
        return;
    }
    else if (itemID == AP_ITEM_AREA_51_MASTER_KEY) {
        unlockedWeapons[WEAPON_KEYCARD4B] = 1;
        unlockedWeapons[WEAPON_KEYCARD47] = 1;
        unlockedWeapons[WEAPON_KEYCARD48] = 1;
        return;
    }
    else if (itemID == AP_ITEM_AIR_FORCE_ONE_MASTER_KEY) {
        unlockedWeapons[WEAPON_KEYCARD49] = 1;
        unlockedWeapons[WEAPON_HAMMER] = 1;
        unlockedWeapons[WEAPON_SCREWDRIVER] = 1;
        return;
    }

    // Filler
	if (itemID == AP_ITEM_FILLER) {
        return;
	}

	// Mission Star
	if (itemID == AP_ITEM_MISSION_STAR) {
        missionStars += 1;

        if (completionGoal == 1
                || (completionGoal == 0 && skedarRequirements == 1)) {
            if (missionStars >= requiredMissionStars) {
                uint64_t location = AP_COLLECT_ALL_STARS_LOCATION;
                InternalCollectAPItem(location);
            }
        }

        if (completionGoal == 3
                || (completionGoal == 0 && skedarRequirements == 3)) {
            if (missionStars >= requiredMissionStars
                    && challengeStars >= requiredChallengeStars) {
                uint64_t location = AP_COLLECT_ALL_STARS_LOCATION;
                InternalCollectAPItem(location);
            }
        }

        return;
	}

    // Challenge Star
    if (itemID == AP_ITEM_CHALLENGE_STAR) {
        challengeStars += 1;

        if (completionGoal == 2
                || (completionGoal == 0 && skedarRequirements == 2)) {
            if (challengeStars >= requiredChallengeStars) {
                uint64_t location = AP_COLLECT_ALL_STARS_LOCATION;
                InternalCollectAPItem(location);
            }
        }

        if (completionGoal == 3
                || (completionGoal == 0 && skedarRequirements == 3)) {
            if (missionStars >= requiredMissionStars
                    && challengeStars >= requiredChallengeStars) {
                uint64_t location = AP_COLLECT_ALL_STARS_LOCATION;
                InternalCollectAPItem(location);
            }
        }

        return;
    }

    // Give Skedar Ruins on any difficulty that you have a mission in
    if (itemID == AP_ITEM_SKEDAR_RUINS) {
        for (s32 i = 0; i < 21; i++) {
            if (unlockedMissions[i][0] == 1) {
                unlockedMissions[16][0] = 1;
            }
            if (unlockedMissions[i][1] == 1) {
                unlockedMissions[16][1] = 1;
            }
            if (unlockedMissions[i][2] == 1) {
                unlockedMissions[16][2] = 1;
            }
        }

        // Give Skedar Ruins on all difficulties if you played no missions
        if (unlockedMissions[16][0] == 0
                && unlockedMissions[16][1] == 0
                && unlockedMissions[16][2] == 0) {
            unlockedMissions[16][0] = 1;
            unlockedMissions[16][1] = 1;
            unlockedMissions[16][2] = 1;
        }

        return;
    }

    // Victory
    if (itemID == AP_ITEM_VICTORY) {
        ReachedGoal();
        return;
    }

    // Progressive Weapon Type
    if (weaponProgressionType == WEAPONPROG_TYPES && itemID >= AP_ITEM_PROG_PISTOL) {
        if (itemID == AP_ITEM_PROG_PISTOL) {
            if (progressivePistol < ARRAYCOUNT(progressivePistolNumbers) - 1) {
                progressivePistol += 1;
            }
            unlockedWeapons[progressivePistolNumbers[progressivePistol]] = 1;
            GetNextProgressiveWeapon(progressivePistolNumbers, progressivePistol);
        }
        else if (itemID == AP_ITEM_PROG_SMG) {
            if (progressiveSMG < ARRAYCOUNT(progressiveSMGNumbers) - 1) {
                progressiveSMG += 1;
            }
            unlockedWeapons[progressiveSMGNumbers[progressiveSMG]] = 1;
            GetNextProgressiveWeapon(progressiveSMGNumbers, progressiveSMG);
        }
        else if (itemID == AP_ITEM_PROG_RIFLE) {
            if (progressiveRifle < ARRAYCOUNT(progressiveRifleNumbers) - 1) {
                progressiveRifle += 1;
            }
            unlockedWeapons[progressiveRifleNumbers[progressiveRifle]] = 1;
            GetNextProgressiveWeapon(progressiveRifleNumbers, progressiveRifle);
        }
        else if (itemID == AP_ITEM_PROG_EXPLOSIVE) {
            if (progressiveExplosive < ARRAYCOUNT(progressiveExplosiveNumbers) - 1) {
                progressiveExplosive += 1;
            }
            unlockedWeapons[progressiveExplosiveNumbers[progressiveExplosive]] = 1;
            GetNextProgressiveWeapon(progressiveExplosiveNumbers, progressiveExplosive);
        }
        else if (itemID == AP_ITEM_PROG_OTHER_WEAPON) {
            if (progressiveOtherWeapon < ARRAYCOUNT(progressiveOtherWeaponNumbers) - 1) {
                progressiveOtherWeapon += 1;
            }
            unlockedWeapons[progressiveOtherWeaponNumbers[progressiveOtherWeapon]] = 1;
            GetNextProgressiveWeapon(progressiveOtherWeaponNumbers, progressiveOtherWeapon);
        }

        return;
    }    

    // Weapons, Missions, Challenges, Cheats
    if (itemID < AP_ITEM_AGENT_START) {
        u8 weaponnum = itemID - 1;
        unlockedWeapons[weaponnum] = 1;

        if (weaponnum == WEAPON_CROSSBOW) {
            unlockedWeapons[WEAPON_BOLT] = 1;
        }

        // Get starting weapons during the mission
        if (weaponProgressionType == WEAPONPROG_DISABLED && !g_Vars.normmplayerisrunning) {    
            switch (weaponnum) {
                case WEAPON_FALCON2:
                    if (g_Vars.stagenum == STAGE_INVESTIGATION
                            || g_Vars.stagenum == STAGE_INFILTRATION) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_FALCON2_SILENCER:
                    if (g_Vars.stagenum == STAGE_DEFECTION
                            || g_Vars.stagenum == STAGE_G5BUILDING
                            || g_Vars.stagenum == STAGE_RESCUE
                            || g_Vars.stagenum == STAGE_PELAGIC) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_FALCON2_SCOPE:
                    if (g_Vars.stagenum == STAGE_EXTRACTION
                            || g_Vars.stagenum == STAGE_CHICAGO
                            || g_Vars.stagenum == STAGE_ESCAPE
                            || g_Vars.stagenum == STAGE_CRASHSITE
                            || g_Vars.stagenum == STAGE_DEEPSEA
                            || g_Vars.stagenum == STAGE_SKEDARRUINS
                            || g_Vars.stagenum == STAGE_DUEL) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_SNIPERRIFLE:
                    if (g_Vars.stagenum == STAGE_VILLA
                            && g_MissionConfig.difficulty <= DIFF_SA) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_LAPTOPGUN:
                    if ((g_Vars.stagenum == STAGE_VILLA
                                && g_MissionConfig.difficulty == DIFF_PA)
                            || (g_Vars.stagenum == STAGE_AIRFORCEONE
                                && g_MissionConfig.difficulty == DIFF_A)
                            || g_Vars.stagenum == STAGE_PELAGIC) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_REMOTEMINE:
                    if ((g_Vars.stagenum == STAGE_G5BUILDING
                                && g_MissionConfig.difficulty >= DIFF_SA)
                            || g_Vars.stagenum == STAGE_CRASHSITE) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_CROSSBOW:
                    if (g_Vars.stagenum == STAGE_AIRBASE) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_TIMEDMINE:
                    if (g_Vars.stagenum == STAGE_AIRFORCEONE
                            && g_MissionConfig.difficulty == DIFF_A) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_NBOMB:
                    if (g_Vars.stagenum == STAGE_PELAGIC) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_SHOTGUN:
                    if (g_Vars.stagenum == STAGE_DEEPSEA) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_AR34:
                case WEAPON_LASER:
                    if (g_Vars.stagenum == STAGE_DEFENSE) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_COMBATKNIFE:
                    if (g_Vars.stagenum == STAGE_ATTACKSHIP) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_CALLISTO:
                case WEAPON_DEVASTATOR:
                    if (g_Vars.stagenum == STAGE_SKEDARRUINS) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_MAULER:
                    if (g_Vars.stagenum == STAGE_MBR) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
                case WEAPON_PHOENIX:
                    if (g_Vars.stagenum == STAGE_MAIANSOS) {
                        invGiveSingleWeapon(weaponnum);
                    }
                    break;
            }
        }
        else if (weaponProgressionType == WEAPONPROG_VANILLA_ALLGUNS) {
            if (g_Vars.stagenum != STAGE_CITRAINING
                    && ((g_Vars.normmplayerisrunning 
                        && allowProgWeaponInChallenges == 1)
                    || !g_Vars.normmplayerisrunning)) {
                invGiveSingleWeapon(weaponnum);
            }
        }

        // Get starting mission items during the mission
        switch (weaponnum) {
            case WEAPON_ECMMINE:
                if (g_Vars.stagenum == STAGE_DEFECTION) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_DATAUPLINK:
                if (g_Vars.stagenum == STAGE_DEFECTION
                        || g_Vars.stagenum == STAGE_INVESTIGATION
                        || g_Vars.stagenum == STAGE_RESCUE
                        || g_Vars.stagenum == STAGE_DEFENSE) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_EYESPY:
                if (g_Vars.stagenum == STAGE_INVESTIGATION
                        || g_Vars.stagenum == STAGE_G5BUILDING
                        || g_Vars.stagenum == STAGE_AIRBASE
                        || g_Vars.stagenum == STAGE_MBR) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_NIGHTVISION:
                if (g_Vars.stagenum == STAGE_EXTRACTION
                        || g_Vars.stagenum == STAGE_CRASHSITE) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_RTRACKER:
                if (g_Vars.stagenum == STAGE_VILLA
                        || g_Vars.stagenum == STAGE_SKEDARRUINS) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_TRACERBUG:
                if (g_Vars.stagenum == STAGE_CHICAGO
                        && g_MissionConfig.difficulty == DIFF_PA) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_DOORDECODER:
                if (g_Vars.stagenum == STAGE_G5BUILDING) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_COMMSRIDER:
            case WEAPON_EXPLOSIVES:
                if (g_Vars.stagenum == STAGE_INFILTRATION) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_XRAYSCANNER:
                if (g_Vars.stagenum == STAGE_RESCUE
                        || g_Vars.stagenum == STAGE_PELAGIC) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_AUTOSURGEON:
                if (g_Vars.stagenum == STAGE_ESCAPE
                        && g_MissionConfig.difficulty <= DIFF_SA) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_HORIZONSCANNER:
                if (g_Vars.stagenum == STAGE_AIRBASE
                        || g_Vars.stagenum == STAGE_CRASHSITE) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_COMBATBOOST:
                if (g_Vars.stagenum == STAGE_AIRFORCEONE
                        || g_Vars.stagenum == STAGE_DEFENSE) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_SUITCASE:
                if (g_Vars.stagenum == STAGE_AIRFORCEONE
                        && g_MissionConfig.difficulty == DIFF_A) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_PRESIDENTSCANNER:
                if (g_Vars.stagenum == STAGE_CRASHSITE
                        && g_MissionConfig.difficulty == DIFF_A) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_IRSCANNER:
                if (g_Vars.stagenum == STAGE_DEEPSEA
                        || g_Vars.stagenum == STAGE_SKEDARRUINS) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_BACKUPDISK:
                if (g_Vars.stagenum == STAGE_DEEPSEA
                        && g_MissionConfig.difficulty == DIFF_PA) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_TARGETAMPLIFIER:
                if (g_Vars.stagenum == STAGE_SKEDARRUINS) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
            case WEAPON_CLOAKINGDEVICE:
            case WEAPON_SKEDARBOMB:
                if (g_Vars.stagenum == STAGE_MBR) {
                    invGiveSingleWeapon(weaponnum);
                }
                break;
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

void GetNextProgressiveWeapon(int weaponNumbersArray[], int currentWeaponNumber) {
    for (s32 i = 0; i < PLAYERCOUNT(); i++) {
        setCurrentPlayerNum(i);

        if (g_Vars.stagenum != STAGE_CITRAINING && !g_Vars.normmplayerisrunning) {
            // Co-operative
            if (currentWeaponNumber > 1) {
                invRemoveItemByNum(weaponNumbersArray[currentWeaponNumber - 1]);
            }

            invGiveSingleWeapon(weaponNumbersArray[currentWeaponNumber]);
            
            // Switch to weapon if you are still holding the one that was removed
            if (g_Vars.currentplayer->gunctrl.weaponnum == weaponNumbersArray[currentWeaponNumber - 1]) {
                bgunEquipWeapon2(HAND_RIGHT, weaponNumbersArray[currentWeaponNumber]);
            }
        }
        else if (g_Vars.stagenum != STAGE_CITRAINING
                && g_Vars.normmplayerisrunning 
                && allowProgWeaponInChallenges == 1) {
            // Allow Progressive Weapons in Challenges
            if (currentWeaponNumber > 1) {
                invRemoveItemByNum(weaponNumbersArray[currentWeaponNumber - 1]);
            }

            invGiveSingleWeapon(weaponNumbersArray[currentWeaponNumber]);

            // Switch to weapon if you are still holding the one that was removed
            if (g_Vars.currentplayer->gunctrl.weaponnum == weaponNumbersArray[currentWeaponNumber - 1]) {
                bgunEquipWeapon2(HAND_RIGHT, weaponNumbersArray[currentWeaponNumber]);
            }
        }
    }
}
