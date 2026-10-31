#include <ultra64.h>
#include "constants.h"
#include "lib/rng.h"
#include "archipelago.h"

u8 seedElvisHead = HEAD_ELVIS;
u8 seedCarringtonHead = HEAD_CARRINGTON;
u8 seedMrBlondeHead = HEAD_MRBLONDE;
u8 seedTrentHead = HEAD_TRENT;
u8 seedCassandraHead = HEAD_CASSANDRA;
u8 seedJonathanHead = HEAD_JONATHAN;
u8 seedPresidentHead = HEAD_PRESIDENT;

// All weapons an enemy can roll
u8 randomWeapons[] = {
    WEAPON_FALCON2,
    WEAPON_FALCON2_SILENCER,
    WEAPON_FALCON2_SCOPE,
    WEAPON_MAGSEC4,
    WEAPON_MAULER,
    WEAPON_PHOENIX,
    WEAPON_DY357MAGNUM,
    WEAPON_DY357LX,
    WEAPON_CMP150,
    WEAPON_CYCLONE,
    WEAPON_CALLISTO,
    WEAPON_RCP120,
    WEAPON_LAPTOPGUN,
    WEAPON_DRAGON,
    WEAPON_K7AVENGER,
    WEAPON_AR34,
    WEAPON_SUPERDRAGON,
    WEAPON_SHOTGUN,
    WEAPON_REAPER,
    WEAPON_SNIPERRIFLE,
    WEAPON_FARSIGHT,
    WEAPON_DEVASTATOR,
    WEAPON_ROCKETLAUNCHER,
    WEAPON_SLAYER,
    WEAPON_COMBATKNIFE,
    WEAPON_CROSSBOW,
    WEAPON_TRANQUILIZER,
    WEAPON_LASER,
    WEAPON_GRENADE,
    WEAPON_NBOMB,
    WEAPON_TIMEDMINE,
    WEAPON_PROXIMITYMINE,
    WEAPON_REMOTEMINE,
    // WEAPON_COMBATBOOST,
    WEAPON_PP9I,
    WEAPON_CC13,
    WEAPON_KL01313,
    WEAPON_KF7SPECIAL,
    WEAPON_ZZT,
    WEAPON_DMC,
    WEAPON_AR53,
    WEAPON_RCP45,
    WEAPON_PSYCHOSISGUN,
};

u8 originalHeads[] = {
	HEAD_ELVIS,
	HEAD_CARRINGTON,
	HEAD_MRBLONDE,
	HEAD_TRENT,
	HEAD_CASSANDRA,
	HEAD_JONATHAN,
	HEAD_PRESIDENT,
	HEAD_SECRETARY,
	HEAD_MARK2,
	HEAD_CHRIST,
	HEAD_RUSS,
	HEAD_BEAU1,
	HEAD_BRIAN,
};

u8 assignedHeads[13];

u8 availableHeads[] = {
	HEAD_DARK_COMBAT,
	HEAD_ELVIS,
	HEAD_ROSS,
	HEAD_CARRINGTON,
	HEAD_MRBLONDE,
	HEAD_TRENT,
	HEAD_DDSHOCK,
	HEAD_GRAHAM,
	HEAD_DARK_FROCK,
	HEAD_SECRETARY,
	HEAD_CASSANDRA,
	HEAD_THEKING,
	HEAD_FEM_GUARD,
	HEAD_JON,
	HEAD_MARK2,
	HEAD_CHRIST,
	HEAD_RUSS,
	// HEAD_GREY,
	HEAD_DARLING,
	HEAD_ROBERT,
	HEAD_BEAU1,
	HEAD_FEM_GUARD2,
	HEAD_BRIAN,
	HEAD_JAMIE,
	HEAD_DUNCAN2,
	HEAD_BIOTECH,
	HEAD_NEIL2,
	HEAD_EDMCG,
	HEAD_ANKA,
	HEAD_LESLIE_S,
	HEAD_MATT_C,
	HEAD_PEER_S,
	HEAD_EILEEN_T,
	HEAD_ANDY_R,
	HEAD_BEN_R,
	HEAD_STEVE_K,
	HEAD_JONATHAN,
	HEAD_MAIAN_S,
	HEAD_SHAUN,
	// HEAD_BEAU2,
	HEAD_EILEEN_H,
	HEAD_SCOTT_H,
	HEAD_SANCHEZ,
	HEAD_DARKAQUA,
	HEAD_DDSNIPER,
	// HEAD_BEAU3,
	// HEAD_BEAU4,
	// HEAD_BEAU5,
	// HEAD_BEAU6,
	HEAD_GRIFFEY,
	HEAD_MOTO,
	HEAD_KEITH,
	HEAD_WINNER,
	HEAD_A51FACEPLATE,
	HEAD_ELVIS_GOGS,
	HEAD_STEVEM,
	HEAD_DARK_SNOW,
	HEAD_PRESIDENT,
	HEAD_VD,
	HEAD_KEN,
	HEAD_JOEL,
	HEAD_TIM,
	HEAD_GRANT,
	HEAD_PENNY,
	HEAD_ROBIN,
	HEAD_ALEX,
	HEAD_JULIANNE,
	HEAD_LAURA,
	HEAD_DAVEC,
	HEAD_COOK,
	HEAD_PRYCE,
	HEAD_SILKE,
	HEAD_SMITH,
	HEAD_GARETH,
	HEAD_MURCHIE,
	HEAD_WONG,
	HEAD_CARTER,
	HEAD_TINTIN,
	HEAD_MUNTON,
	HEAD_STAMPER,
	HEAD_JONES,
	HEAD_PHELPS,
};

u8 availableBodies[] = {
	BODY_DJBOND,
	BODY_CONNERY,
	BODY_DALTON,
	BODY_MOORE,
	BODY_DARK_COMBAT,
	BODY_ELVIS1,
	BODY_AREA51GUARD,
	BODY_OVERALL,
	BODY_CARRINGTON,
	BODY_MRBLONDE,
	// BODY_SKEDAR,
	BODY_TRENT,
	BODY_DDSHOCK,
	BODY_LABTECH,
	BODY_STRIPES,
	BODY_DARK_FROCK,
	BODY_DARK_TRENCH,
	BODY_OFFICEWORKER,
	BODY_OFFICEWORKER2,
	BODY_SECRETARY,
	BODY_CASSANDRA,
	BODY_THEKING,
	BODY_FEM_GUARD,
	BODY_DD_LABTECH,
	BODY_DD_SECGUARD,
	// BODY_DRCAROLL,
	// BODY_EYESPY,
	BODY_DARK_RIPPED,
	BODY_DD_GUARD,
	BODY_DD_SHOCK_INF,
	// BODY_TESTCHR,
	BODY_BIOTECH,
	BODY_FBIGUY,
	BODY_CIAGUY,
	BODY_A51TROOPER,
	BODY_A51AIRMAN,
	// BODY_CHICROB,
	BODY_STEWARD,
	BODY_STEWARDESS,
	BODY_PRESIDENT,
	BODY_STEWARDESS_COAT,
	// BODY_MINISKEDAR,
	BODY_NSA_LACKEY,
	BODY_PRES_SECURITY,
	BODY_NEGOTIATOR,
	BODY_G5_GUARD,
	BODY_PELAGIC_GUARD,
	BODY_G5_SWAT_GUARD,
	BODY_ALASKAN_GUARD,
	BODY_MAIAN_SOLDIER,
	BODY_PRESIDENT_CLONE,
	BODY_PRESIDENT_CLONE2,
	BODY_DARK_AF1,
	BODY_DARKWET,
	BODY_DARKAQUALUNG,
	BODY_DARKSNOW,
	BODY_DARKLAB,
	BODY_FEMLABTECH,
	BODY_DDSNIPER,
	BODY_PILOTAF1,
	BODY_CILABTECH,
	BODY_CIFEMTECH,
	BODY_CARREVENINGSUIT,
	BODY_JONATHAN,
	BODY_CISOLDIER,
	// BODY_SKEDARKING,
	BODY_ELVISWAISTCOAT,
	BODY_DARK_LEATHER,
	BODY_DARK_NEGOTIATOR,
};

u8 assignedBodies[62];

u8 APGetRandomWeapon(void)
{
    return randomWeapons[rngRandom() % ARRAYCOUNT(randomWeapons)];
}

u8 APGetRandomHead(void)
{
	u32 randomHead = availableHeads[rngRandom() % ARRAYCOUNT(availableHeads)];
	while (randomHead == seedElvisHead
			|| randomHead == seedCarringtonHead
			|| randomHead == seedMrBlondeHead
			|| randomHead == seedTrentHead
			|| randomHead == seedCassandraHead
			|| randomHead == seedJonathanHead
			|| randomHead == seedPresidentHead) {
		randomHead = availableHeads[rngRandom() % ARRAYCOUNT(availableHeads)];
	}

    return randomHead;
}

u8 APGetRandomBody(void)
{
    return availableBodies[rngRandom() % ARRAYCOUNT(availableBodies)];
}

u8 APGetAssignedHead(u8 headnum)
{
	u8 head = HEAD_DARK_COMBAT;

	if (!IsConnected()) {
		return headnum;
	}

	switch (headnum) {
		case HEAD_ELVIS:
		case HEAD_ELVIS_GOGS:
			head = assignedHeads[0];
			break;
		case HEAD_CARRINGTON:
			head = assignedHeads[1];
			break;
		case HEAD_MRBLONDE:
			head = assignedHeads[2];
			break;
		case HEAD_TRENT:
			head = assignedHeads[3];
			break;
		case HEAD_CASSANDRA:
			head = assignedHeads[4];
			break;
		case HEAD_JONATHAN:
			head = assignedHeads[5];
			break;
		case HEAD_PRESIDENT:
			head = assignedHeads[6];
			break;
		case HEAD_SECRETARY:
			head = assignedHeads[7];
			break;
		case HEAD_MARK2:
			head = assignedHeads[8];
			break;
		case HEAD_CHRIST:
			head = assignedHeads[9];
			break;
		case HEAD_RUSS:
			head = assignedHeads[10];
			break;
		case HEAD_BEAU1:
			head = assignedHeads[11];
			break;
		case HEAD_BRIAN:
			head = assignedHeads[12];
			break;
	}
	
	return head;
}

u8 APGetAssignedBody(u8 bodynum)
{
	u8 body = BODY_DJBOND;
	u8 startindex = 0;

	if (!IsConnected()) {
		return bodynum;
	}

	if (bodynum > BODY_MOORE) {
		startindex = 82;
	}
	
	if (bodynum > BODY_SKEDAR) {
		startindex++;
	}

	if (bodynum > BODY_DRCAROLL) {
		startindex++;
	}
	
	if (bodynum > BODY_EYESPY) {
		startindex++;
	}

	if (bodynum > BODY_TESTCHR) {
		startindex++;
	}

	if (bodynum > BODY_CHICROB) {
		startindex++;
	}

	if (bodynum > BODY_MINISKEDAR) {
		startindex++;
	}

	if (bodynum > BODY_SKEDARKING) {
		startindex++;
	}

	body = assignedBodies[bodynum - startindex];

	return body;
}

// Sets all the heads and bodies based on the seed
void setSeedHeadsAndBodies(void)
{
    u32 seed;
    s32 headnum;
    s32 bodynum;
	u8 i;
	u8 index;

    // head
    for (i = 0; i < ARRAYCOUNT(assignedHeads); i++) {
		seed = (u32)GetSeed();
        headnum = originalHeads[i];

        seed ^= seed >> 16;
        seed *= headnum;
        seed ^= seed >> 13;
        seed *= 0xC2B2AE35;
        seed ^= seed >> 16;
        index = seed % ARRAYCOUNT(availableHeads);

        assignedHeads[i] = availableHeads[index];
    }

    // body
    for (i = 0; i < ARRAYCOUNT(assignedBodies); i++) {
		seed = (u32)GetSeed();
        bodynum = availableBodies[i];

        seed ^= seed >> 16;
        seed *= bodynum;
        seed ^= seed >> 13;
        seed *= 0xC2B2AE35;
        seed ^= seed >> 16;
        index = seed % ARRAYCOUNT(availableBodies);

        assignedBodies[i] = availableBodies[index];
    }

	seedElvisHead = assignedHeads[0];
	seedCarringtonHead = assignedHeads[1];
	seedMrBlondeHead = assignedHeads[2];
	seedTrentHead = assignedHeads[3];
	seedCassandraHead = assignedHeads[4];
	seedJonathanHead = assignedHeads[5];
	seedPresidentHead = assignedHeads[6];
}