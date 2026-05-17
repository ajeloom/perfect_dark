#pragma once

#include <PR/ultratypes.h>

void APConfigInit(void);
s32 APConfigLoad(const char *fname);
s32 APConfigSave(const char *fname);
void APConfigRegisterUInt(const char* key, u32* var, u32 min, u32 max);