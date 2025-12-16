// Sprite animation data for Pokemon front sprites (intro animations)
#include "global.h"
#include "sprite.h"

// General frame 0 used for all Pokemon (default pose)
static const union AnimCmd sAnim_GeneralFrame0[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

// Mudkip animation (from pokeemerald)
static const union AnimCmd sAnim_Mudkip_1[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(1, 13),
    ANIMCMD_FRAME(0, 7),
    ANIMCMD_END,
};

// Animation table for Mudkip (single animation)
static const union AnimCmd *const sAnims_Mudkip[] =
{
    sAnim_GeneralFrame0,
    sAnim_Mudkip_1,
};

// Master animation table for all Pokemon
const union AnimCmd *const *const gMonFrontAnimsPtrTable[NUM_SPECIES] =
{
    [SPECIES_NONE]   = sAnims_Mudkip,  // Placeholder
    [SPECIES_BULBASAUR ... SPECIES_MUDKIP - 1] = sAnims_Mudkip, // Placeholder for other Pokemon
    [SPECIES_MUDKIP] = sAnims_Mudkip,
    [SPECIES_MUDKIP + 1 ... SPECIES_EGG - 1] = sAnims_Mudkip, // Placeholder for other Pokemon
};
