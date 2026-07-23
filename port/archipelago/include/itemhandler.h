#include <PR/ultratypes.h>

void resetAP();
void collectMissionItem(u8 missionIndex, u8 difficulty);
void collectObjectiveItem(u8 missionIndex, u8 difficulty, u8 objIndex);
void collectChallengeItem(u32 challengeIndex);
void collectFiringRangeItem(s32 weaponIndex, s32 difficulty);
void collectDeviceTrainingItem(s32 deviceIndex);
void collectHolotrainingItem(s32 holoIndex);
void collectCompleteCheatItem(u8 missionIndex);
void collectTimedCheatItem(u8 missionIndex);
void collectClassicWeaponCheatItem(u8 weaponIndex);
void printSentItemMessage(const char* itemname, const char* recipient, const char* location);
void handleItem(int itemID, const char* itemname, const char* sender, const char* location);
void GetNextProgressiveWeapon(int weaponNumbersArray[], int currentWeaponNumber);