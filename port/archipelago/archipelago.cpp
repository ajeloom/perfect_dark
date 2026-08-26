#include "archipelago.h"
#include "apmenu.h"

#include "apclient.hpp"
#include "apuuid.hpp"
#include "defaultdatapackagestore.hpp"
#include <map>

extern "C" {
    #include "itemhandler.h"
}

using nlohmann::json;

#ifdef __EMSCRIPTEN__
#define DATAPACKAGE_CACHE "/settings/datapackage.json"
#define UUID_FILE "/settings/uuid"
#else
#define DATAPACKAGE_CACHE "datapackage.json"
#define UUID_FILE "uuid"
#define CERT_STORE "cacert.pem"
#endif

bool ap_sync_queued = false;
APClient* ap;

#define VERSION_TUPLE {0, 6, 7}

char* clientVersion = "0.4.0";

std::string URI;
std::string slotName;
std::string password;
std::string status = "Not connected\n";

int nextCheckToGet = 0;
extern int lastReceivedItemIndex;

int completionGoal;
int skedarRequirements;

int missionLogic;
int hasAgent;
int hasSpecialAgent;
int hasPerfectAgent;

extern int missionStars;
int requiredAgentMissionStars;
int requiredSpecialAgentMissionStars;
int requiredPerfectAgentMissionStars;
int requiredMissionStars = 0;


extern int progressiveWeapon;
extern int progressivePistol;
extern int progressiveSMG;
extern int progressiveRifle;
extern int progressiveExplosive;
extern int progressiveOtherWeapon;
int weaponProgressionType;
int allowProgWeaponInChallenges;

int hasMasterKey;

int hasChallenges;
extern int challengeStars;
int requiredChallengeStars;
int challengeLogic;
int shorterChallenges = 0;

int hasWeaponTraining;
int hasDeviceTraining;
int hasHolotraining;

int hasCompletionCheats = -1;
int hasTimedCheats = -1;
int hasWeaponCheats = -1;
int areCheatsInItemPool = -1;

int hasNPCs;
extern u32 unlockedCharacters[6];

int hasMPUnlocks;
int hasAlternateExits;

extern u32 completedMissions[21][3];
extern u32 completedChallenges[30];
extern u32 completedTrainingMedals[33][3];
extern u32 completedLocations[578];

int failedToConnectTotal = 0;

int deathLink;
bool pendingDeathLink;

std::string itemNames[] = {
    "NONE",
    "UNARMED",
    "Falcon 2",
    "Falcon 2 (Silencer)",
    "Falcon 2 (Scope)",
    "MagSec 4",
    "Mauler",
    "Phoenix",
    "DY357 Magnum",
    "DY357-LX",
    "CMP150",
    "Cyclone",
    "Callisto NTG",
    "RC-P120",
    "Laptop Gun",
    "Dragon",
    "K7 Avenger",
    "AR34",
    "SuperDragon",
    "Shotgun",
    "Reaper",
    "Sniper Rifle",
    "FarSight XR-20",
    "Devastator",
    "Rocket Launcher",
    "Slayer",
    "Combat Knife",
    "Crossbow",
    "Tranquilizer",
    "Laser",
    "Grenade",
    "N-Bomb",
    "Timed Mine",
    "Proximity Mine",
    "Remote Mine",
    "Combat Boost",
	"PP9i",
	"CC13",
	"KL01313",
	"KF7 Special",
	"ZZT (9mm)",
	"DMC",
	"AR53",
	"RC-P45",
    "Psychosis Gun",
	"Night Vision",
	"CamSpy",
	"X-Ray Scanner",
	"IR Scanner",
	"Cloaking Device",
	"Horizon Scanner",
    "TESTER",
    "ROCKETLAUNCHER_34",
	"ECM Mine",
	"Data Uplink",
	"R-Tracker",
	"President Scanner",
	"Door Decoder",
	"Alien Medpack",
	"Explosives",
	"Skedar Bomb",
	"Comms Rider",
	"Tracer Bug",
	"Target Amplifier",
	"Lab Clothes",
	"Stewardess Disguise",
	"Flight Plans",
	"Research Tape",
	"Backup Disk",
	"G5 Building Level 1 Key Card",
	"G5 Building Level 2 Key Card",
	"Medlab 2 Key Card",
	"Op Room Key Card",
	"Air Force One Lift Key Card",
	"Cellar Key Card",
	"Area 51 Lift Key Card",
	"Cassandra's Office Key Card",
	"Suitcase",
	"WEAPON_BRIEFCASE",
	"Shield Tech Item",
	"De Vries' Necklace",
    "Air Force One Left Room Key Card",  // "HAMMER",
    "Air Force One Right Room Key Card",  // "SCREWDRIVER",
    "ROCKET",
    "HOMINGROCKET",
    "GRENADEROUND",
    "BOLT",
    "Briefcase",
	"SKROCKET",
    "CHOPPERGUN",
    "WATCHLASER",
    "Shield",
	"DISABLED",
    "SUICIDEPILL",
    "dD Defection - Agent",
    "dD Investigation - Agent",
    "dD Extraction - Agent",
    "Carrington Villa - Agent",
    "Chicago - Agent",
    "G5 Building - Agent",
    "A51 Infiltration - Agent",
    "A51 Rescue - Agent",
    "A51 Escape - Agent",
    "Air Base - Agent",
    "Air Force One - Agent",
    "Crash Site - Agent",
    "Pelagic II - Agent",
    "Deep Sea - Agent",
    "CI Defense - Agent",
    "Attack Ship - Agent",
    "Skedar Ruins - Agent",
    "Mr. Blonde's Revenge - Agent",
    "Maian SOS - Agent",
    "WAR! - Agent",
    "The Duel - Agent",
    "dD Defection - Special Agent",
    "dD Investigation - Special Agent",
    "dD Extraction - Special Agent",
    "Carrington Villa - Special Agent",
    "Chicago - Special Agent",
    "G5 Building - Special Agent",
    "A51 Infiltration - Special Agent",
    "A51 Rescue - Special Agent",
    "A51 Escape - Special Agent",
    "Air Base - Special Agent",
    "Air Force One - Special Agent",
    "Crash Site - Special Agent",
    "Pelagic II - Special Agent",
    "Deep Sea - Special Agent",
    "CI Defense - Special Agent",
    "Attack Ship - Special Agent",
    "Skedar Ruins - Special Agent",
    "Mr. Blonde's Revenge - Special Agent",
    "Maian SOS - Special Agent",
    "WAR! - Special Agent",
    "The Duel - Special Agent",
    "dD Defection - Perfect Agent",
    "dD Investigation - Perfect Agent",
    "dD Extraction - Perfect Agent",
    "Carrington Villa - Perfect Agent",
    "Chicago - Perfect Agent",
    "G5 Building - Perfect Agent",
    "A51 Infiltration - Perfect Agent",
    "A51 Rescue - Perfect Agent",
    "A51 Escape - Perfect Agent",
    "Air Base - Perfect Agent",
    "Air Force One - Perfect Agent",
    "Crash Site - Perfect Agent",
    "Pelagic II - Perfect Agent",
    "Deep Sea - Perfect Agent",
    "CI Defense - Perfect Agent",
    "Attack Ship - Perfect Agent",
    "Skedar Ruins - Perfect Agent",
    "Mr. Blonde's Revenge - Perfect Agent",
    "Maian SOS - Perfect Agent",
    "WAR! - Perfect Agent",
    "The Duel - Perfect Agent",
	"Challenge 1",
	"Challenge 2",
	"Challenge 3",
	"Challenge 4",
	"Challenge 5",
	"Challenge 6",
	"Challenge 7",
	"Challenge 8",
	"Challenge 9",
	"Challenge 10",
	"Challenge 11",
	"Challenge 12",
	"Challenge 13",
	"Challenge 14",
	"Challenge 15",
	"Challenge 16",
	"Challenge 17",
	"Challenge 18",
	"Challenge 19",
	"Challenge 20",
	"Challenge 21",
	"Challenge 22",
	"Challenge 23",
	"Challenge 24",
	"Challenge 25",
	"Challenge 26",
	"Challenge 27",
	"Challenge 28",
	"Challenge 29",
	"Challenge 30",
    "Progressive Weapon",
    "Cheat: Hurricane Fists",
    "Cheat: Cloaking Device",
    "Cheat: Invincible",
    "Cheat: All Guns in Solo",
    "Cheat: Unlimited Ammo",
    "Cheat: Unlimited Ammo, No Reloads",
    "Cheat: Slo-mo Single Player",
    "Cheat: DK Mode",
    "Cheat: Trent's Magnum",
    "Cheat: FarSight",
    "Cheat: Small Jo",
    "Cheat: Small Characters",
    "Cheat: Enemy Shields",
    "Cheat: Jo Shield",
    "Cheat: Super Shield",
    "Cheat: Classic Sight",
    "Cheat: Team Heads Only",
    "Cheat: Play as Elvis",
    "Cheat: Enemy Rockets",
    "Cheat: Unlimited Ammo - Laptop Sentry Gun",
    "Cheat: Marquis of Queensbury Rules",
    "Cheat: Perfect Darkness",
    "Cheat: Pugilist",
    "Cheat: Hotshot",
    "Cheat: Hit and Run",
    "Cheat: Alien",
    "Cheat: R-Tracker/Weapon Cache Locations",
    "Cheat: Rocket Launcher",
    "Cheat: Sniper Rifle",
    "Cheat: X-Ray Scanner",
    "Cheat: SuperDragon",
    "Cheat: Laptop Gun",
    "Cheat: Phoenix",
    "Cheat: Psychosis Gun",
    "Cheat: PP9i",
    "Cheat: CC13",
    "Cheat: KL01313",
    "Cheat: KF7 Special",
    "Cheat: ZZT (9mm)",
    "Cheat: DMC",
    "Cheat: AR53",
    "Cheat: RC-P45",
    "dataDyne Master Key",
    "G5 Building Master Key",
    "Area 51 Master Key",
    "Air Force One Master Key",
    "Cheese",
    "Trap",
    "Mission Star",
    "Challenge Star",
    "Skedar Ruins",
    "Victory",
    "Progressive Pistol",
    "Progressive SMG",
    "Progressive Rifle",
    "Progressive Explosive",
    "Progressive Other Weapon",
    "Carrington",
    "Cassandra",
    "Dr. Caroll",
    "Jonathan",
    "Elvis",
    "President",
    "DK Mode Trap",
    "Small Jo Trap",
    "Small Characters Trap",
    "Slow Motion Trap"
};

std::map<int, std::string> locationNames = {
    {1, "dD Defection - Agent Objective 1"},
    {4, "dD Investigation - Agent Objective 1"},
    {5, "dD Investigation - Agent Objective 2"},
    {7, "dD Extraction - Agent Objective 1"},
    {8, "dD Extraction - Agent Objective 2"},
    {9, "dD Extraction - Agent Objective 3"},
    {10, "Carrington Villa - Agent Objective 1"},
    {11, "Carrington Villa - Agent Objective 2"},
    {12, "Carrington Villa - Agent Objective 3"},
    {13, "Chicago - Agent Objective 1"},
    {14, "Chicago - Agent Objective 2"},
    {15, "Chicago - Agent Objective 3"},
    {16, "G5 Building - Agent Objective 1"},
    {17, "G5 Building - Agent Objective 2"},
    {18, "G5 Building - Agent Objective 3"},
    {19, "A51 Infiltration - Agent Objective 1"},
    {20, "A51 Infiltration - Agent Objective 2"},
    {21, "A51 Infiltration - Agent Objective 3"},
    {22, "A51 Rescue - Agent Objective 1"},
    {23, "A51 Rescue - Agent Objective 2"},
    {24, "A51 Rescue - Agent Objective 3"},
    {25, "A51 Escape - Agent Objective 1"},
    {26, "A51 Escape - Agent Objective 2"},
    {27, "A51 Escape - Agent Objective 3"},
    {28, "Air Base - Agent Objective 1"},
    {29, "Air Base - Agent Objective 2"},
    {30, "Air Base - Agent Objective 3"},
    {31, "Air Force One - Agent Objective 1"},
    {32, "Air Force One - Agent Objective 2"},
    {33, "Air Force One - Agent Objective 3"},
    {34, "Crash Site - Agent Objective 1"},
    {35, "Crash Site - Agent Objective 2"},
    {36, "Crash Site - Agent Objective 3"},
    {37, "Pelagic II - Agent Objective 1"},
    {38, "Pelagic II - Agent Objective 2"},
    {39, "Pelagic II - Agent Objective 3"},
    {40, "Deep Sea - Agent Objective 1"},
    {41, "Deep Sea - Agent Objective 2"},
    {42, "Deep Sea - Agent Objective 3"},
    {43, "CI Defense - Agent Objective 1"},
    {44, "CI Defense - Agent Objective 2"},
    {45, "CI Defense - Agent Objective 3"},
    {46, "Attack Ship - Agent Objective 1"},
    {47, "Attack Ship - Agent Objective 2"},
    {48, "Attack Ship - Agent Objective 3"},
    {49, "Skedar Ruins - Agent Objective 1"},
    {50, "Skedar Ruins - Agent Objective 2"},
    {51, "Skedar Ruins - Agent Objective 3"},
    {52, "Mr. Blonde's Revenge - Agent Objective 1"},
    {55, "Maian SOS - Agent Objective 1"},
    {58, "WAR! - Agent Objective 1"},
    {61, "The Duel - Agent Objective 1"},
    {62, "dD Defection - Special Agent Objective 1"},
    {63, "dD Defection - Special Agent Objective 2"},
    {64, "dD Defection - Special Agent Objective 3"},
    {65, "dD Defection - Special Agent Objective 4"},
    {66, "dD Investigation - Special Agent Objective 1"},
    {67, "dD Investigation - Special Agent Objective 2"},
    {68, "dD Investigation - Special Agent Objective 3"},
    {69, "dD Investigation - Special Agent Objective 4"},
    {70, "dD Extraction - Special Agent Objective 1"},
    {71, "dD Extraction - Special Agent Objective 2"},
    {72, "dD Extraction - Special Agent Objective 3"},
    {73, "dD Extraction - Special Agent Objective 4"},
    {74, "Carrington Villa - Special Agent Objective 1"},
    {75, "Carrington Villa - Special Agent Objective 2"},
    {76, "Carrington Villa - Special Agent Objective 3"},
    {77, "Carrington Villa - Special Agent Objective 4"},
    {78, "Chicago - Special Agent Objective 1"},
    {79, "Chicago - Special Agent Objective 2"},
    {80, "Chicago - Special Agent Objective 3"},
    {81, "Chicago - Special Agent Objective 4"},
    {82, "G5 Building - Special Agent Objective 1"},
    {83, "G5 Building - Special Agent Objective 2"},
    {84, "G5 Building - Special Agent Objective 3"},
    {85, "G5 Building - Special Agent Objective 4"},
    {86, "A51 Infiltration - Special Agent Objective 1"},
    {87, "A51 Infiltration - Special Agent Objective 2"},
    {88, "A51 Infiltration - Special Agent Objective 3"},
    {89, "A51 Infiltration - Special Agent Objective 4"},
    {90, "A51 Rescue - Special Agent Objective 1"},
    {91, "A51 Rescue - Special Agent Objective 2"},
    {92, "A51 Rescue - Special Agent Objective 3"},
    {93, "A51 Rescue - Special Agent Objective 4"},
    {94, "A51 Escape - Special Agent Objective 1"},
    {95, "A51 Escape - Special Agent Objective 2"},
    {96, "A51 Escape - Special Agent Objective 3"},
    {97, "A51 Escape - Special Agent Objective 4"},
    {98, "Air Base - Special Agent Objective 1"},
    {99, "Air Base - Special Agent Objective 2"},
    {100, "Air Base - Special Agent Objective 3"},
    {101, "Air Base - Special Agent Objective 4"},
    {102, "Air Force One - Special Agent Objective 1"},
    {103, "Air Force One - Special Agent Objective 2"},
    {104, "Air Force One - Special Agent Objective 3"},
    {105, "Air Force One - Special Agent Objective 4"},
    {106, "Crash Site - Special Agent Objective 1"},
    {107, "Crash Site - Special Agent Objective 2"},
    {108, "Crash Site - Special Agent Objective 3"},
    {109, "Crash Site - Special Agent Objective 4"},
    {110, "Pelagic II - Special Agent Objective 1"},
    {111, "Pelagic II - Special Agent Objective 2"},
    {112, "Pelagic II - Special Agent Objective 3"},
    {113, "Pelagic II - Special Agent Objective 4"},
    {114, "Deep Sea - Special Agent Objective 1"},
    {115, "Deep Sea - Special Agent Objective 2"},
    {116, "Deep Sea - Special Agent Objective 3"},
    {117, "Deep Sea - Special Agent Objective 4"},
    {118, "CI Defense - Special Agent Objective 1"},
    {119, "CI Defense - Special Agent Objective 2"},
    {120, "CI Defense - Special Agent Objective 3"},
    {121, "CI Defense - Special Agent Objective 4"},
    {122, "Attack Ship - Special Agent Objective 1"},
    {123, "Attack Ship - Special Agent Objective 2"},
    {124, "Attack Ship - Special Agent Objective 3"},
    {125, "Attack Ship - Special Agent Objective 4"},
    {126, "Skedar Ruins - Special Agent Objective 1"},
    {127, "Skedar Ruins - Special Agent Objective 2"},
    {128, "Skedar Ruins - Special Agent Objective 3"},
    {129, "Skedar Ruins - Special Agent Objective 4"},
    {130, "Mr. Blonde's Revenge - Special Agent Objective 1"},
    {131, "Mr. Blonde's Revenge - Special Agent Objective 2"},
    {134, "Maian SOS - Special Agent Objective 1"},
    {135, "Maian SOS - Special Agent Objective 2"},
    {138, "WAR! - Special Agent Objective 1"},
    {139, "WAR! - Special Agent Objective 2"},
    {142, "The Duel - Special Agent Objective 1"},
    {143, "The Duel - Special Agent Objective 2"},
    {144, "dD Defection - Perfect Agent Objective 1"},
    {145, "dD Defection - Perfect Agent Objective 2"},
    {146, "dD Defection - Perfect Agent Objective 3"},
    {147, "dD Defection - Perfect Agent Objective 4"},
    {148, "dD Defection - Perfect Agent Objective 5"},
    {149, "dD Investigation - Perfect Agent Objective 1"},
    {150, "dD Investigation - Perfect Agent Objective 2"},
    {151, "dD Investigation - Perfect Agent Objective 3"},
    {152, "dD Investigation - Perfect Agent Objective 4"},
    {153, "dD Investigation - Perfect Agent Objective 5"},
    {154, "dD Extraction - Perfect Agent Objective 1"},
    {155, "dD Extraction - Perfect Agent Objective 2"},
    {156, "dD Extraction - Perfect Agent Objective 3"},
    {157, "dD Extraction - Perfect Agent Objective 4"},
    {158, "dD Extraction - Perfect Agent Objective 5"},
    {159, "Carrington Villa - Perfect Agent Objective 1"},
    {160, "Carrington Villa - Perfect Agent Objective 2"},
    {161, "Carrington Villa - Perfect Agent Objective 3"},
    {162, "Carrington Villa - Perfect Agent Objective 4"},
    {163, "Carrington Villa - Perfect Agent Objective 5"},
    {164, "Chicago - Perfect Agent Objective 1"},
    {165, "Chicago - Perfect Agent Objective 2"},
    {166, "Chicago - Perfect Agent Objective 3"},
    {167, "Chicago - Perfect Agent Objective 4"},
    {168, "Chicago - Perfect Agent Objective 5"},
    {169, "G5 Building - Perfect Agent Objective 1"},
    {170, "G5 Building - Perfect Agent Objective 2"},
    {171, "G5 Building - Perfect Agent Objective 3"},
    {172, "G5 Building - Perfect Agent Objective 4"},
    {173, "G5 Building - Perfect Agent Objective 5"},
    {174, "A51 Infiltration - Perfect Agent Objective 1"},
    {175, "A51 Infiltration - Perfect Agent Objective 2"},
    {176, "A51 Infiltration - Perfect Agent Objective 3"},
    {177, "A51 Infiltration - Perfect Agent Objective 4"},
    {178, "A51 Infiltration - Perfect Agent Objective 5"},
    {179, "A51 Rescue - Perfect Agent Objective 1"},
    {180, "A51 Rescue - Perfect Agent Objective 2"},
    {181, "A51 Rescue - Perfect Agent Objective 3"},
    {182, "A51 Rescue - Perfect Agent Objective 4"},
    {183, "A51 Rescue - Perfect Agent Objective 5"},
    {184, "A51 Escape - Perfect Agent Objective 1"},
    {185, "A51 Escape - Perfect Agent Objective 2"},
    {186, "A51 Escape - Perfect Agent Objective 3"},
    {187, "A51 Escape - Perfect Agent Objective 4"},
    {188, "A51 Escape - Perfect Agent Objective 5"},
    {189, "Air Base - Perfect Agent Objective 1"},
    {190, "Air Base - Perfect Agent Objective 2"},
    {191, "Air Base - Perfect Agent Objective 3"},
    {192, "Air Base - Perfect Agent Objective 4"},
    {193, "Air Base - Perfect Agent Objective 5"},
    {194, "Air Force One - Perfect Agent Objective 1"},
    {195, "Air Force One - Perfect Agent Objective 2"},
    {196, "Air Force One - Perfect Agent Objective 3"},
    {197, "Air Force One - Perfect Agent Objective 4"},
    {198, "Air Force One - Perfect Agent Objective 5"},
    {199, "Crash Site - Perfect Agent Objective 1"},
    {200, "Crash Site - Perfect Agent Objective 2"},
    {201, "Crash Site - Perfect Agent Objective 3"},
    {202, "Crash Site - Perfect Agent Objective 4"},
    {203, "Crash Site - Perfect Agent Objective 5"},
    {204, "Pelagic II - Perfect Agent Objective 1"},
    {205, "Pelagic II - Perfect Agent Objective 2"},
    {206, "Pelagic II - Perfect Agent Objective 3"},
    {207, "Pelagic II - Perfect Agent Objective 4"},
    {208, "Pelagic II - Perfect Agent Objective 5"},
    {209, "Deep Sea - Perfect Agent Objective 1"},
    {210, "Deep Sea - Perfect Agent Objective 2"},
    {211, "Deep Sea - Perfect Agent Objective 3"},
    {212, "Deep Sea - Perfect Agent Objective 4"},
    {213, "Deep Sea - Perfect Agent Objective 5"},
    {214, "CI Defense - Perfect Agent Objective 1"},
    {215, "CI Defense - Perfect Agent Objective 2"},
    {216, "CI Defense - Perfect Agent Objective 3"},
    {217, "CI Defense - Perfect Agent Objective 4"},
    {218, "CI Defense - Perfect Agent Objective 5"},
    {219, "Attack Ship - Perfect Agent Objective 1"},
    {220, "Attack Ship - Perfect Agent Objective 2"},
    {221, "Attack Ship - Perfect Agent Objective 3"},
    {222, "Attack Ship - Perfect Agent Objective 4"},
    {223, "Attack Ship - Perfect Agent Objective 5"},
    {224, "Skedar Ruins - Perfect Agent Objective 1"},
    {225, "Skedar Ruins - Perfect Agent Objective 2"},
    {226, "Skedar Ruins - Perfect Agent Objective 3"},
    {227, "Skedar Ruins - Perfect Agent Objective 4"},
    {228, "Skedar Ruins - Perfect Agent Objective 5"},
    {229, "Mr. Blonde's Revenge - Perfect Agent Objective 1"},
    {230, "Mr. Blonde's Revenge - Perfect Agent Objective 2"},
    {231, "Mr. Blonde's Revenge - Perfect Agent Objective 3"},
    {234, "Maian SOS - Perfect Agent Objective 1"},
    {235, "Maian SOS - Perfect Agent Objective 2"},
    {236, "Maian SOS - Perfect Agent Objective 3"},
    {239, "WAR! - Perfect Agent Objective 1"},
    {240, "WAR! - Perfect Agent Objective 2"},
    {241, "WAR! - Perfect Agent Objective 3"},
    {244, "The Duel - Perfect Agent Objective 1"},
    {245, "The Duel - Perfect Agent Objective 2"},
    {246, "The Duel - Perfect Agent Objective 3"},
    {247, "Complete: dD Defection - Agent"},
    {248, "Complete: dD Defection - Special Agent"},
    {249, "Complete: dD Defection - Perfect Agent"},
    {250, "Complete: dD Investigation - Agent"},
    {251, "Complete: dD Investigation - Special Agent"},
    {252, "Complete: dD Investigation - Perfect Agent"},
    {253, "Complete: dD Extraction - Agent"},
    {254, "Complete: dD Extraction - Special Agent"},
    {255, "Complete: dD Extraction - Perfect Agent"},
    {256, "Complete: Carrington Villa - Agent"},
    {257, "Complete: Carrington Villa - Special Agent"},
    {258, "Complete: Carrington Villa - Perfect Agent"},
    {259, "Complete: Chicago - Agent"},
    {260, "Complete: Chicago - Special Agent"},
    {261, "Complete: Chicago - Perfect Agent"},
    {262, "Complete: G5 Building - Agent"},
    {263, "Complete: G5 Building - Special Agent"},
    {264, "Complete: G5 Building - Perfect Agent"},
    {265, "Complete: A51 Infiltration - Agent"},
    {266, "Complete: A51 Infiltration - Special Agent"},
    {267, "Complete: A51 Infiltration - Perfect Agent"},
    {268, "Complete: A51 Rescue - Agent"},
    {269, "Complete: A51 Rescue - Special Agent"},
    {270, "Complete: A51 Rescue - Perfect Agent"},
    {271, "Complete: A51 Escape - Agent"},
    {272, "Complete: A51 Escape - Special Agent"},
    {273, "Complete: A51 Escape - Perfect Agent"},
    {274, "Complete: Air Base - Agent"},
    {275, "Complete: Air Base - Special Agent"},
    {276, "Complete: Air Base - Perfect Agent"},
    {277, "Complete: Air Force One - Agent"},
    {278, "Complete: Air Force One - Special Agent"},
    {279, "Complete: Air Force One - Perfect Agent"},
    {280, "Complete: Crash Site - Agent"},
    {281, "Complete: Crash Site - Special Agent"},
    {282, "Complete: Crash Site - Perfect Agent"},
    {283, "Complete: Pelagic II - Agent"},
    {284, "Complete: Pelagic II - Special Agent"},
    {285, "Complete: Pelagic II - Perfect Agent"},
    {286, "Complete: Deep Sea - Agent"},
    {287, "Complete: Deep Sea - Special Agent"},
    {288, "Complete: Deep Sea - Perfect Agent"},
    {289, "Complete: Carrington Institute - Agent"},
    {290, "Complete: Carrington Institute - Special Agent"},
    {291, "Complete: Carrington Institute - Perfect Agent"},
    {292, "Complete: Attack Ship - Agent"},
    {293, "Complete: Attack Ship - Special Agent"},
    {294, "Complete: Attack Ship - Perfect Agent"},
    {295, "Complete: Skedar Ruins - Agent"},
    {296, "Complete: Skedar Ruins - Special Agent"},
    {297, "Complete: Skedar Ruins - Perfect Agent"},
    {298, "Complete: Mr. Blonde's Revenge - Agent"},
    {299, "Complete: Mr. Blonde's Revenge - Special Agent"},
    {300, "Complete: Mr. Blonde's Revenge - Perfect Agent"},
    {301, "Complete: Maian SOS - Agent"},
    {302, "Complete: Maian SOS - Special Agent"},
    {303, "Complete: Maian SOS - Perfect Agent"},
    {304, "Complete: WAR! - Agent"},
    {305, "Complete: WAR! - Special Agent"},
    {306, "Complete: WAR! - Perfect Agent"},
    {307, "Complete: The Duel - Agent"},
    {308, "Complete: The Duel - Special Agent"},
    {309, "Complete: The Duel - Perfect Agent"},
    {310, "Complete: Challenge 1"},
    {311, "Complete: Challenge 2"},
    {312, "Complete: Challenge 3"},
    {313, "Complete: Challenge 4"},
    {314, "Complete: Challenge 5"},
    {315, "Complete: Challenge 6"},
    {316, "Complete: Challenge 7"},
    {317, "Complete: Challenge 8"},
    {318, "Complete: Challenge 9"},
    {319, "Complete: Challenge 10"},
    {320, "Complete: Challenge 11"},
    {321, "Complete: Challenge 12"},
    {322, "Complete: Challenge 13"},
    {323, "Complete: Challenge 14"},
    {324, "Complete: Challenge 15"},
    {325, "Complete: Challenge 16"},
    {326, "Complete: Challenge 17"},
    {327, "Complete: Challenge 18"},
    {328, "Complete: Challenge 19"},
    {329, "Complete: Challenge 20"},
    {330, "Complete: Challenge 21"},
    {331, "Complete: Challenge 22"},
    {332, "Complete: Challenge 23"},
    {333, "Complete: Challenge 24"},
    {334, "Complete: Challenge 25"},
    {335, "Complete: Challenge 26"},
    {336, "Complete: Challenge 27"},
    {337, "Complete: Challenge 28"},
    {338, "Complete: Challenge 29"},
    {339, "Complete: Challenge 30"},
    {340, "Firing Range: Falcon 2 - Bronze"},
    {341, "Firing Range: Falcon 2 - Silver"},
    {342, "Firing Range: Falcon 2 - Gold"},
    {343, "Firing Range: Falcon 2 (Silencer) - Bronze"},
    {344, "Firing Range: Falcon 2 (Silencer) - Silver"},
    {345, "Firing Range: Falcon 2 (Silencer) - Gold"},
    {346, "Firing Range: Falcon 2 (Scope) - Bronze"},
    {347, "Firing Range: Falcon 2 (Scope) - Silver"},
    {348, "Firing Range: Falcon 2 (Scope) - Gold"},
    {349, "Firing Range: MagSec 4 - Bronze"},
    {350, "Firing Range: MagSec 4 - Silver"},
    {351, "Firing Range: MagSec 4 - Gold"},
    {352, "Firing Range: Mauler - Bronze"},
    {353, "Firing Range: Mauler - Silver"},
    {354, "Firing Range: Mauler - Gold"},
    {355, "Firing Range: Phoenix - Bronze"},
    {356, "Firing Range: Phoenix - Silver"},
    {357, "Firing Range: Phoenix - Gold"},
    {358, "Firing Range: DY357 Magnum - Bronze"},
    {359, "Firing Range: DY357 Magnum - Silver"},
    {360, "Firing Range: DY357 Magnum - Gold"},
    {361, "Firing Range: DY357-LX - Bronze"},
    {362, "Firing Range: DY357-LX - Silver"},
    {363, "Firing Range: DY357-LX - Gold"},
    {364, "Firing Range: CMP150 - Bronze"},
    {365, "Firing Range: CMP150 - Silver"},
    {366, "Firing Range: CMP150 - Gold"},
    {367, "Firing Range: Cyclone - Bronze"},
    {368, "Firing Range: Cyclone - Silver"},
    {369, "Firing Range: Cyclone - Gold"},
    {370, "Firing Range: Callisto NTG - Bronze"},
    {371, "Firing Range: Callisto NTG - Silver"},
    {372, "Firing Range: Callisto NTG - Gold"},
    {373, "Firing Range: RC-P120 - Bronze"},
    {374, "Firing Range: RC-P120 - Silver"},
    {375, "Firing Range: RC-P120 - Gold"},
    {376, "Firing Range: Laptop Gun - Bronze"},
    {377, "Firing Range: Laptop Gun - Silver"},
    {378, "Firing Range: Laptop Gun - Gold"},
    {379, "Firing Range: Dragon - Bronze"},
    {380, "Firing Range: Dragon - Silver"},
    {381, "Firing Range: Dragon - Gold"},
    {382, "Firing Range: K7 Avenger - Bronze"},
    {383, "Firing Range: K7 Avenger - Silver"},
    {384, "Firing Range: K7 Avenger - Gold"},
    {385, "Firing Range: AR34 - Bronze"},
    {386, "Firing Range: AR34 - Silver"},
    {387, "Firing Range: AR34 - Gold"},
    {388, "Firing Range: SuperDragon - Bronze"},
    {389, "Firing Range: SuperDragon - Silver"},
    {390, "Firing Range: SuperDragon - Gold"},
    {391, "Firing Range: Shotgun - Bronze"},
    {392, "Firing Range: Shotgun - Silver"},
    {393, "Firing Range: Shotgun - Gold"},
    {394, "Firing Range: Reaper - Bronze"},
    {395, "Firing Range: Reaper - Silver"},
    {396, "Firing Range: Reaper - Gold"},
    {397, "Firing Range: Sniper Rifle - Bronze"},
    {398, "Firing Range: Sniper Rifle - Silver"},
    {399, "Firing Range: Sniper Rifle - Gold"},
    {400, "Firing Range: FarSight XR-20 - Bronze"},
    {401, "Firing Range: FarSight XR-20 - Silver"},
    {402, "Firing Range: FarSight XR-20 - Gold"},
    {403, "Firing Range: Devastator - Bronze"},
    {404, "Firing Range: Devastator - Silver"},
    {405, "Firing Range: Devastator - Gold"},
    {406, "Firing Range: Rocket Launcher - Bronze"},
    {407, "Firing Range: Rocket Launcher - Silver"},
    {408, "Firing Range: Rocket Launcher - Gold"},
    {409, "Firing Range: Slayer - Bronze"},
    {410, "Firing Range: Slayer - Silver"},
    {411, "Firing Range: Slayer - Gold"},
    {412, "Firing Range: Combat Knife - Bronze"},
    {413, "Firing Range: Combat Knife - Silver"},
    {414, "Firing Range: Combat Knife - Gold"},
    {415, "Firing Range: Crossbow - Bronze"},
    {416, "Firing Range: Crossbow - Silver"},
    {417, "Firing Range: Crossbow - Gold"},
    {418, "Firing Range: Tranquilizer - Bronze"},
    {419, "Firing Range: Tranquilizer - Silver"},
    {420, "Firing Range: Tranquilizer - Gold"},
    {421, "Firing Range: Laser - Bronze"},
    {422, "Firing Range: Laser - Silver"},
    {423, "Firing Range: Laser - Gold"},
    {424, "Firing Range: Grenade - Bronze"},
    {425, "Firing Range: Grenade - Silver"},
    {426, "Firing Range: Grenade - Gold"},
    // {427, "Firing Range: N-Bomb - Bronze"},
    // {428, "Firing Range: N-Bomb - Silver"},
    // {429, "Firing Range: N-Bomb - Gold"},
    {430, "Firing Range: Timed Mine - Bronze"},
    {431, "Firing Range: Timed Mine - Silver"},
    {432, "Firing Range: Timed Mine - Gold"},
    {433, "Firing Range: Proximity Mine - Bronze"},
    {434, "Firing Range: Proximity Mine - Silver"},
    {435, "Firing Range: Proximity Mine - Gold"},
    {436, "Firing Range: Remote Mine - Bronze"},
    {437, "Firing Range: Remote Mine - Silver"},
    {438, "Firing Range: Remote Mine - Gold"},
    {439, "Device Training: Data Uplink"},
    {440, "Device Training: ECM Mine"},
    {441, "Device Training: CamSpy"},
    {442, "Device Training: Night Vision"},
    {443, "Device Training: Door Decoder"},
    {444, "Device Training: R-Tracker"},
    {445, "Device Training: IR Scanner"},
    {446, "Device Training: X-Ray Scanner"},
    {447, "Device Training: Disguise"},
    {448, "Device Training: Cloaking Device"},
    {449, "Holotraining 1: Looking Around"},
    {450, "Holotraining 2: Movement 1"},
    {451, "Holotraining 3: Movement 2"},
    {452, "Holotraining 4: Unarmed Combat 1"},
    {453, "Holotraining 5: Unarmed Combat 2"},
    {454, "Holotraining 6: Live Combat 1"},
    {455, "Holotraining 7: Live Combat 2"},
    {456, "Cheat Unlock: Complete dD Defection"},
    {457, "Cheat Unlock: Complete dD Investigation"},
    {458, "Cheat Unlock: Complete dD Extraction"},
    {459, "Cheat Unlock: Complete Carrington Villa"},
    {460, "Cheat Unlock: Complete Chicago"},
    {461, "Cheat Unlock: Complete G5 Building"},
    {462, "Cheat Unlock: Complete A51 Infiltration"},
    {463, "Cheat Unlock: Complete A51 Rescue"},
    {464, "Cheat Unlock: Complete A51 Escape"},
    {465, "Cheat Unlock: Complete Air Base"},
    {466, "Cheat Unlock: Complete Air Force One"},
    {467, "Cheat Unlock: Complete Crash Site"},
    {468, "Cheat Unlock: Complete Pelagic II"},
    {469, "Cheat Unlock: Complete Deep Sea"},
    {470, "Cheat Unlock: Complete CI Defense"},
    {471, "Cheat Unlock: Complete Attack Ship"},
    {472, "Cheat Unlock: Complete Skedar Ruins"},
    {473, "Cheat Unlock: Complete dD Defection (Special Agent) in under 1:30"},
    {474, "Cheat Unlock: Complete dD Investigation (Perfect Agent) in under 6:30"},
    {475, "Cheat Unlock: Complete dD Extraction (Agent) in under 2:03"},
    {476, "Cheat Unlock: Complete Carrington Villa (Special Agent) in under 2:30"},
    {477, "Cheat Unlock: Complete Chicago (Perfect Agent) in under 2:00"},
    {478, "Cheat Unlock: Complete G5 Building (Agent) in under 1:40"},
    {479, "Cheat Unlock: Complete A51 Infiltration (Special Agent) in under 5:00"},
    {480, "Cheat Unlock: Complete A51 Rescue (Perfect Agent) in under 7:59"},
    {481, "Cheat Unlock: Complete A51 Escape (Agent) in under 3:50"},
    {482, "Cheat Unlock: Complete Air Base (Special Agent) in under 3:11"},
    {483, "Cheat Unlock: Complete Air Force One (Perfect Agent) in under 3:55"},
    {484, "Cheat Unlock: Complete Crash Site (Agent) in under 2:50"},
    {485, "Cheat Unlock: Complete Pelagic II (Special Agent) in under 7:07"},
    {486, "Cheat Unlock: Complete Deep Sea (Perfect Agent) in under 7:27"},
    {487, "Cheat Unlock: Complete CI Defense (Agent) in under 1:45"},
    {488, "Cheat Unlock: Complete Attack Ship (Special Agent) in under 5:17"},
    {489, "Cheat Unlock: Complete Skedar Ruins (Perfect Agent) in under 5:31"},
    {490, "Cheat Unlock: Get gold medals for Falcon 2, Falcon 2 (Silencer), and Falcon 2 (Scope)"},
    {491, "Cheat Unlock: Get gold medals for MagSec 4, Mauler, Phoenix, DY357 Magnum, and DY357-LX"},
    {492, "Cheat Unlock: Get gold medals for CMP150, Cyclone, Callisto NTG, and RC-P120"},
    {493, "Cheat Unlock: Get gold medals for Laptop Gun, Dragon, K7 Avenger, AR34, and SuperDragon"},
    {494, "Cheat Unlock: Get gold medals for Shotgun, Sniper Rifle, Rocket Launcher, and Slayer"},
    {495, "Cheat Unlock: Get gold medals for Timed Mine, Proximity Mine, and Remote Mine"},
    {496, "Cheat Unlock: Get gold medals for FarSight XR-20, Crossbow, Combat Knife, and Grenade"},
    {497, "Cheat Unlock: Get gold medals for Tranquilizer, Reaper, and Devastator"},
    {498, "Collect All Stars"},
    // {499, "Complete Challenges: Unused First Unlock"},
    {500, "Complete 1 Challenge: FarSight XR-20 Unlock"},
    {501, "Complete 7 Challenges: Tranquilizer Unlock"},
    {502, "Complete 4 Challenges: SuperDragon Unlock"},
    {503, "Complete 13 Challenges: Slayer Unlock"},
    {504, "Complete 3 Challenges: Falcon 2 (Silencer) Unlock"},
    {505, "Complete 8 Challenges: Falcon 2 (Scope) Unlock"},
    {506, "Complete 16 Challenges: Mauler Unlock"},
    {507, "Complete 14 Challenges: Phoenix Unlock"},
    {508, "Complete 20 Challenges: DY357-LX Unlock"},
    {509, "Complete 17 Challenges: Callisto NTG Unlock"},
    {510, "Complete 5 Challenges: Laptop Gun Unlock"},
    // {511, "Complete Challenges: K7 Avenger Unlock"},
    {512, "Complete 19 Challenges: RC-P120 Unlock"},
    {513, "Complete 2 Challenges: Shotgun Unlock"},
    {514, "Complete 9 Challenges: Reaper Unlock"},
    {515, "Complete 11 Challenges: Devastator Unlock"},
    {516, "Complete 18 Challenges: Crossbow Unlock"},
    {517, "Complete 21 Challenges: N-Bomb Unlock"},
    {518, "Complete 12 Challenges: Proximity Mine Unlock"},
    {519, "Complete 6 Challenges: Remote Mine Unlock"},
    // {520, "Complete Challenges: X-Ray Scanner Unlock"},
    // {521, "Complete Challenges: Shield Unlock"},
    {522, "Complete 10 Challenges: Cloaking Device Unlock"},
    {523, "Complete 15 Challenges: Combat Boost Unlock"},
    {524, "Complete 7 Challenges: Hard Bot Difficulty Unlock"},
    {525, "Complete 12 Challenges: Perfect Bot Difficulty Unlock"},
    // {526, "Complete Challenges: Unused 1B Unlock"},
    {527, "Complete 22 Challenges: Dark Bot Difficulty Unlock"},
    {528, "Complete 8 Challenges: Slow Motion Unlock"},
    {529, "Complete 3 Challenges: One-Hit Kills Unlock"},
    // {530, "Complete Challenges: King of the Hill Unlock"},
    {531, "Complete 2 Challenges: Hold the Briefcase Unlock"},
    {532, "Complete 4 Challenges: Capture the Case Unlock"},
    // {533, "Complete Challenges: Unused 22 Unlock"},
    {534, "Complete 17 Challenges: Car Park Unlock"},
    {535, "Complete 1 Challenge: Complex Unlock"},
    {536, "Complete 3 Challenges: Warehouse Unlock"},
    {537, "Complete 5 Challenges: Ravine Unlock"},
    {538, "Complete 6 Challenges: Temple Unlock"},
    {539, "Complete 9 Challenges: G5 Building Unlock"},
    {540, "Complete 11 Challenges: Grid Unlock"},
    {541, "Complete 12 Challenges: Felicity Unlock"},
    {542, "Complete 14 Challenges: Villa Unlock"},
    {543, "Complete 16 Challenges: Sewers Unlock"},
    {544, "Complete 22 Challenges: Ruins Unlock"},
    {545, "Complete 18 Challenges: Base Unlock"},
    // {546, "Complete Challenges: Unused 2F Unlock"},
    {547, "Complete 20 Challenges: Fortress Unlock"},
    // {548, "Complete Challenges: Unused 31 Unlock"},
    {549, "Complete 1 Challenge: dataDyne Female Guard Unlock"},
    {550, "Complete 2 Challenges: Office Suit and Office Casual Unlock"},
    {551, "Complete 4 Challenges: Carrington Villa Outfits Unlock"},
    {552, "Complete 5 Challenges: Trent Unlock"},
    {553, "Complete 5 Challenges: NSA Lackey Unlock"},
    {554, "Complete 6 Challenges: G5 Building Outfits Unlock"},
    {555, "Complete 7 Challenges: Mr. Blonde Unlock"},
    {556, "Complete 9 Challenges: CIA Agent and FBI Agent Unlock"},
    {557, "Complete 10 Challenges: A51 Infiltration Outfits Unlock"},
    {558, "Complete 11 Challenges: Lab Technician Outfits Unlock"},
    {559, "Complete 12 Challenges: Biotechnician Unlock"},
    {560, "Complete 14 Challenges: Elvis and Maian Soldier Unlock"},
    {561, "Complete 17 Challenges: Alaskan Guard Unlock"},
    {562, "Complete 16 Challenges: Air Force One Outfits Unlock"},
    {563, "Complete 7 Challenges: 8 Bots and Dinner Jacket Outfits Unlock"},
    {564, "Complete 18 Challenges: Party Frock, Party (Ripped), Evening Wear, and President Unlock"},
    {565, "Complete 19 Challenges: President's Clone Unlock"},
    {566, "Complete 18 Challenges: Presidential Security Unlock"},
    {567, "Complete 19 Challenges: NSA Bodyguard Unlock"},
    {568, "Complete 24 Challenges: Pelagic II Outfits Unlock"},
    {569, "Complete 8 Challenges: Joanna Trench Unlock"},
    // {570, "Complete Challenges: Unused Jo Snow Unlock"},
    // {571, "Complete Challenges: Unused 48 Unlock"},
    // {572, "Complete Challenges: Unused 49 Unlock"},
    {573, "Complete 17 Challenges: Joanna Arctic Unlock"},
    // {574, "Complete Challenges: Unused 4B Unlock"},
    // {575, "Complete Challenges: Jonathan Unlock"},
    {576, "Complete 12 Challenges: Pop a Cap Unlock"},
    {577, "Complete 6 Challenges: Hacker Central Unlock"},
    // {578, "Complete Challenges: Laser Unlock"},
    {579, "Complete A51 Escape (Agent): UFO Escape"},
    {580, "Complete A51 Escape (Agent): Alternate Escape"},
    {581, "Complete A51 Escape (Special Agent): UFO Escape"},
    {582, "Complete A51 Escape (Special Agent): Alternate Escape"},
    {583, "Complete A51 Escape (Perfect Agent): UFO Escape"},
    {584, "Complete A51 Escape (Perfect Agent): Alternate Escape"},
    {585, "Complete Air Base (Agent): Shuttle Exit"},
    {586, "Complete Air Base (Agent): Ladder Exit"},
    {587, "Complete Air Base (Special Agent): Shuttle Exit"},
    {588, "Complete Air Base (Special Agent): Ladder Exit"},
    {589, "Complete Air Base (Perfect Agent): Shuttle Exit"},
    {590, "Complete Air Base (Perfect Agent): Ladder Exit"},
};

void AP_Init()
{
    AllocConsole();
	SetConsoleTitleA("Perfect Dark - Archipelago Console");

    FILE *fp;
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);

    system("cls");

    printf("Version: %s\n", clientVersion);
    printf("If you are using an incompatible version of the APWorld, then it will not work correctly.\n");
    PrintCommands();

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InputCommand, NULL, 0, NULL);
}

void AP_Close() {
    FreeConsole();
}

VOID InputCommand()
{
    while (true) {
        std::string line;
		std::getline(std::cin, line);
        
        if (line == "/help") {
            PrintCommands();
		}
        else if (line[0] == '!') {
            Message(line);
        }
        else if (line.find("/connect ") == 0) {
            
			std::string param = line.substr(9);
			int spaceIndex = param.find(" ");

			if (spaceIndex == -1) {
                printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
			} else {
                URI = param.substr(0, spaceIndex);
                if (spaceIndex == 0) {
                    printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
                    continue;
                }

                int passwordIndex = param.find("password:");
				std::string password = "";
				if (passwordIndex != -1) {
					password = param.substr(passwordIndex + 9);
				}

				slotName = param.substr(spaceIndex + 1, passwordIndex - spaceIndex - 2);
				
                // std::cout << "URI: " << URI << "\n";
                // std::cout << "slotname: " << slotName << "\n";
                // std::cout << "password: " << password << "\n";
				
				printf("Attempting to connect to room...\n");

                Initialize();
			}
		}
		else if (line.find("/connect") == 0) {
			printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]\n");
		}
        else if (line.find("/reconnect") == 0) {
            URI = GetSavedAddress();
            slotName = GetSavedSlotName();
            password = GetSavedPassword();

            Initialize();
		}
        else if (line.find("/version") == 0) {
            printf("Version: %s\n", clientVersion);
		}
        else if (line.find("/disconnect") == 0) {
            if (ap) {
                ap->reset();
                delete ap;
                ap = nullptr;

                status = "Not connected\n";
                resetAP(true);
                system("cls");
                printf("You are now disconnected\n");
                PrintCommands();
            }
        }
    }
}

bool Initialize() {
    // Generate a uuid
    std::string uuid = ap_get_uuid(UUID_FILE);

    std::string uri = URI;
    if (URI.find("localhost") == 0 && URI.find("://") == std::string::npos) {
        uri = "ws://" + uri;
    }

    if (ap != nullptr) {
        ap->reset();
    }

    ap = new APClient(uuid, "Perfect Dark", uri);

    ap->set_receive_own_locations(true);

    // load DataPackage cache
    FILE* f = fopen(DATAPACKAGE_CACHE, "rb");
    if (f) {
        char* buf = nullptr;
        size_t len = (size_t)0;
        if ((0 == fseek(f, 0, SEEK_END)) 
                && ((len = ftell(f)) > 0) 
                && ((buf = (char*)malloc(len+1))) 
                && (0 == fseek(f, 0, SEEK_SET)) 
                && (len == fread(buf, 1, len, f))) {
            buf[len] = 0;
        }
        free(buf);
        fclose(f);
    }

    ap_sync_queued = false;

    bool error = false;
    bool connected = false;
    bool roomInfo = false;
    bool roomUpdate = false;

    // Set event callbacks
    // Called when the socket gets connected
    ap->set_socket_connected_handler([&connected]() {
        // printf("socket connected\n");
        connected = true;
        status = "Connected\n";
        failedToConnectTotal = 0;
    });

    // Called when connect or a ping failed - no action required, reconnect is automatic
    ap->set_socket_error_handler([&error](const std::string& msg) {
        printf("socket error: %s\n", msg.c_str());
        error = true;
        status = "Not connected\n";
        failedToConnectTotal++;
    });
	
    // Called when the socket gets disconnected - no action required, reconnect is automatic
    ap->set_socket_disconnected_handler([]() {
        printf("socket disconnected\n");
        status = "Disconnected\n";
    });

    // Called as reply to ConnectSlot when successful. argument is slot data.
	ap->set_slot_connected_handler([](const json& data) {
        ap->StatusUpdate(APClient::ClientStatus::PLAYING);
        printf("Connected and ready to go as %s\n", ap->get_player_alias(ap->get_player_number()).c_str());

        SaveLoginInfo();

        if (data.contains("options")) {
            if (data.at("options").contains("goal")) {
                completionGoal = data.at("options").at("goal");
            }

            if (data.at("options").contains("skedar_ruins_requirements")) {
                skedarRequirements = data.at("options").at("skedar_ruins_requirements");
            }

            if (data.at("options").contains("mission_logic")) {
                missionLogic = data.at("options").at("mission_logic");
            }

            if (data.at("options").contains("agent")) {
                hasAgent = data.at("options").at("agent");
            }

            if (data.at("options").contains("required_agent_mission_stars")) {
                missionStars = 0;
                requiredAgentMissionStars = data.at("options").at("required_agent_mission_stars");
            }

            if (data.at("options").contains("special_agent")) {
                hasSpecialAgent = data.at("options").at("special_agent");
            }

            if (data.at("options").contains("required_special_agent_mission_stars")) {
                missionStars = 0;
                requiredSpecialAgentMissionStars = data.at("options").at("required_special_agent_mission_stars");
            }

            if (data.at("options").contains("perfect_agent")) {
                hasPerfectAgent = data.at("options").at("perfect_agent");
            }

            if (data.at("options").contains("required_perfect_agent_mission_stars")) {
                missionStars = 0;
                requiredPerfectAgentMissionStars = data.at("options").at("required_perfect_agent_mission_stars");
            }

            requiredMissionStars = 0;

            if (hasAgent == 1) {
                requiredMissionStars += requiredAgentMissionStars;
            }

            if (hasSpecialAgent == 1) {
                requiredMissionStars += requiredSpecialAgentMissionStars;
            }

            if (hasPerfectAgent == 1) {
                requiredMissionStars += requiredPerfectAgentMissionStars;
            }

            if (data.at("options").contains("weapon_progression")) {
                progressiveWeapon = 0;
                progressivePistol = 0;
                progressiveSMG = 0;
                progressiveRifle = 0;
                progressiveExplosive = 0;
                progressiveOtherWeapon = 0;
                weaponProgressionType = data.at("options").at("weapon_progression");
            }

            if (data.at("options").contains("allow_progressive_weapon_in_challenges")) {
                allowProgWeaponInChallenges = data.at("options").at("allow_progressive_weapon_in_challenges");
            }

            if (data.at("options").contains("master_key")) {
                hasMasterKey = data.at("options").at("master_key");
            }

            if (data.at("options").contains("challenges")) {
                hasChallenges = data.at("options").at("challenges");

                if (completionGoal >= 2
                        || (completionGoal == 0 && skedarRequirements >= 2)) {
                    hasChallenges = 1;
                }
            }

            if (data.at("options").contains("required_challenge_stars")) {
                challengeStars = 0;
                requiredChallengeStars = data.at("options").at("required_challenge_stars");
            }

            if (data.at("options").contains("challenge_logic")) {
                challengeLogic = data.at("options").at("challenge_logic");
            }

            if (data.at("options").contains("shorter_challenges")) {
                shorterChallenges = data.at("options").at("shorter_challenges");
            }

            if (data.at("options").contains("weapon_training")) {
                hasWeaponTraining = data.at("options").at("weapon_training");
            }

            if (data.at("options").contains("device_training")) {
                hasDeviceTraining = data.at("options").at("device_training");
            }

            if (data.at("options").contains("holotraining")) {
                hasHolotraining = data.at("options").at("holotraining");
            }

            if (data.at("options").contains("completion_cheats")) {
                hasCompletionCheats = data.at("options").at("completion_cheats");
            }

            if (data.at("options").contains("timed_cheats")) {
                hasTimedCheats = data.at("options").at("timed_cheats");
            }

            if (data.at("options").contains("weapon_cheats")) {
                hasWeaponCheats = data.at("options").at("weapon_cheats");
            }

            if (data.at("options").contains("include_cheats_in_pool")) {
                areCheatsInItemPool = data.at("options").at("include_cheats_in_pool");
            }

            if (data.at("options").contains("npcs")) {
                hasNPCs = data.at("options").at("npcs");

                if (hasNPCs == 0) {
                    for (int i = 0; i < 6; i++) {
                        unlockedCharacters[i] = 1;
                    }
                }
            }

            if (data.at("options").contains("multiplayer_unlocks")) {
                hasMPUnlocks = data.at("options").at("multiplayer_unlocks");
            }

            if (data.at("options").contains("alternate_exits")) {
                hasAlternateExits = data.at("options").at("alternate_exits");
            }

            if (data.at("options").contains("deathlink")) {
                deathLink = data.at("options").at("deathlink");
            }

            // Set completed locations
            std::set<int64_t> locations = ap->get_checked_locations();
            for (const auto& location : locations) {
                if (location >= 247 && location < 310) {
                    int missionIndex = (location - 247) / 3;
                    int difficulty = (location - 247) % 3;

                    completedMissions[missionIndex][difficulty] = 1;
                }
                else if (location >= 310 && location < 340) {
                    completedChallenges[location - 310] = 1;
                }
                else if (location >= 340 && location < 439) {
                    int weaponIndex = (location - 340) / 3;
                    int difficulty = (location - 340) % 3;

                    completedTrainingMedals[weaponIndex][difficulty] = 1;
                }

                completedLocations[location] = 1;
            }
		}

        if (deathLink == 1) {
            ap->ConnectUpdate(false, 0b111, true, {"AP", "DeathLink"});
        }
	});
	
    // Called as reply to ConnectSlot failed. argument is reason.
    ap->set_slot_refused_handler([](const std::list<std::string>& errors){
        printf("AP: Connection refused:");
        for (const auto& error: errors) {
            printf(" %s", error.c_str());
        }
        printf("\n");
        status = "Not connected\n";
	});
    
    ap->set_slot_disconnected_handler([]() {
		printf("Slot disconnected\n");
        status = "Disconnected\n";
	});

    // Called when the server sent room info. send ConnectSlot from this callback
    ap->set_room_info_handler([&roomInfo]() {
        // printf("Server sent room info\n");
        std::list<std::string> tags;
        tags.push_back("AP");
        if (deathLink == 1) {
            tags.push_back("DeathLink");
        }
        ap->ConnectSlot(slotName, password, 0b111, tags, VERSION_TUPLE);
        roomInfo = true;
    });

    // Sent when there is a need to update information about the present game session
    ap->set_room_update_handler([&roomUpdate]() {
        roomUpdate = true;
    });

    // Called when receiving items - previously received after connect and new over time
	ap->set_items_received_handler([](const std::list<APClient::NetworkItem>& items) {
        int myPlayerNumber = ap->get_player_number();

        // Checks if data package seems to be valid for the server/room.
        if (!ap->is_data_package_valid()) {
            if (!ap_sync_queued) {
                ap->Sync();
            }
            ap_sync_queued = true;
            return;
        }

        // Reset last item received count
        if (lastReceivedItemIndex > items.size() - 1) {
            lastReceivedItemIndex = -1;
        }

        for (const auto& item : items) {
            QueueItem(item);
        }
    });

    // Called as reply to LocationScouts
    ap->set_location_info_handler([](const std::list<APClient::NetworkItem>& items) {
        int playerNum = ap->get_player_number();
        for (auto item: items) {
            if (item.player != playerNum) {
                std::string itemname = ap->get_item_name(item.item, ap->get_player_game(item.player));
                std::string recipient = ap->get_player_alias(item.player);
                std::string location = locationNames[item.location];

                if (completedLocations[item.location] == 0) {
                    printSentItemMessage(itemname.c_str(), recipient.c_str(), location.c_str());
                    completedLocations[item.location] = 1;
                }
            }
        }
    });

    // Called when data package (texts) were updated from the server
    ap->set_data_package_changed_handler([](const json& data) {
        FILE* f = fopen(DATAPACKAGE_CACHE, "wb");
        if (f) {
            std::string s = data.dump();
            fwrite(s.c_str(), 1, s.length(), f);
            fclose(f);
        }
    });

    // legacy chat message
	ap->set_print_handler([](const std::string& msg) {
        printf("%s\n", msg.c_str());
    });

    // colorful chat and server messages. pass arg.data to render_json for text output
	ap->set_print_json_handler([](const std::list<APClient::TextNode>& msg) {
        printf("%s\n", ap->render_json(msg, APClient::RenderFormat::ANSI).c_str());
    });

    // broadcasted when a client sends a Bounce
	ap->set_bounced_handler([](const json& cmd) {
        if (deathLink == 1) {
            auto tagsIt = cmd.find("tags");
			auto dataIt = cmd.find("data");

            if (tagsIt != cmd.end() && tagsIt->is_array()
				&& std::find(tagsIt->begin(), tagsIt->end(), "DeathLink") != tagsIt->end())
			{
				if (dataIt != cmd.end() && dataIt->is_object()) {
					json data = *dataIt;
					if (data["source"].get<std::string>() != slotName) {
						std::string source = data["source"].is_string() ? data["source"].get<std::string>().c_str() : "???";
						std::string cause = data["cause"].is_string() ? data["cause"].get<std::string>().c_str() : "???";
						std::cout << "Killed by " << source << " - " << cause << "\n";
                        
                        printf("Received Death Link\n");
                        pendingDeathLink = true;
					}
				}
				else {
					printf("Bad deathlink packet!\n");
				}
			}
        }
    });

    // Called when a local location was remotely checked or was already checked when connecting
    ap->set_location_checked_handler([](const std::list<int64_t>& locations) {
        
    });

    // called as reply to Get
    ap->set_retrieved_handler([](const json& cmd) {
        
    });

    // called as reply to Set and when value for SetNotify changed
    ap->set_set_reply_handler([](const json& cmd) {
        
    });

    // if (!connected) {
    //     fprintf(stderr, "FAIL: Could not connect socket\n");
    //     return false;
    // }
    // if (!roomInfo) {
    //     fprintf(stderr, "FAIL: Did not receive room info\n");
    //     return false;
    // }
    // if (!roomUpdate) {
    //     fprintf(stderr, "FAIL: Did not receive room update\n");
    //     return false;
    // }
    // if (error) {
    //     fprintf(stderr, "FAIL: Error\n");
    //     return false;
    // }

    return true;
}

void Message(std::string message) {
	if (ap && ap->get_state() == APClient::State::SLOT_CONNECTED) {
		ap->Say(message);
	}
}

int IsConnected() {
	return ap && ap->get_state() == APClient::State::SLOT_CONNECTED;
}

void PollServer() {
    if (ap) {
        ap->poll();
    }
}

void ReachedGoal() {
	if (ap) {
        ap->StatusUpdate(APClient::ClientStatus::GOAL);
    }
}

void InternalCollectAPItem(uint64_t location)
{
    if (!ap) {
        return;
    }
    
    std::list<int64_t> check;
    check.push_back(location);
    ap->LocationScouts(check);
    ap->LocationChecks(check);
}

void QueueItem(APClient::NetworkItem item) {
    std::string itemname = ap->get_item_name(item.item, ap->get_player_game(item.player));
    if (ap->get_player_game(item.player) != "Perfect Dark") {
        itemname = itemNames[item.item - 1];
    }

	std::string sender = ap->get_player_alias(item.player);
	std::string location = ap->get_location_name(item.location, ap->get_player_game(item.player));
    if (ap->get_player_game(item.player) == "Perfect Dark") {
        location = locationNames[item.location];
    }

    handleItem(item.item, itemname.c_str(), sender.c_str(), location.c_str(), item.index);
}

char SendDeathLink()
{
    if (!ap || deathLink == 0) {
        return 0;
    }

    double deathTime = ap->get_server_time();

    json data{
        {"time", deathTime},
        {"cause", "Agent Status: Deceased"},
        {"source", ap->get_slot()}
    };

    ap->Bounce(data, {}, {}, {"DeathLink"});
    return 1;
}

const char *GetServerAddress()
{
    return URI.c_str();
}

const char *GetSlotName()
{
    return slotName.c_str();
}

const char *GetPassword()
{
    return password.c_str();
}

const char *GetStatus()
{
    return status.c_str();
}

void PrintCommands()
{
    printf("--------------------------------------------------------------------------------------------------\n");
    printf("Commands:\n");
    printf(" - '/help' - shows available commands\n");
    if (ap) {
        printf(" - '!help' - Prints the help message related to Archipelago.\n");
    }
	printf(" - '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' - to connect to the room\n");
    printf(" - '/reconnect - to connect to the last connected room\n");
    printf(" - '/version' - to show the version of the client\n");
    printf(" - '/disconnect' - to exit the room\n");
	printf("--------------------------------------------------------------------------------------------------\n");
    if (GetSavedAddress()[0] != '\0' 
            && GetSavedSlotName()[0] != '\0') {
        printf("Last connected room:\n");
        printf(" - Address: %s\n", GetSavedAddress());
        printf(" - Slot name: %s\n", GetSavedSlotName());
        printf(" - Password: %s\n", GetSavedPassword());
        printf("Use /reconnect to rejoin this room.\n");
        printf("--------------------------------------------------------------------------------------------------\n");
    }
}

void DisconnectAP()
{
    if (ap) {
        ap->reset();
        delete ap;
        ap = nullptr;

        status = "Not connected\n";
        resetAP(false);
        printf("--------------------------------------------------------------------------------------------------\n");
        printf("Failed to join room.\n");

        failedToConnectTotal = 0;
    }
}