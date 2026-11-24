#include <ultra64.h>
#include "data.h"
#include "types.h"

Gfx *apHudmsgRenderBox(Gfx *gdl, s32 x1, s32 y1, s32 x2, s32 y2, f32 bgopacity, u32 bordercolour, f32 textopacity);
s32 apHudmsgGetWrapWidth(s32 *arg0, s32 arg1);
void apHudmsgsReset(void);
s32 apHudmsgGetNext(s32 refid);
void apHudmsgCreate(char *text, s32 type);
void apHudmsgCalculatePosition(struct hudmessage *msg);
void apHudmsgCreateFromArgs(char *text, s32 type, s32 conf00, s32 conf01, s32 conf02,
		struct fontchar **conf04, struct font **conf08,
		u32 textcolour, u32 shadowcolour,
		u32 alignh, s32 conf16, u32 alignv, s32 conf18, s32 arg14, u32 flags);
void apHudmsgsTick(void);
Gfx *apHudmsgsRender(Gfx *gdl);
