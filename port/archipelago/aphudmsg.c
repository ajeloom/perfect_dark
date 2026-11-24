#include <ultra64.h>
#include "constants.h"
#include "game/propsnd.h"
#include "game/savebuffer.h"
#include "game/menugfx.h"
#include "game/playermgr.h"
#include "game/game_1531a0.h"
#include "game/options.h"
#include "bss.h"
#include "lib/memp.h"
#include "lib/snd.h"
#include "lib/vi.h"
#include "string.h"
#include "aphudmsg.h"

u32 ap_NextHudMessageId;
u8 ap_HudmsgsActive = 0;
s32 ap_NumHudMessages = 0;
struct hudmessage *ap_HudMessages = NULL;
extern struct hudmsgtype g_HudmsgTypes[];

Gfx *apHudmsgRenderBox(Gfx *gdl, s32 x1, s32 y1, s32 x2, s32 y2, f32 bgopacity, u32 bordercolour, f32 textopacity)
{
	f32 f0;
	f32 f20;
	f32 f22;

	ap_HudmsgsActive = true;

	f0 = sinf(90 * bgopacity * M_PI / 180.0f);
	f22 = (x2 - x1) * 0.5f;
	f20 = (y2 - y1) * 0.5f;

	if (f0 < 0.5f) {
		f20 = 0.0f;
		f22 *= f0 + f0;
	} else {
		f20 *= (f0 - 0.5f) + (f0 - 0.5f);
	}

	gdl = func0f0d479c(gdl);

	gdl = menugfxDrawFilledRect(gdl, x1, y1, x2, y1 + 1, bordercolour, bordercolour);
	gdl = menugfxDrawFilledRect(gdl, x1, y2, x2, y2 + 1, bordercolour, bordercolour);
	gdl = menugfxDrawFilledRect(gdl, x1, y1 + 1, x1 + 1, y2, bordercolour, bordercolour);
	gdl = menugfxDrawFilledRect(gdl, x2, y1, x2 + 1, y2 + 1, bordercolour, bordercolour);

	gdl = func0f0d49c8(gdl);

	if (textopacity > 0.0f) {
		f32 width = (x1 + x2) * 0.5f;
		f32 height = (y1 + y2) * 0.5f;

		gdl = text0f153a34(gdl,
				(s32)((width - f22) + 1.0f) * g_ScaleX,
				(height - f20) + 1.0f,
				(s32)(width + f22) * g_ScaleX,
				height + f20,
				128.0f * textopacity);
	}

	return gdl;
}

s32 apHudmsgGetWrapWidth(s32 *arg0, s32 arg1)
{
	s32 viewwidth = g_Vars.currentplayer->viewwidth / g_ScaleX;
	s32 result = 0;

	*arg0 = 24;

	result = result + viewwidth - *arg0 - arg1 - 11;

	if (PLAYERCOUNT() == 1) {
		result -= 16;
	}

	return result;
}

void apHudmsgsReset(void)
{
	s32 i;

	ap_NumHudMessages = 5;
	ap_HudMessages = mempAlloc(ALIGN64(sizeof(struct hudmessage) * ap_NumHudMessages), MEMPOOL_STAGE);

	for (i = 0; i < ap_NumHudMessages; i++) {
		ap_HudMessages[i].state = HUDMSGSTATE_FREE;
	}

	ap_NextHudMessageId = 0;
}

s32 apHudmsgGetNext(s32 refid)
{
	s32 bestid = -1;
	s32 bestindex = -1;
	s32 i;

	// Finding the smallest ID that is greater than refid
	for (i = 0; i < ap_NumHudMessages; i++) {
		if (ap_HudMessages[i].state && ap_HudMessages[i].id > refid) {
			if (bestid < 0 || ap_HudMessages[i].id < bestid) {
				bestindex = i;
				bestid = ap_HudMessages[i].id;
			}
		}
	}

	return bestindex;
}

void apHudmsgCreate(char *text, s32 type)
{
	apHudmsgCreateFromArgs(text, type,
			g_HudmsgTypes[type].unk00,
			g_HudmsgTypes[type].unk01,
			g_HudmsgTypes[type].unk02,
			&g_CharsHandelGothicXs, 
			&g_FontHandelGothicXs,
			g_HudmsgTypes[type].colour,
			g_HudmsgTypes[type].unk10,
			g_HudmsgTypes[type].alignh,
			g_HudmsgTypes[type].unk16,
			g_HudmsgTypes[type].alignv,
			g_HudmsgTypes[type].unk18,
			-1, 0);
}

void apHudmsgCalculatePosition(struct hudmessage *msg)
{
	s32 x;
	s32 y;
	s32 viewleft = g_Vars.players[msg->playernum]->viewleft / g_ScaleX;
	s32 viewtop = g_Vars.players[msg->playernum]->viewtop;
	s32 viewwidth = g_Vars.players[msg->playernum]->viewwidth / g_ScaleX;
	s32 viewheight = g_Vars.players[msg->playernum]->viewheight;
	// s32 v0;

	s32 offset = (msg->alignh == HUDMSGALIGN_XMIDDLE) ? 10 : 0;

	switch (msg->alignh) {
	case HUDMSGALIGN_SCREENLEFT:
		x = msg->xmargin;
		break;
	case HUDMSGALIGN_LEFT:
		// v0 = (g_InCutscene && !g_MainIsEndscreen) ? 24 : msg->xmarginextra;
		// x = viewleft + v0 + msg->xmargin - 13;
		x = 4;
		break;
	case HUDMSGALIGN_RIGHT:
		x = viewleft + viewwidth - msg->width - msg->xmargin - 57;
		break;
	case HUDMSGALIGN_XMIDDLE:
		x = (viewwidth - msg->width) / 2 + viewleft + msg->xmargin;
		break;
	default:
		x = msg->xmargin;
		break;
	}

	switch (msg->alignv) {
	case HUDMSGALIGN_SCREENTOP:
		y = msg->ymargin;
		break;
	case HUDMSGALIGN_TOP:
		y = viewtop + msg->ymargin + 13;
		break;
	case HUDMSGALIGN_BOTTOM:
		y = viewtop + viewheight - msg->height - msg->ymargin - 34;
		if (optionsGetEffectiveScreenSize() != SCREENSIZE_FULL) {
			y += 8;
		}
		break;
	case HUDMSGALIGN_YMIDDLE:
		y = (viewheight - msg->height) / 2 + viewtop + msg->ymargin;
		break;
	case HUDMSGALIGN_BELOWVIEWPORT:
		y = viewtop + viewheight - (msg->height / 2) + 18;
		break;
	default:
		y = msg->ymargin;
		break;
	}

	msg->x = x;
	msg->y = y;
}

void apHudmsgCreateFromArgs(char *text, s32 type, s32 conf00, s32 conf01, s32 conf02,
		struct fontchar **conf04, struct font **conf08,
		u32 textcolour, u32 glowcolour,
		u32 alignh, s32 conf16, u32 alignv, s32 conf18, s32 arg14, u32 flags)
{
	s32 j;
	struct hudmessage *msg;
	s32 hash = 0;
	s32 i;
	s32 index;
	s32 textwidth;
	s32 textheight;
	s32 xmarginaextra;
	s32 wrapwidth;
	char stacktext[400];
	s32 writeindex;

	for (j = 0; text[j] != '\0'; j++) {
		hash = hash + text[j];
	}

	g_ScaleX = g_ViRes == VIRES_HI ? 2 : 1;

	// Find an unused index for the new message
	for (index = 0; index < ap_NumHudMessages; index++) {
		if (ap_HudMessages[index].state == HUDMSGSTATE_FREE) {
			// printf("Found unused index: %d\n", index);
			break;
		}
	}

	// Remove the oldest message and move each one 
	// down to free up space for a new message
	if (index >= ap_NumHudMessages) {
		for (i = 0; i < ap_NumHudMessages - 1; i++) {
			ap_HudMessages[i] = ap_HudMessages[i + 1];
		}

		ap_HudMessages[4].state = HUDMSGSTATE_FREE;
		index = 4;
	}

	if (index >= 0 && index < ap_NumHudMessages) {
		xmarginaextra = 0;
		msg = &ap_HudMessages[index];
		wrapwidth = apHudmsgGetWrapWidth(&xmarginaextra, conf16);
		textMeasure(&textheight, &textwidth, text, *conf04, *conf08, 0);

		// Adjust the text to use multiple lines if too long
		if (textwidth > wrapwidth)
		{
			i = 0;
			writeindex = 0;

			while (i < 400 && text[i] != '\0') {
				if (text[i] != '\n') {
					stacktext[writeindex++] = text[i];
				}

				i++;
			}

			stacktext[writeindex++] = '\n';
			stacktext[writeindex++] = '\0';

			textWrap(wrapwidth, stacktext, msg->text, *conf04, *conf08);
			textMeasure(&textheight, &textwidth, msg->text, *conf04, *conf08, 0);
		} else {
			strncpy(msg->text, text, 399);
			msg->text[399] = '\0';
		}

		msg->flags = flags;
		msg->playernum = 0;
		msg->type = type;
		msg->id = ap_NextHudMessageId++;
		msg->state = HUDMSGSTATE_QUEUED;
		msg->timer = 0;
		msg->boxed = conf00;
		msg->allowfadein = conf01;
		msg->flash = conf02;
		msg->font1 = *conf04;
		msg->font2 = *conf08;
		msg->textcolour = textcolour;
		msg->glowcolour = glowcolour;
		msg->alignh = alignh;
		msg->alignv = alignv;
		msg->width = textwidth;
		msg->height = textheight;
		msg->xmarginextra = xmarginaextra;
		msg->xmargin = conf16;
		msg->ymargin = conf18;
		msg->hash = hash;

		apHudmsgCalculatePosition(msg);

		msg->showduration = TICKS(450);
		msg->channelnum = arg14;
	}

	g_ScaleX = 1;
}

void apHudmsgsTick(void)
{
	s32 k;
	s32 previd;
	bool show;
	struct hudmessage *msg;

	s32 i;
	s32 j;

	s32 index;
	bool hide;

	f32 fadeintime;
	f32 fadeouttime;

	ap_HudmsgsActive = false;

	g_ScaleX = (g_ViRes == VIRES_HI) ? 2 : 1;

	s32 totalMessages;

	s32 viewtop = g_Vars.players[0]->viewtop;
	s32 viewheight = g_Vars.players[0]->viewheight;

	for (k = 0; k < ap_NumHudMessages; k++) {
		totalMessages = 0;
		if (ap_HudMessages[k].state != HUDMSGSTATE_FREE) {
			for (j = k; j < ap_NumHudMessages; j++) {
				if (k != j && ap_HudMessages[j].state != HUDMSGSTATE_FREE) {
					totalMessages++;
				}
			}

			ap_HudMessages[k].y = viewtop + viewheight - ap_HudMessages[k].height - ap_HudMessages[k].ymargin - 34 - (16 * totalMessages);
		}
	}

	previd = -1;
	while (true) {
		index = apHudmsgGetNext(previd);

		if (index < 0) {
			break;
		}

		msg = &ap_HudMessages[index];
		previd = msg->id;

		if (msg->channelnum >= 0) {
			msg->opacity = psGetSubtitleOpacity(msg->channelnum);
		} else {
			msg->opacity = 0xff;
		}

		switch (msg->state) {
		case HUDMSGSTATE_QUEUED:
			if (msg->flags & HUDMSGFLAG_DELAY) {
				msg->timer++;

				if (msg->timer > 3) {
					msg->flags &= ~HUDMSGFLAG_DELAY;
				}
			} else {
				show = true;

				if (show) {
					if (msg->boxed) {
						msg->state = HUDMSGSTATE_CHOOSETRANSITION;
					} else if (msg->allowfadein) {
						msg->state = HUDMSGSTATE_FADINGIN;
					} else {
						msg->state = HUDMSGSTATE_ONSCREEN;
					}

					msg->timer = 0;

					if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
						msg->state = HUDMSGSTATE_ONSCREEN;
					}
				}
			}
			break;
		case HUDMSGSTATE_CHOOSETRANSITION:
			msg->state = HUDMSGSTATE_FADINGIN;
			msg->timer = 0;
			break;
		case HUDMSGSTATE_FADINGIN:
			// Make message appear immediately
			msg->state = HUDMSGSTATE_ONSCREEN;
			msg->timer = 0;
			sndStart(var80095200, SFX_HUDMSG, NULL, -1, -1, -1, -1, -1);
			break;
		case HUDMSGSTATE_ONSCREEN:
			// Update even when lvupdate is paused
			msg->timer++;

			hide = false;

			if (msg->timer >= msg->showduration && msg->showduration != -1) {
				hide = true;
			}

			if (hide) {
				if (msg->boxed) {
					msg->state = HUDMSGSTATE_FADINGOUT;
				} else {
					msg->state = HUDMSGSTATE_FREE;
				}

				msg->timer = 0;
			}
			break;
		case HUDMSGSTATE_FADINGOUT:
			fadeouttime = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 92) / PALUPF(7.0f);

			// Update even when lvupdate is paused
			msg->timer++;

			if (msg->timer >= (s32)fadeouttime) {
				msg->state = HUDMSGSTATE_FREE;
				msg->timer = 0;
			}
			break;
		case HUDMSGSTATE_FREE:
			break;
		}
	}

	g_ScaleX = 1;
}

Gfx *apHudmsgsRender(Gfx *gdl)
{
	struct hudmessage *msg;
	s32 i;
	u32 textcolour;
	u32 glowcolour;
	f32 sin;
	s32 x;
	s32 y;
	s32 timerthing = 255;
	s32 spdc = true;
#ifndef PLATFORM_N64
	const s32 playercount = PLAYERCOUNT();
#endif

	g_ScaleX = g_ViRes == VIRES_HI ? 2 : 1;

	gdl = text0f153628(gdl);

	for (i = 0; i < ap_NumHudMessages; i++) {
		msg = &ap_HudMessages[i];

		if (!msg->opacity) {
			continue;
		}

		if (msg->flash) {
			s32 alpha;
			sin = sinf((msg->timer * M_PI) / 60.0f);

			if (sin < 0.0f) {
				sin = -sin;
			}

			alpha = 192.0f * sin;

			textcolour = (msg->textcolour & 0xffffff00) + alpha;
			glowcolour = msg->glowcolour;
		} else {
			textcolour = msg->textcolour | 0xa0;
			glowcolour = msg->glowcolour;
		}

		if (msg->opacity != 255) {
			u32 textalpha = textcolour & 0xff;
			u32 glowalpha = glowcolour & 0xff;

			textalpha = (msg->opacity * textalpha) / 255;
			glowalpha = (msg->opacity * glowalpha) / 255;

			textcolour = (textcolour & 0xffffff00) + (textalpha & 0xff);
			glowcolour = (glowcolour & 0xffffff00) + (glowalpha & 0xff);
		}

		x = msg->x;
		y = msg->y;

#ifndef PLATFORM_N64
		if (msg->state >= HUDMSGSTATE_FADINGIN) {
			if (msg->alignh == HUDMSGALIGN_SCREENLEFT || msg->alignh == HUDMSGALIGN_LEFT) {
				gSPExtraGeometryModeEXT(gdl++, G_ASPECT_MODE_EXT, g_HudAlignModeL);
			} else if (msg->alignh == HUDMSGALIGN_RIGHT) {
				gSPExtraGeometryModeEXT(gdl++, G_ASPECT_MODE_EXT, g_HudAlignModeR);
			} else {
				gSPExtraGeometryModeEXT(gdl++, G_ASPECT_MODE_EXT, G_ASPECT_CENTER_EXT);
			}
		}
#endif

		switch (msg->state) {
		case HUDMSGSTATE_FREE:
		case HUDMSGSTATE_QUEUED:
			break;
		case HUDMSGSTATE_FADINGIN:
			{
				u32 bordercolour = msg->textcolour | 0x40;
				f32 tmp;
				f32 spc0;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

				spc0 = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 132.0f) / PALUPF(7.0f);

				if (spc0 > 30.0f) {
					spc0 = 30.0f;
				}

				spc0 = msg->timer / spc0;

				if (spc0 > 1.0f) {
					spc0 = 1.0f;
				}

				if (spc0 < 0.0f) {
					spc0 = 0.0f;
				}

				tmp = msg->timer * PALUPF(7.0f);

				textSetDiagonalBlend(x, y, tmp, DIAGMODE_FADEIN);

				if (msg->boxed) {
					gdl = apHudmsgRenderBox(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, spc0);

					if (spc0 > 0) {
						gdl = textRenderProjected(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, viGetWidth(), viGetHeight(), 0, 0);
					}
				} else {
					gdl = text0f153a34(gdl, x, y, x + msg->width, y + msg->height, 0);
					gdl = textRender(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, viGetWidth(), viGetHeight(), 0, 0);
				}

				if (msg->alignv == 6) {
					timerthing = 0;
				}

				textResetBlends();
			}
			break;
		case HUDMSGSTATE_ONSCREEN:
			if (msg->boxed) {
				u32 bordercolour = msg->textcolour | 0x40;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

				gdl = apHudmsgRenderBox(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, 1.0f);
				gdl = textRenderProjected(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, viGetWidth(), viGetHeight(), 0, 0);
			} else {
				gdl = text0f153a34(gdl, x, y, x + msg->width, y + msg->height, 0);
				gdl = textRender(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, viGetWidth(), viGetHeight(), 0, 0);
			}
			if (msg->alignv == 6) {
				timerthing = 0;
			}

			break;
		case HUDMSGSTATE_FADINGOUT:
			{
				u32 bordercolour;
				u32 stack;
				f32 spa8 = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 92.0f) / PALUPF(7.0f);
				f32 tmp;

				bordercolour = msg->textcolour | 0x40;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

				tmp = (spa8 - msg->timer) * PALUPF(7.0f);

				textSetDiagonalBlend(x + msg->width, y + msg->height, tmp, DIAGMODE_FADEOUT);

				if (spa8 > 30.0f) {
					spa8 = 30.0f;
				}

				spa8 = msg->timer / spa8;

				if (spa8 > 1.0f) {
					spa8 = 1.0f;
				}

				if (msg->boxed) {
					gdl = apHudmsgRenderBox(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, 1.0f - spa8);

					if (spa8 < 1.0f) {
						gdl = textRenderProjected(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, viGetWidth(), viGetHeight(), 0, 0);
					}
				} else {
					gdl = text0f153a34(gdl, x, y, x + msg->width, y + msg->height, 0);
					gdl = textRender(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, viGetWidth(), viGetHeight(), 0, 0);
				}

				if (msg->alignv == 6) {
					timerthing = 0;
				}

				textResetBlends();
			}
			break;
		}

#ifndef PLATFORM_N64
		gSPClearExtraGeometryModeEXT(gdl++, G_ASPECT_MODE_EXT);
#endif
	}

	gdl = text0f153780(gdl);

	g_ScaleX = 1;

	return gdl;
}
