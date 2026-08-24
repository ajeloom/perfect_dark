#include <ultra64.h>
#include "constants.h"
#include "game/cheats.h"
#include "game/bondgun.h"
#include "game/game_0b0fd0.h"
#include "game/inv.h"
#include "game/training.h"
#include "game/lang.h"
#include "bss.h"
#include "lib/main.h"
#include "data.h"
#include "types.h"

extern u32 unlockedWeapons[94];

extern int progressiveWeapon;
extern int weaponProgressionType;
extern int progressiveWeaponNumbers[43];
extern int progWeaponInvPosition[];

extern int progressivePistol;
extern int progressiveSMG;
extern int progressiveRifle;
extern int progressiveExplosive;
extern int progressiveOtherWeapon;
extern int progressivePistolNumbers[11];
extern int progressiveSMGNumbers[10];
extern int progressiveRifleNumbers[7];
extern int progressiveExplosiveNumbers[9];
extern int progressiveOtherWeaponNumbers[10];

void invClear(void)
{
	s32 i;

	for (i = 0; i < g_Vars.currentplayer->equipmaxitems; i++) {
		g_Vars.currentplayer->equipment[i].type = -1;
	}

	g_Vars.currentplayer->weapons = NULL;
	g_Vars.currentplayer->equipcuritem = 0;
}

/**
 * Sorts subject into its correct position in the inventory list.
 *
 * Subject is expected to initially be at the head of the list. It works by
 * swapping the subject with the item to its right as many times as needed.
 */
void invSortItem(struct invitem *subject)
{
	struct invitem *candidate;
	s32 subjweapon1 = -1;
	s32 subjweapon2 = -1;
	s32 candweapon1;
	s32 candweapon2;

	// Prepare subject's properties for comparisons
	if (subject->type == INVITEMTYPE_WEAP) {
		subjweapon1 = subject->type_weap.weapon1;
		if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
			subjweapon1 = progWeaponInvPosition[subjweapon1];
		}
	} else if (subject->type == INVITEMTYPE_DUAL) {
		subjweapon1 = subject->type_dual.weapon1;
		subjweapon2 = subject->type_dual.weapon2;
	} else if (subject->type == INVITEMTYPE_PROP) {
		subjweapon1 = 2000;
	}

	candidate = subject->next;

	while (g_Vars.currentplayer->weapons != subject->next) {
		// Prepare candidate's properties for comparisons
		candweapon1 = -1;
		candweapon2 = -1;

		if (subject->next->type == INVITEMTYPE_WEAP) {
			candweapon1 = subject->next->type_weap.weapon1;
			if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
				candweapon1 = progWeaponInvPosition[candweapon1];
			}
		} else if (subject->next->type == INVITEMTYPE_DUAL) {
			candweapon1 = subject->next->type_dual.weapon1;
			candweapon2 = subject->next->type_dual.weapon2;
		} else if (subject->next->type == INVITEMTYPE_PROP) {
			candweapon1 = 1000;
		}

		// If the candidate should sort ahead of subject
		// then subject is in the desired position.
		if (candweapon1 >= subjweapon1 &&
				(subjweapon1 != candweapon1 || subjweapon2 <= candweapon2)) {
			return;
		}

		// If there's only two items in the list then there's no point swapping
		// them. Just set the list head to the candidate.
		if (candidate->next == subject) {
			g_Vars.currentplayer->weapons = candidate;
		} else {
			// Swap subject with candidate
			subject->next = candidate->next;
			candidate->prev = subject->prev;
			subject->prev = candidate;
			candidate->next = subject;
			subject->next->prev = subject;
			candidate->prev->next = candidate;

			// Set new list head if subject was the head
			if (subject == g_Vars.currentplayer->weapons) {
				g_Vars.currentplayer->weapons = candidate;
			}
		}

		candidate = subject->next;
	}
}

void invInsertItem(struct invitem *item)
{
	if (item->type == INVITEMTYPE_PROP) {
		struct prop *prop = item->type_prop.prop;

		if (prop && prop->obj) {
			struct textoverride *override = invGetTextOverrideForObj(prop->obj);
			bool setflag = true;

			if (override) {
				if (override->weapon >= WEAPON_UNARMED && override->weapon <= WEAPON_NECKLACE) {
					setflag = false;
				}
				if (override->weapon == WEAPON_MPSHIELD) {
					setflag = false;
				}
				if (override->weapon == WEAPON_SUICIDEPILL) {
					setflag = false;
				}
				if (override->weapon == WEAPON_BRIEFCASE2) {
					setflag = false;
				}
			}

			if (setflag && prop->type == PROPTYPE_OBJ) {
				struct defaultobj *obj = prop->obj;
				obj->flags2 |= OBJFLAG2_INVHIDDEN;
			}
		}
	}

	// Place item at head of weapons list
	if (g_Vars.currentplayer->weapons) {
		item->next = g_Vars.currentplayer->weapons;
		item->prev = g_Vars.currentplayer->weapons->prev;
		item->next->prev = item;
		item->prev->next = item;
	} else {
		item->next = item;
		item->prev = item;
	}

	g_Vars.currentplayer->weapons = item;

	invSortItem(item);
	invCalculateCurrentIndex();
}

void invRemoveItem(struct invitem *item)
{
	struct invitem *next = item->next;
	struct invitem *prev = item->prev;

	if (g_Vars.currentplayer->weapons == item) {
		if (item == item->next) {
			g_Vars.currentplayer->weapons = NULL;
		} else {
			g_Vars.currentplayer->weapons = item->next;
		}
	}

	next->prev = prev;
	prev->next = next;
	item->type = -1;

	invCalculateCurrentIndex();
}

struct invitem *invFindUnusedSlot(void)
{
	s32 i;

	for (i = 0; i < g_Vars.currentplayer->equipmaxitems; i++) {
		if (g_Vars.currentplayer->equipment[i].type == -1) {
			return &g_Vars.currentplayer->equipment[i];
		}
	}

	return NULL;
}

void invSetAllGuns(bool enable)
{
	s32 weaponnum;

	g_Vars.currentplayer->equipallguns = enable;
	invCalculateCurrentIndex();
	weaponnum = invGetWeaponNumByIndex(g_Vars.currentplayer->equipcuritem);
	bgunEquipWeapon(weaponnum);
}

bool invHasAllGuns(void)
{
	return g_Vars.currentplayer->equipallguns;
}

struct invitem *invFindSingleWeapon(s32 weaponnum)
{
	struct invitem *first = g_Vars.currentplayer->weapons;
	struct invitem *item = first;

	while (item) {
		if (item->type == INVITEMTYPE_WEAP && item->type_weap.weapon1 == weaponnum) {
			return item;
		}

		item = item->next;

		if (item == first) {
			break;
		}
	}

	return NULL;
}

bool invHasSingleWeaponExcAllGuns(s32 weaponnum)
{
	return invFindSingleWeapon(weaponnum) != NULL;
}

struct invitem *invFindDoubleWeapon(s32 weapon1, s32 weapon2)
{
	struct invitem *first = g_Vars.currentplayer->weapons;
	struct invitem *item = first;

	while (item) {
		if (item->type == INVITEMTYPE_DUAL
				&& item->type_dual.weapon1 == weapon1
				&& item->type_dual.weapon2 == weapon2) {
			return item;
		}

		item = item->next;

		if (item == first) {
			break;
		}
	}

	return NULL;
}

bool invHasDoubleWeaponExcAllGuns(s32 weapon1, s32 weapon2)
{
	return invFindDoubleWeapon(weapon1, weapon2) != NULL;
}

bool invHasSingleWeaponOrProp(s32 weaponnum)
{
	struct invitem *item = g_Vars.currentplayer->weapons;

	while (item) {
		if (item->type == INVITEMTYPE_WEAP) {
			if (weaponnum == item->type_weap.weapon1) {
				return true;
			}
		} else if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;

			if (prop && prop->type == PROPTYPE_WEAPON) {
				struct defaultobj *obj = prop->obj;

				if (obj && obj->type == OBJTYPE_WEAPON) {
					struct weaponobj *weapon = (struct weaponobj *)prop->obj;

					if (weapon->weaponnum == weaponnum) {
						return true;
					}
				}
			}
		}

		item = item->next;

		if (item == g_Vars.currentplayer->weapons) {
			break;
		}
	}

	return false;
}

s32 invAddOneIfCantHaveSlayer(s32 index)
{
	if (mainGetStageNum());

	if (mainGetStageNum() != STAGE_ATTACKSHIP
			&& mainGetStageNum() != STAGE_SKEDARRUINS
			&& index >= WEAPON_SLAYER) {
		index++;
	}

#if (VERSION >= VERSION_JPN_FINAL) && defined(PLATFORM_N64)
	if (index >= 26) {
		index++;
	}
#endif

	return index;
}

s32 currentStageForbidsSlayer(void)
{
	bool value = VERSION >= VERSION_JPN_FINAL ? 1 : 0;

	if (mainGetStageNum() != STAGE_ATTACKSHIP && mainGetStageNum() != STAGE_SKEDARRUINS) {
		value++;
	}

	return value;
}

bool invCanHaveAllGunsWeapon(s32 weaponnum)
{
	bool canhave = true;

#if (VERSION == VERSION_JPN_FINAL) && defined(PLATFORM_N64)
	if (weaponnum == WEAPON_COMBATKNIFE) {
		canhave = false;
	}
#endif

	if (weaponnum == WEAPON_SLAYER) {
		canhave = false;
	}

	// @bug: The stage conditions need an OR. This condition can never pass.
	if ((mainGetStageNum() == STAGE_ATTACKSHIP && mainGetStageNum() == STAGE_SKEDARRUINS)
			&& weaponnum == WEAPON_SLAYER) {
		canhave = true;
	}

	return canhave;
}

bool invHasSingleWeaponIncAllGuns(s32 weaponnum)
{
	if (g_Vars.currentplayer->equipallguns &&
			weaponnum && weaponnum <= WEAPON_PSYCHOSISGUN &&
			invCanHaveAllGunsWeapon(weaponnum)) {
		return true;
	}

	return invHasSingleWeaponExcAllGuns(weaponnum);
}

bool invHasDoubleWeaponIncAllGuns(s32 weapon1, s32 weapon2)
{
	if (weapon2 == WEAPON_NONE) {
		return true;
	}

	if (g_Vars.currentplayer->equipallguns &&
			weapon1 <= WEAPON_PSYCHOSISGUN &&
			weapon1 == weapon2 &&
			weaponHasFlag(weapon1, WEAPONFLAG_DUALWIELD) &&
			invCanHaveAllGunsWeapon(weapon1)) {
		return true;
	}

	return invHasDoubleWeaponExcAllGuns(weapon1, weapon2);
}

bool invGiveSingleWeapon(s32 weaponnum)
{
	frSetWeaponFound(weaponnum);

	if (invHasSingleWeaponExcAllGuns(weaponnum) == 0) {
		struct invitem *item;

		if (g_Vars.currentplayer->equipallguns &&
				weaponnum <= WEAPON_PSYCHOSISGUN &&
				invCanHaveAllGunsWeapon(weaponnum)) {
			return false;
		}

		item = invFindUnusedSlot();

		if (item) {
			item->type = INVITEMTYPE_WEAP;
			item->type_weap.weapon1 = weaponnum;
			item->type_weap.pickuppad = -1;
			invInsertItem(item);
		}

		return true;
	}

	return false;
}

bool invGiveDoubleWeapon(s32 weapon1, s32 weapon2)
{
	if (invHasDoubleWeaponExcAllGuns(weapon1, weapon2) == 0) {
		if (weaponHasFlag(weapon1, WEAPONFLAG_DUALWIELD)) {
			struct invitem *item = invFindUnusedSlot();

			if (item) {
				item->type = INVITEMTYPE_DUAL;
				item->type_dual.weapon1 = weapon1;
				item->type_dual.weapon2 = weapon2;
				invInsertItem(item);
			}

			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}

	return false;
}

void invRemoveItemByNum(s32 weaponnum)
{
	if (g_Vars.currentplayer->weapons) {
		// Begin iterating from the second item in the list. This is required
		// because the item might be removed from the list when iterating it,
		// and it needs to determine when the end of the list has been reached.
		struct invitem *item = g_Vars.currentplayer->weapons->next;

		while (true) {
			// Have to preload this because item->next shouldn't be trusted
			// after calling invRemoveItem()
			struct invitem *next = item->next;

			if (item->type == INVITEMTYPE_PROP) {
				struct prop *prop = item->type_prop.prop;
				struct textoverride *override = invGetTextOverrideForObj(prop->obj);

				if (override && override->weapon == weaponnum) {
					invRemoveItem(item);
				}
			} else if (item->type == INVITEMTYPE_WEAP) {
				if (item->type_weap.weapon1 == weaponnum) {
					invRemoveItem(item);
				}
			} else if (item->type == INVITEMTYPE_DUAL) {
				if (item->type_dual.weapon1 == weaponnum || item->type_dual.weapon2 == weaponnum) {
					invRemoveItem(item);
				}
			}

			if (item == g_Vars.currentplayer->weapons || !g_Vars.currentplayer->weapons) {
				break;
			}

			item = next;
		}
	}
}

bool invGiveProp(struct prop *prop)
{
	struct invitem *item;

	// Don't add duplicate night vision to inventory
	// (night vision is already there when using perfect darkness)
	// Note that this check doesn't work on Investigation because it uses the
	// IR specs model. See bug note in Investigation's setup file (setupear.c).
	if (cheatIsActive(CHEAT_PERFECTDARKNESS)
			&& prop->type == PROPTYPE_OBJ
			&& prop->obj
			&& prop->obj->modelnum == MODEL_CHRNIGHTSIGHT) {
		return true;
	}

	item = invFindUnusedSlot();

	if (item) {
		item->type = INVITEMTYPE_PROP;
		item->type_prop.prop = prop;
		invInsertItem(item);
	}

	return true;
}

void invRemoveProp(struct prop *prop)
{
	if (g_Vars.currentplayer->weapons) {
		struct invitem *item = g_Vars.currentplayer->weapons->next;

		while (true) {
			struct invitem *next = item->next;

			if (item->type == INVITEMTYPE_PROP && item->type_prop.prop == prop) {
				invRemoveItem(item);
			}

			if (item == g_Vars.currentplayer->weapons || !g_Vars.currentplayer->weapons) {
				break;
			}

			item = next;
		}
	}
}

s32 invGiveWeaponsByProp(struct prop *prop)
{
	s32 numgiven = 0;

	if (prop->type == PROPTYPE_WEAPON) {
		struct defaultobj *obj = prop->obj;
		struct weaponobj *weapon;
		struct weaponobj *otherweapon;
		s32 weaponnum;
		s32 otherweaponnum;

		if (obj->type == OBJTYPE_WEAPON) {
			weapon = prop->weapon;
			weaponnum = weapon->weaponnum;
			otherweaponnum;

#ifndef PLATFORM_N64
			// always allow picking up a second gun if dual wield cheat is on
			if (!g_Vars.normmplayerisrunning && cheatIsActive(CHEAT_DUALWIELDALLGUNS)) {
				if (invHasSingleWeaponExcAllGuns(weaponnum) && !invHasDoubleWeaponExcAllGuns(weaponnum, weaponnum)) {
					if (invGiveDoubleWeapon(weaponnum, weaponnum)) {
						return 2;
					}
				}
			}
#endif

			if (cheatIsActive(CHEAT_PERFECTDARKNESS) && weaponnum == WEAPON_NIGHTVISION) {
				return 1;
			}

			if (invGiveSingleWeapon(weaponnum)) {
				numgiven = 1;
			}

			if (g_Vars.normmplayerisrunning
					&& weaponHasFlag(weaponnum, WEAPONFLAG_DUALWIELD)
					&& !invHasDoubleWeaponExcAllGuns(weaponnum, weaponnum)) {
				struct invitem *invitem = invFindSingleWeapon(weaponnum);

				if (invitem) {
					if (invitem->type_weap.pickuppad < 0) {
						if (obj->pad >= 0) {
							invitem->type_weap.pickuppad = obj->pad;
						}
					} else if (obj->pad >= 0 && invitem->type_weap.pickuppad != obj->pad) {
						if (invGiveDoubleWeapon(weaponnum, weaponnum)) {
							numgiven = 2;
						} else {
							numgiven = 0;
						}
					}
				}
			}

			otherweapon = weapon->dualweapon;

			if (otherweapon) {
				if (weapon->base.flags & OBJFLAG_WEAPON_LEFTHANDED) {
					numgiven = invHasDoubleWeaponExcAllGuns(otherweapon->weaponnum, weaponnum) == 0;
				} else {
					numgiven = invHasDoubleWeaponExcAllGuns(weaponnum, otherweapon->weaponnum) == 0;
				}

				weapon->dualweapon->dualweaponnum = weaponnum;
				weapon->dualweapon->dualweapon = NULL;
				weapon->dualweapon = NULL;
			} else if (weapon->dualweaponnum >= 0) {
				if (weapon->base.flags & OBJFLAG_WEAPON_LEFTHANDED) {
					if (invGiveDoubleWeapon(weapon->dualweaponnum, weaponnum)) {
						numgiven = 2;
					} else {
						numgiven = 0;
					}
				} else {
					if (invGiveDoubleWeapon(weaponnum, weapon->dualweaponnum)) {
						numgiven = 2;
					} else {
						numgiven = 0;
					}
				}
			}
		}
	}

	return numgiven;
}

void invChooseCycleForwardWeapon(s32 *ptr1, s32 *ptr2, bool arg2)
{
	s32 weapon1 = *ptr1;
	s32 weapon2 = *ptr2;

	if (g_Vars.currentplayer->equipallguns) {
		s32 candidate = *ptr1;

		if (weaponHasFlag(*ptr1, WEAPONFLAG_DUALWIELD) && *ptr2 != *ptr1) {
			// Switching to dual from single
			weapon1 = *ptr1;
			weapon2 = *ptr1;
		} else {
			// Find next weapon
			do {
				candidate = (candidate + 1) % NUM_CYCLEABLE_WEAPONS;

				if (candidate == WEAPON_NONE) {
					candidate = (candidate + 1) % NUM_CYCLEABLE_WEAPONS;
				}

				if ((!arg2 || bgun0f0a1a10(candidate)) && invCanHaveAllGunsWeapon(candidate)) {
					weapon1 = candidate;
					weapon2 = WEAPON_NONE;
					break;
				}
			} while (candidate != weapon1);
		}
	} else {
		struct invitem *item = g_Vars.currentplayer->weapons;

		while (item) {
			if (item->type == INVITEMTYPE_WEAP) {
				if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
					if (item->type_weap.weapon1 < NUM_CYCLEABLE_WEAPONS 
							&& progWeaponInvPosition[item->type_weap.weapon1] > progWeaponInvPosition[weapon1]) {
						if (!arg2 || bgun0f0a1a10(item->type_weap.weapon1)) {
							weapon1 = item->type_weap.weapon1;
							weapon2 = WEAPON_NONE;
							break;
						}
					}
				}
				else {
					if (item->type_weap.weapon1 < NUM_CYCLEABLE_WEAPONS && item->type_weap.weapon1 > weapon1) {
						if (!arg2 || bgun0f0a1a10(item->type_weap.weapon1)) {
							weapon1 = item->type_weap.weapon1;
							weapon2 = WEAPON_NONE;
							break;
						}
					}
				}
			} else if (item->type == INVITEMTYPE_DUAL) {
				if (item->type_dual.weapon1 > weapon1
						|| (weapon1 == item->type_dual.weapon1 && item->type_dual.weapon2 > weapon2)) {
					if (!arg2 || bgun0f0a1a10(item->type_dual.weapon1) || bgun0f0a1a10(item->type_dual.weapon2)) {
						weapon1 = item->type_dual.weapon1;
						weapon2 = item->type_dual.weapon2;
						break;
					}
				}
			}

			item = item->next;

			if (item == g_Vars.currentplayer->weapons) {
				if (arg2) {
					break;
				}

				weapon1 = -1;
				weapon2 = -1;
			}
		}
	}

	*ptr1 = weapon1;
	*ptr2 = weapon2;
}

void invChooseCycleBackWeapon(s32 *ptr1, s32 *ptr2, bool arg2)
{
	s32 weapon1 = *ptr1;
	s32 weapon2 = *ptr2;

	if (g_Vars.currentplayer->equipallguns) {
		s32 candidate = *ptr1;

		if (weaponHasFlag(weapon1, WEAPONFLAG_DUALWIELD) && weapon1 == weapon2) {
			// Switching from dual to single
			weapon1 = candidate;
			weapon2 = WEAPON_NONE;
		} else {
			// Find prev weapon
			do {
				candidate = (candidate + NUM_CYCLEABLE_WEAPONS - 1) % NUM_CYCLEABLE_WEAPONS;

				if (candidate == WEAPON_NONE) {
					candidate = (candidate + NUM_CYCLEABLE_WEAPONS - 1) % NUM_CYCLEABLE_WEAPONS;
				}
			} while ((arg2 && !bgun0f0a1a10(candidate)) || !invCanHaveAllGunsWeapon(candidate));

			if (weaponHasFlag(candidate, WEAPONFLAG_DUALWIELD)) {
				weapon1 = candidate;
				weapon2 = candidate;
			} else {
				weapon1 = candidate;
				weapon2 = WEAPON_NONE;
			}
		}
	} else if (g_Vars.currentplayer->weapons != NULL) {
		struct invitem *item = g_Vars.currentplayer->weapons->prev;

		while (true) {
			if (item->type == INVITEMTYPE_WEAP) {
				if (weaponProgressionType == WEAPONPROG_ALLGUNS) {
					if (item->type_weap.weapon1 < NUM_CYCLEABLE_WEAPONS
							&& (progWeaponInvPosition[item->type_weap.weapon1] < progWeaponInvPosition[weapon1] 
							|| (progWeaponInvPosition[weapon1] == progWeaponInvPosition[item->type_weap.weapon1] 
							&& weapon2 > 0))) {
						if (!arg2 || bgun0f0a1a10(item->type_weap.weapon1)) {
							weapon1 = item->type_weap.weapon1;
							weapon2 = WEAPON_NONE;
							break;
						}
					}
				}
				else {
					if (item->type_weap.weapon1 < NUM_CYCLEABLE_WEAPONS
							&& (item->type_weap.weapon1 < weapon1 || (weapon1 == item->type_weap.weapon1 && weapon2 > 0))) {
						if (!arg2 || bgun0f0a1a10(item->type_weap.weapon1)) {
							weapon1 = item->type_weap.weapon1;
							weapon2 = WEAPON_NONE;
							break;
						}
					}
				}
			} else if (item->type == INVITEMTYPE_DUAL) {
				if (item->type_dual.weapon1 < weapon1
						|| (weapon1 == item->type_dual.weapon1 && item->type_dual.weapon2 < weapon2)) {
					if (!arg2 || bgun0f0a1a10(item->type_dual.weapon1) || bgun0f0a1a10(item->type_dual.weapon2)) {
						weapon1 = item->type_dual.weapon1;
						weapon2 = item->type_dual.weapon2;
						break;
					}
				}
			}

			if (item == g_Vars.currentplayer->weapons) {
				if (arg2) {
					break;
				}

				weapon1 = 1000;
				weapon2 = 1000;
			}

			item = item->prev;
		}
	}

	*ptr1 = weapon1;
	*ptr2 = weapon2;
}

bool invHasKeyFlags(u32 wantkeyflags)
{
	u32 heldkeyflags = 0;
	struct invitem *item = g_Vars.currentplayer->weapons;

	while (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;

			if (prop && prop->type == PROPTYPE_OBJ) {
				struct defaultobj *obj = prop->obj;

				if (obj && obj->type == OBJTYPE_KEY) {
					struct keyobj *key = (struct keyobj *)prop->obj;

					heldkeyflags |= key->keyflags;

					if ((wantkeyflags & heldkeyflags) == wantkeyflags) {
						return true;
					}
				}
			}
		}

		item = item->next;

		if (item == g_Vars.currentplayer->weapons) {
			break;
		}
	}

	return false;
}

bool func0f11283c(void)
{
	return false;
}

bool invHasBriefcase(void)
{
	if (g_Vars.currentplayer->isdead == false) {
		return invHasSingleWeaponExcAllGuns(WEAPON_BRIEFCASE2);
	}

	return false;
}

bool invHasDataUplink(void)
{
	if (g_Vars.currentplayer->isdead == false) {
		return invHasSingleWeaponExcAllGuns(WEAPON_DATAUPLINK);
	}

	return false;
}

bool func0f1128c4(void)
{
	return false;
}

bool invHasProp(struct prop *prop)
{
	struct invitem *item = g_Vars.currentplayer->weapons;
	struct prop *child;

	while (item) {
		if (item->type == INVITEMTYPE_PROP && item->type_prop.prop == prop) {
			return true;
		}

		item = item->next;

		if (item == g_Vars.currentplayer->weapons) {
			break;
		}
	}

	child = g_Vars.currentplayer->prop->child;

	while (child) {
		if (child == prop) {
			return true;
		}

		child = child->next;
	}

	return false;
}

s32 invGetCount(void)
{
	s32 numitems = 0;
	struct invitem *item;

	if (g_Vars.currentplayer->equipallguns) {
		numitems = WEAPON_PSYCHOSISGUN - currentStageForbidsSlayer();
	}

	item = g_Vars.currentplayer->weapons;

	while (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;

			if (prop) {
				struct defaultobj *obj = prop->obj;

				if (obj) {
					if (prop->type == PROPTYPE_WEAPON) {
						if (obj->hidden & OBJHFLAG_HASTEXTOVERRIDE) {
							numitems++;
						}
					} else if (prop->type == PROPTYPE_OBJ) {
						if ((obj->flags2 & OBJFLAG2_INVHIDDEN) == 0) {
							numitems++;
						}
					}
				}
			}
		} else if (item->type == INVITEMTYPE_WEAP) {
			if (g_Vars.currentplayer->equipallguns == false
					|| item->type_weap.weapon1 > WEAPON_PSYCHOSISGUN) {
				numitems++;
			}
		}

		item = item->next;

		if (item == g_Vars.currentplayer->weapons) {
			break;
		}
	}

	return numitems;
}

struct invitem *invGetItemByIndex(s32 index)
{
	struct invitem *item;

	if (g_Vars.currentplayer->equipallguns) {
		if (index < WEAPON_PSYCHOSISGUN - currentStageForbidsSlayer()) {
			return NULL;
		}

		index += currentStageForbidsSlayer() - WEAPON_PSYCHOSISGUN;
	}

	item = g_Vars.currentplayer->weapons;

	while (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;

			if (prop) {
				struct defaultobj *obj = prop->obj;

				if (obj) {
					if (prop->type == PROPTYPE_WEAPON) {
						if (obj->hidden & OBJHFLAG_HASTEXTOVERRIDE) {
							if (index == 0) {
								return item;
							}
							index--;
						}
					} else if (prop->type == PROPTYPE_OBJ) {
						if ((obj->flags2 & OBJFLAG2_INVHIDDEN) == 0) {
							if (index == 0) {
								return item;
							}
							index--;
						}
					}
				}
			}
		} else if (item->type == INVITEMTYPE_WEAP) {
			if (g_Vars.currentplayer->equipallguns == false
					|| item->type_weap.weapon1 > WEAPON_PSYCHOSISGUN) {
				if (index == 0) {
					return item;
				}
				index--;
			}
		}

		item = item->next;

		if (item == g_Vars.currentplayer->weapons) {
			break;
		}
	}

	return NULL;
}

struct textoverride *invGetTextOverrideForObj(struct defaultobj *obj)
{
	struct textoverride *override = g_Vars.textoverrides;

	while (override) {
		if (override->obj == obj) {
			return override;
		}

		override = override->next;
	}

	return NULL;
}

struct textoverride *invGetTextOverrideForWeapon(s32 weaponnum)
{
	struct textoverride *override = g_Vars.textoverrides;

	while (override) {
		if (override->objoffset == 0 && override->weapon == weaponnum) {
			return override;
		}

		override = override->next;
	}

	return NULL;
}

s32 invGetWeaponNumByIndex(s32 index)
{
	struct invitem *item = invGetItemByIndex(index);

	if (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;
			struct textoverride *override = invGetTextOverrideForObj(prop->obj);

			if (override) {
				return override->weapon;
			}
		} else if (item->type == INVITEMTYPE_WEAP) {
			return item->type_weap.weapon1;
		}
	} else if (g_Vars.currentplayer->equipallguns) {
		if (index < WEAPON_PSYCHOSISGUN - currentStageForbidsSlayer()) {
			index++;
			return invAddOneIfCantHaveSlayer(index);
		}
	}

	return 0;
}

u16 invGetNameIdByIndex(s32 index)
{
	struct invitem *item = invGetItemByIndex(index);
	s32 weaponnum = 0;
	struct textoverride *override;

	if (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;
			override = invGetTextOverrideForObj(prop->obj);

			if (override) {
				if (override->inventorytext) {
					return override->inventorytext;
				}

				weaponnum = override->weapon;
			}
		} else if (item->type == INVITEMTYPE_WEAP) {
			weaponnum = item->type_weap.weapon1;
			override = invGetTextOverrideForWeapon(weaponnum);

			if (override && override->inventorytext) {
				return override->inventorytext;
			}
		}
	} else {
		if (g_Vars.currentplayer->equipallguns) {
			if (index < WEAPON_PSYCHOSISGUN - currentStageForbidsSlayer()) {
				index++;
				return bgunGetNameId(invAddOneIfCantHaveSlayer(index));
			}
		}
	}

	return bgunGetNameId(weaponnum);
}

char *invGetNameByIndex(s32 index)
{
	return langGet(invGetNameIdByIndex(index));
}

char *invGetShortNameByIndex(s32 index)
{
	struct invitem *item = invGetItemByIndex(index);
	s32 weaponnum = 0;
	struct textoverride *override;

	if (item) {
		if (item->type == INVITEMTYPE_PROP) {
			struct prop *prop = item->type_prop.prop;
			override = invGetTextOverrideForObj(prop->obj);

			if (override) {
#if VERSION < VERSION_JPN_FINAL
				if (override->inventorytext) {
					return langGet(override->inventorytext);
				}
#endif

				weaponnum = override->weapon;
			}
		} else if (item->type == INVITEMTYPE_WEAP) {
			weaponnum = item->type_weap.weapon1;
#if VERSION < VERSION_JPN_FINAL
			override = invGetTextOverrideForWeapon(weaponnum);

			if (override && override->inventorytext) {
				return langGet(override->inventorytext);
			}
#endif
		}
	} else if (g_Vars.currentplayer->equipallguns) {
		if (index < WEAPON_PSYCHOSISGUN - currentStageForbidsSlayer()) {
			index++;
			return bgunGetShortName(invAddOneIfCantHaveSlayer(index));
		}
	}

	return bgunGetShortName(weaponnum);
}

void invInsertTextOverride(struct textoverride *override)
{
	override->next = g_Vars.textoverrides;
	g_Vars.textoverrides = override;
}

u32 invGetCurrentIndex(void)
{
	return g_Vars.currentplayer->equipcuritem;
}

void invSetCurrentIndex(u32 item)
{
	g_Vars.currentplayer->equipcuritem = item;
}

void invCalculateCurrentIndex(void)
{
	s32 curweaponnum = bgunGetWeaponNum(HAND_RIGHT);
	s32 i;

	g_Vars.currentplayer->equipcuritem = 0;

	for (i = 0; i < invGetCount(); i++) {
		if (invGetWeaponNumByIndex(i) == curweaponnum) {
			g_Vars.currentplayer->equipcuritem = i;
			break;
		}
	}
}

char *invGetPickupTextByObj(struct defaultobj *obj)
{
	struct textoverride *override = invGetTextOverrideForObj(obj);

	if (override && override->pickuptext) {
		return langGet(override->pickuptext);
	}

	return NULL;
}

char *invGetPickupTextByWeaponNum(s32 weaponnum)
{
	struct textoverride *override = invGetTextOverrideForWeapon(weaponnum);

	if (override && override->pickuptext) {
		return langGet(override->pickuptext);
	}

	return NULL;
}

void invIncrementHeldTime(s32 weapon1, s32 weapon2)
{
	s32 leastusedtime;
	s32 leastusedindex;
	s32 i;

	if (!weaponHasFlag(weapon1, WEAPONFLAG_TRACKTIMEUSED)) {
		return;
	}

	leastusedtime = 0x7fffffff;
	leastusedindex = 0;

	if (!weaponHasFlag(weapon2, WEAPONFLAG_TRACKTIMEUSED)) {
		weapon2 = 0;
	}

	for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->gunheldarr); i++) {
		s32 time = g_Vars.currentplayer->gunheldarr[i].totaltime240_60;

		if (time >= 0) {
			if (weapon1 == g_Vars.currentplayer->gunheldarr[i].weapon1 &&
					weapon2 == g_Vars.currentplayer->gunheldarr[i].weapon2) {
				g_Vars.currentplayer->gunheldarr[i].totaltime240_60 = time + g_Vars.lvupdate60;
				break;
			}

			if (time < leastusedtime) {
				leastusedtime = time;
				leastusedindex = i;
			}
		} else {
			leastusedindex = i;
			i = ARRAYCOUNT(g_Vars.currentplayer->gunheldarr);
			break;
		}
	}

	if (i == ARRAYCOUNT(g_Vars.currentplayer->gunheldarr)) {
		g_Vars.currentplayer->gunheldarr[leastusedindex].totaltime240_60 = g_Vars.lvupdate60;
		g_Vars.currentplayer->gunheldarr[leastusedindex].weapon1 = weapon1;
		g_Vars.currentplayer->gunheldarr[leastusedindex].weapon2 = weapon2;
	}
}

void invGetWeaponOfChoice(s32 *weapon1, s32 *weapon2)
{
	s32 mosttime = -1;
	s32 i;

	*weapon1 = 0;
	*weapon2 = 0;

	for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->gunheldarr); i++) {
		if (g_Vars.currentplayer->gunheldarr[i].totaltime240_60 >= 0
				&& g_Vars.currentplayer->gunheldarr[i].totaltime240_60 > mosttime) {
			mosttime = g_Vars.currentplayer->gunheldarr[i].totaltime240_60;
			*weapon1 = g_Vars.currentplayer->gunheldarr[i].weapon1;
			*weapon2 = g_Vars.currentplayer->gunheldarr[i].weapon2;
		}
	}
}

void invRemoveLockedWeapons(void)
{
	s32 i;

	if (weaponProgressionType == WEAPONPROG_DISABLED
			|| weaponProgressionType == WEAPONPROG_VANILLA_ALLGUNS
			|| weaponProgressionType == WEAPONPROG_ALLGUNS) {
		for (i = WEAPON_FALCON2; i <= WEAPON_SUICIDEPILL; i++) {
			if (unlockedWeapons[i] == 0) {
				invRemoveItemByNum(i);
			}
		}
	}
	else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
		for (i = WEAPON_FALCON2; i <= WEAPON_PSYCHOSISGUN; i++) {
			// Don't clear weapons on some missions
			// Don't clear Laser on certain weapons
			if ((progressiveWeaponNumbers[progressiveWeapon] == WEAPON_DEVASTATOR
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_COMBATKNIFE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_CROSSBOW
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TRANQUILIZER
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN)
					&& i == WEAPON_LASER) {
				continue;
			}

			// Don't clear Laptop Gun on certain weapons
			if ((progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TIMEDMINE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PROXIMITYMINE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_GRENADE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_REMOTEMINE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_NBOMB)
					&& i == WEAPON_LAPTOPGUN) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_G5BUILDING 
					&& i == WEAPON_REMOTEMINE
					&& unlockedWeapons[WEAPON_REMOTEMINE] == 1) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_AIRFORCEONE
					&& i == WEAPON_TIMEDMINE
					&& unlockedWeapons[WEAPON_TIMEDMINE] == 1) {
				continue;
			}

			if ((g_Vars.stagenum == STAGE_AIRFORCEONE 
					|| g_Vars.stagenum == STAGE_DEFENSE) 
					&& i == WEAPON_COMBATBOOST
					&& unlockedWeapons[WEAPON_COMBATBOOST] == 1) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_DEEPSEA 
					&& i == WEAPON_FARSIGHT
					&& unlockedWeapons[WEAPON_FARSIGHT] == 1) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_DEFENSE
					&& i == WEAPON_LASER
					&& unlockedWeapons[WEAPON_LASER] == 1) {
				continue;
			}


			if (i != progressiveWeaponNumbers[progressiveWeapon]) {
				invRemoveItemByNum(i);
			}
		}

		for (i = WEAPON_NIGHTVISION; i <= WEAPON_SUICIDEPILL; i++) {
			if (unlockedWeapons[i] == 0) {
				invRemoveItemByNum(i);
			}
		}
	}
	else if (weaponProgressionType == WEAPONPROG_TYPES) {
		for (i = WEAPON_FALCON2; i <= WEAPON_SUICIDEPILL; i++) {
			if (g_Vars.stagenum == STAGE_G5BUILDING 
					&& i == WEAPON_REMOTEMINE
					&& unlockedWeapons[WEAPON_REMOTEMINE] == 1) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_AIRFORCEONE
					&& i == WEAPON_TIMEDMINE
					&& unlockedWeapons[WEAPON_TIMEDMINE] == 1) {
				continue;
			}

			if (g_Vars.stagenum == STAGE_DEFENSE
					&& i == WEAPON_LASER
					&& unlockedWeapons[WEAPON_LASER] == 1) {
				continue;
			}

			switch (i) {
				case WEAPON_FALCON2:
				case WEAPON_FALCON2_SILENCER:
				case WEAPON_FALCON2_SCOPE:
				case WEAPON_MAGSEC4:
				case WEAPON_MAULER:
				case WEAPON_PHOENIX:
				case WEAPON_DY357MAGNUM:
				case WEAPON_DY357LX:
				case WEAPON_PP9I:
				case WEAPON_CC13:
					if (progressivePistol >= 0 
							&& i != progressivePistolNumbers[progressivePistol]) {
						invRemoveItemByNum(i);
					}
					break;
				case WEAPON_CMP150:
				case WEAPON_CYCLONE:
				case WEAPON_CALLISTO:
				case WEAPON_RCP120:
				case WEAPON_LAPTOPGUN:
				case WEAPON_KL01313:
				case WEAPON_ZZT:
				case WEAPON_DMC:
				case WEAPON_RCP45:
					if (progressiveSMG >= 0 
							&& i != progressiveSMGNumbers[progressiveSMG]) {
						invRemoveItemByNum(i);
					}
					break;
				case WEAPON_DRAGON:
				case WEAPON_K7AVENGER:
				case WEAPON_AR34:
				case WEAPON_SUPERDRAGON:
				case WEAPON_KF7SPECIAL:
				case WEAPON_AR53:
					if (progressiveRifle >= 0 
							&& i != progressiveRifleNumbers[progressiveRifle]) {
						invRemoveItemByNum(i);
					}
					break;
				case WEAPON_DEVASTATOR:
				case WEAPON_ROCKETLAUNCHER:
				case WEAPON_SLAYER:
				case WEAPON_GRENADE:
				case WEAPON_NBOMB:
				case WEAPON_TIMEDMINE:
				case WEAPON_PROXIMITYMINE:
				case WEAPON_REMOTEMINE:
					if (progressiveExplosive >= 0 
							&& i != progressiveExplosiveNumbers[progressiveExplosive]) {
						invRemoveItemByNum(i);
					}
					break;
				case WEAPON_SHOTGUN:
				case WEAPON_REAPER:
				case WEAPON_SNIPERRIFLE:
				case WEAPON_FARSIGHT:
				case WEAPON_COMBATKNIFE:
				case WEAPON_CROSSBOW:
				case WEAPON_TRANQUILIZER:
				case WEAPON_LASER:
				case WEAPON_PSYCHOSISGUN:
					if (progressiveOtherWeapon >= 0 
							&& i != progressiveOtherWeaponNumbers[progressiveOtherWeapon]) {
						invRemoveItemByNum(i);
					}
					break;
				default:
					if (unlockedWeapons[i] == 0) {
						invRemoveItemByNum(i);
					}
					break;
			}
		}

		
	}

	invGetCheatItems();
}

void invGetProgressiveWeapons(void)
{
	s32 i;

	bgunGiveAmmoForProgressiveWeapons();

	if (weaponProgressionType == WEAPONPROG_VANILLA_ALLGUNS) {
		for (i = WEAPON_FALCON2; i <= WEAPON_PSYCHOSISGUN; i++) {
			if (unlockedWeapons[i] == 1) {
				invGiveSingleWeapon(i);
			}
		}
	}
	else if (weaponProgressionType == WEAPONPROG_ALLGUNS && progressiveWeapon > 0) {
		for (i = 1; i <= progressiveWeapon; i++) {
			if (unlockedWeapons[progressiveWeaponNumbers[i]] == 1) {
				invGiveSingleWeapon(progressiveWeaponNumbers[i]);
			}
		}
	}
	else if (weaponProgressionType == WEAPONPROG_ONEGUN) {
		invGiveSingleWeapon(progressiveWeaponNumbers[progressiveWeapon]);

		// Give the Laptop Gun for Mines, Grenade, and N-Bomb
		if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TIMEDMINE
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PROXIMITYMINE
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_GRENADE
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_REMOTEMINE
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_NBOMB) {
			invGiveSingleWeapon(WEAPON_LAPTOPGUN);
		}

		// Need laser to destroy things
		// Extraction - hovercopter
		// Chicago - dumpster
		// Infiltration - robot interceptors
		// Rescue - crate
		// Escape - explosive barrel
		// Air Base - remote mine
		// Crash Site - enemy jamming device
		// Deep Sea - switches
		// Maian SOS - stretcher
		if (g_Vars.stagenum == STAGE_CHICAGO
				|| g_Vars.stagenum == STAGE_INFILTRATION
				|| g_Vars.stagenum == STAGE_RESCUE
				|| g_Vars.stagenum == STAGE_ESCAPE
				|| g_Vars.stagenum == STAGE_AIRBASE
				|| g_Vars.stagenum == STAGE_CRASHSITE
				|| g_Vars.stagenum == STAGE_DEEPSEA
				|| g_Vars.stagenum == STAGE_MAIANSOS) {
			if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_COMBATKNIFE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_CROSSBOW) {
				invGiveSingleWeapon(WEAPON_LASER);
			}
		}

		// Need a weapon to destroy things and eliminate guards
		if (g_Vars.stagenum == STAGE_EXTRACTION) {
			if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_COMBATKNIFE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TRANQUILIZER
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_CROSSBOW
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN) {
				invGiveSingleWeapon(WEAPON_LASER);
			}
		}

		// Need laser to kill snipers
		if (g_Vars.stagenum == STAGE_VILLA) {
			if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_COMBATKNIFE
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TRANQUILIZER
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN) {
				invGiveSingleWeapon(WEAPON_LASER);
			}
		}

		if (g_Vars.stagenum == STAGE_G5BUILDING
				&& unlockedWeapons[WEAPON_REMOTEMINE]) {
			invGiveSingleWeapon(WEAPON_REMOTEMINE);
		}

		// Start with other items from original mission
		if (g_Vars.stagenum == STAGE_AIRFORCEONE
				&& unlockedWeapons[WEAPON_COMBATBOOST] == 1) {
			invGiveSingleWeapon(WEAPON_COMBATBOOST);
		}

		// Need laser for certain objectives
		// Psychosis Gun doesn't work on skedar
		// Attack Ship - shield system & engine systems
		if (g_Vars.stagenum == STAGE_ATTACKSHIP
				&& (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_DEVASTATOR
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_COMBATKNIFE
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_CROSSBOW
				|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN)) {
			invGiveSingleWeapon(WEAPON_LASER);
		}

		if (g_Vars.stagenum == STAGE_DEFENSE) {
			if (unlockedWeapons[WEAPON_LASER] == 1) {
				invGiveSingleWeapon(WEAPON_LASER);
			}

			if (unlockedWeapons[WEAPON_COMBATBOOST] == 1) {
				invGiveSingleWeapon(WEAPON_COMBATBOOST);
			}
		}

		// Need a weapon to blow up the wall
		if (g_Vars.stagenum == STAGE_SKEDARRUINS
				&& progressiveWeapon >= 30 
				&& progressiveWeaponNumbers[progressiveWeapon] != WEAPON_ROCKETLAUNCHER
				&& progressiveWeaponNumbers[progressiveWeapon] != WEAPON_DEVASTATOR
				&& progressiveWeaponNumbers[progressiveWeapon] != WEAPON_SUPERDRAGON
				&& progressiveWeaponNumbers[progressiveWeapon] != WEAPON_PHOENIX) {
			invGiveSingleWeapon(WEAPON_TIMEDMINE);
		}

		// Need extra weapon to sacrifice
		if (g_Vars.stagenum == STAGE_SKEDARRUINS
				&& progressiveWeapon > 1) {
			invGiveSingleWeapon(WEAPON_COMBATKNIFE);
		}

		// Need weapon to kill dataDyne Captain
		if (g_Vars.stagenum == STAGE_MBR) {
			if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_TRANQUILIZER
					|| progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN) {
				invGiveSingleWeapon(WEAPON_LASER);
			}
		}

		// Psychosis Gun doesn't work on skedar
		if (g_Vars.stagenum == STAGE_WAR) {
			if (progressiveWeaponNumbers[progressiveWeapon] == WEAPON_PSYCHOSISGUN) {
				invGiveSingleWeapon(WEAPON_LASER);
			}
		}
	}
	else if (weaponProgressionType == WEAPONPROG_TYPES) {
		if (progressivePistol > 0) {
			invGiveSingleWeapon(progressivePistolNumbers[progressivePistol]);
		}

		if (progressiveSMG > 0) {
			invGiveSingleWeapon(progressiveSMGNumbers[progressiveSMG]);
		}

		if (progressiveRifle > 0) {
			invGiveSingleWeapon(progressiveRifleNumbers[progressiveRifle]);
		}

		if (progressiveExplosive > 0) {
			invGiveSingleWeapon(progressiveExplosiveNumbers[progressiveExplosive]);

			// Need a weapon to blow up the wall
			if (g_Vars.stagenum == STAGE_SKEDARRUINS
					&& progressiveExplosiveNumbers[progressiveExplosive] == WEAPON_NBOMB
					&& progressivePistolNumbers[progressivePistol] != WEAPON_PHOENIX
					&& progressiveRifleNumbers[progressiveRifle] != WEAPON_SUPERDRAGON) {
				invGiveSingleWeapon(WEAPON_TIMEDMINE);
			}
		}

		if (progressiveOtherWeapon > 0) {
			invGiveSingleWeapon(progressiveOtherWeaponNumbers[progressiveOtherWeapon]);
		}
	}
}

void invGetCheatItems(void)
{
	if (cheatIsActive(CHEAT_TRENTSMAGNUM)) {
		invGiveSingleWeapon(WEAPON_DY357LX);
		bgunSetAmmoQuantity(AMMOTYPE_MAGNUM, 80);
	}

	if (cheatIsActive(CHEAT_FARSIGHT)) {
		invGiveSingleWeapon(WEAPON_FARSIGHT);
		bgunSetAmmoQuantity(AMMOTYPE_FARSIGHT, 80);
	}

	if (cheatIsActive(CHEAT_CLOAKINGDEVICE)) {
		invGiveSingleWeapon(WEAPON_CLOAKINGDEVICE);
#if VERSION >= VERSION_PAL_FINAL
		bgunSetAmmoQuantity(AMMOTYPE_CLOAK, TICKS(7200));
#else
		bgunSetAmmoQuantity(AMMOTYPE_CLOAK, 7200);
#endif
	}

	if (cheatIsActive(CHEAT_PERFECTDARKNESS)) {
		invGiveSingleWeapon(WEAPON_NIGHTVISION);
	}

	if (cheatIsActive(CHEAT_RTRACKER)) {
		invGiveSingleWeapon(WEAPON_RTRACKER);
	}

	if (cheatIsActive(CHEAT_ROCKETLAUNCHER)) {
		invGiveSingleWeapon(WEAPON_ROCKETLAUNCHER);
		bgunSetAmmoQuantity(AMMOTYPE_ROCKET, 10);
	}

	if (cheatIsActive(CHEAT_SNIPERRIFLE)) {
		invGiveSingleWeapon(WEAPON_SNIPERRIFLE);
		bgunSetAmmoQuantity(AMMOTYPE_RIFLE, 200);
	}

	if (cheatIsActive(CHEAT_XRAYSCANNER)) {
		invGiveSingleWeapon(WEAPON_XRAYSCANNER);
	}

	if (cheatIsActive(CHEAT_SUPERDRAGON)) {
		invGiveSingleWeapon(WEAPON_SUPERDRAGON);
		bgunSetAmmoQuantity(AMMOTYPE_RIFLE, 200);
		bgunSetAmmoQuantity(AMMOTYPE_DEVASTATOR, 20);
	}

	if (cheatIsActive(CHEAT_LAPTOPGUN)) {
		invGiveSingleWeapon(WEAPON_LAPTOPGUN);
		bgunSetAmmoQuantity(AMMOTYPE_SMG, 200);
	}

	if (cheatIsActive(CHEAT_PHOENIX)) {
		invGiveSingleWeapon(WEAPON_PHOENIX);
		bgunSetAmmoQuantity(AMMOTYPE_PISTOL, 200);
	}

#if VERSION >= VERSION_NTSC_1_0
	if (cheatIsActive(CHEAT_PSYCHOSISGUN) || cheatIsActive(CHEAT_ALLGUNS)) {
		bgunSetAmmoQuantity(AMMOTYPE_PSYCHOSIS, 4);

		if (cheatIsActive(CHEAT_PSYCHOSISGUN)) {
			invGiveSingleWeapon(WEAPON_PSYCHOSISGUN);
		}
	}
#else
	if (cheatIsActive(CHEAT_PSYCHOSISGUN)) {
		invGiveSingleWeapon(WEAPON_PSYCHOSISGUN);
		bgunSetAmmoQuantity(AMMOTYPE_PSYCHOSIS, 4);
	}
#endif

	if (cheatIsActive(CHEAT_PP9I)) {
		invGiveSingleWeapon(WEAPON_PP9I);
		bgunSetAmmoQuantity(AMMOTYPE_PISTOL, 200);
	}

	if (cheatIsActive(CHEAT_CC13)) {
		invGiveSingleWeapon(WEAPON_CC13);
#ifndef PLATFORM_N64 // give the correct ammo for port
		bgunSetAmmoQuantity(AMMOTYPE_PISTOL, 200);
#else
		bgunSetAmmoQuantity(AMMOTYPE_RIFLE, 200);
#endif
	}

	if (cheatIsActive(CHEAT_KL01313)) {
		invGiveSingleWeapon(WEAPON_KL01313);
		bgunSetAmmoQuantity(AMMOTYPE_SMG, 200);
	}

	if (cheatIsActive(CHEAT_KF7SPECIAL)) {
		invGiveSingleWeapon(WEAPON_KF7SPECIAL);
		bgunSetAmmoQuantity(AMMOTYPE_RIFLE, 200);
	}

	if (cheatIsActive(CHEAT_ZZT)) {
		invGiveSingleWeapon(WEAPON_ZZT);
		bgunSetAmmoQuantity(AMMOTYPE_SMG, 200);
	}

	if (cheatIsActive(CHEAT_DMC)) {
		invGiveSingleWeapon(WEAPON_DMC);
		bgunSetAmmoQuantity(AMMOTYPE_SMG, 200);
	}

	if (cheatIsActive(CHEAT_AR53)) {
		invGiveSingleWeapon(WEAPON_AR53);
		bgunSetAmmoQuantity(AMMOTYPE_RIFLE, 200);
	}

	if (cheatIsActive(CHEAT_RCP45)) {
		invGiveSingleWeapon(WEAPON_RCP45);
		bgunSetAmmoQuantity(AMMOTYPE_SMG, 200);
	}
}