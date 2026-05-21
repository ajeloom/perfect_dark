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

#define AP_OBJECTIVE_OFFSET 1
#define AP_MISSION_OFFSET 247
#define AP_CHALLENGE_OFFSET 310
#define AP_FIRING_RANGE_OFFSET 340
#define AP_DEVICE_TRAINING_OFFSET 439
#define AP_HOLOTRAINING_OFFSET 449

u32 completedMissions[NUM_SOLOSTAGES][3];
u32 completedAgentObjectives[NUM_SOLOSTAGES][3];
u32 completedSpecialAgentObjectives[NUM_SOLOSTAGES][4];
u32 completedPerfectAgentObjectives[NUM_SOLOSTAGES][5];
u32 completedChallenges[30];
u32 completedTrainingMedals[32][3];

u32 unlockedMissions[NUM_SOLOSTAGES][3];
u32 unlockedChallenges[30];
u32 unlockedWeapons[94];

int missionStars = 0;
extern int requiredMissionStars;

int progressiveWeapon = 0;
extern int weaponProgressionType;
extern int allowProgWeaponInChallenges;

extern int hasChallenges;
extern int hasWeaponTraining;
extern int hasDeviceTraining;
extern int hasHolotraining;

int progressiveWeaponNumbers[43] = {
    WEAPON_UNARMED,
    WEAPON_COMBATKNIFE,
    WEAPON_TRANQUILIZER,
    WEAPON_KL01313,
    WEAPON_LASER,
    WEAPON_FALCON2,
    WEAPON_FALCON2_SILENCER,
    WEAPON_FALCON2_SCOPE,
    WEAPON_PP9I,
    WEAPON_CC13,
    WEAPON_MAGSEC4,
    WEAPON_SNIPERRIFLE,
    WEAPON_DY357MAGNUM,
    WEAPON_SHOTGUN,
    WEAPON_KF7SPECIAL,
    WEAPON_DMC,
    WEAPON_ZZT,
    WEAPON_CMP150,
    WEAPON_DRAGON,
    WEAPON_CYCLONE,
    WEAPON_REAPER,
    WEAPON_AR34,
    WEAPON_LAPTOPGUN,
    WEAPON_PSYCHOSISGUN,
    WEAPON_TIMEDMINE,
    WEAPON_GRENADE,
    WEAPON_PROXIMITYMINE,
    WEAPON_REMOTEMINE,
    WEAPON_NBOMB,
    WEAPON_K7AVENGER,
    WEAPON_CALLISTO,
    WEAPON_AR53,
    WEAPON_SUPERDRAGON,
    WEAPON_SLAYER,
    WEAPON_ROCKETLAUNCHER,
    WEAPON_DEVASTATOR,
    WEAPON_CROSSBOW,
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
	39,     // WEAPON_FALCON2
	38,     // WEAPON_FALCON2_SILENCER
	37,     // WEAPON_FALCON2_SCOPE
	34,     // WEAPON_MAGSEC4
	7,      // WEAPON_MAULER
	6,      // WEAPON_PHOENIX
	32,     // WEAPON_DY357MAGNUM
	3,      // WEAPON_DY357LX
	27,     // WEAPON_CMP150
	25,     // WEAPON_CYCLONE
	14,     // WEAPON_CALLISTO
	4,      // WEAPON_RCP120
	22,     // WEAPON_LAPTOPGUN
	26,     // WEAPON_DRAGON
	15,     // WEAPON_K7AVENGER
	23,     // WEAPON_AR34
	12,     // WEAPON_SUPERDRAGON
	31,     // WEAPON_SHOTGUN
	24,     // WEAPON_REAPER
	33,     // WEAPON_SNIPERRIFLE
	2,      // WEAPON_FARSIGHT
	9,      // WEAPON_DEVASTATOR
	10,      // WEAPON_ROCKETLAUNCHER
	11,     // WEAPON_SLAYER
	43,     // WEAPON_COMBATKNIFE
	8,      // WEAPON_CROSSBOW
	42,     // WEAPON_TRANQUILIZER
	40,     // WEAPON_LASER
	19,     // WEAPON_GRENADE
	16,     // WEAPON_NBOMB
	20,     // WEAPON_TIMEDMINE
	18,     // WEAPON_PROXIMITYMINE
	17,     // WEAPON_REMOTEMINE
	44,     // WEAPON_COMBATBOOST
	36,     // WEAPON_PP9I
	35,     // WEAPON_CC13
	41,     // WEAPON_KL01313
	30,     // WEAPON_KF7SPECIAL
	28,     // WEAPON_ZZT
	29,     // WEAPON_DMC
	13,     // WEAPON_AR53
	5,      // WEAPON_RCP45
	21,     // WEAPON_PSYCHOSISGUN
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
            APOffset = 1;
            difficultyOffset = 3;
            break;
        case 1:
            APOffset = 62;
            difficultyOffset = 4;
            break;
        case 2:
            APOffset = 144;
            difficultyOffset = 5;
            break;
        default:
            APOffset = 1;
            difficultyOffset = 3;
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

    sprintf(buffer, "Got %s from %s (%s)\n", itemname, sender, location);
    hudmsgCreate(buffer, HUDMSGTYPE_DEFAULT);

    // Progressive Weapon
    if (weaponProgressionType != WEAPONPROG_DISABLED && itemID == 188) {
        progressiveWeapon += 1;

        unlockedWeapons[progressiveWeaponNumbers[progressiveWeapon]] = 1;

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

        return;
    }

    // Filler
	if (itemID == 189) {
        return;
	}

	// Mission Star
	if (itemID == 191) {
        missionStars += 1;

        if (missionStars >= requiredMissionStars) {
            ReachedGoal();
        }

        return;
	}

    // Victory
    if (itemID == 192) {
        ReachedGoal();
        return;
    }

    if (itemID < 95) {
        unlockedWeapons[itemID - 1] = 1;

        if ((itemID - 1) == WEAPON_CROSSBOW) {
            unlockedWeapons[WEAPON_BOLT] = 1;
        }
    }
    else if (itemID < 116 && itemID >= 95) {
        // Agent
        unlockedMissions[itemID - 95][0]= 1;
    }
    else if (itemID < 137 && itemID >= 116) {
        // Special Agent
        unlockedMissions[itemID - 116][1]= 1;
    }
    else if (itemID < 158 && itemID >= 137) {
        // Perfect Agent
        unlockedMissions[itemID - 137][2]= 1;
    }
    else if (itemID >= 158) {
        unlockedChallenges[itemID - 158] = 1;
    }
}
