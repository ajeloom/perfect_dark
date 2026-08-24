#pragma once

#include <PR/ultratypes.h>

void APConfigInit(void);
s32 APConfigLoad(const char *fname);
s32 APConfigSave(const char *fname);
void APConfigRegisterInt(const char *key, s32 *var, s32 min, s32 max);
void APConfigRegisterUInt(const char* key, u32* var, u32 min, u32 max);
void APConfigRegisterString(const char *key, char *var, u32 maxstr);