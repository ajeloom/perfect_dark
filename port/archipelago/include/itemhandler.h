#include <PR/ultratypes.h>

void resetAP();
void collectMissionItem(u8 missionIndex, u8 difficulty);
void collectObjectiveItem(u8 missionIndex, u8 difficulty, u8 objIndex);
void collectChallengeItem(u32 challengeIndex);
void collectFiringRangeItem(s32 weaponIndex, s32 difficulty);
void collectDeviceTrainingItem(s32 deviceIndex);
void collectHolotrainingItem(s32 holoIndex);
void printSentItemMessage(const char* itemname, const char* recipient, const char* location);
void handleItem(int itemID, const char* itemname, const char* sender, const char* location);