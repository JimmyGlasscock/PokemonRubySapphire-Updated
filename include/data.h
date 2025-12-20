#ifndef GUARD_DATA_H
#define GUARD_DATA_H

#include "sprite.h"

// Sprite frame image tables for battles
extern const struct SpriteFrameImage gBattlerPicTable_PlayerLeft[];
extern const struct SpriteFrameImage gBattlerPicTable_OpponentLeft[];
extern const struct SpriteFrameImage gBattlerPicTable_PlayerRight[];
extern const struct SpriteFrameImage gBattlerPicTable_OpponentRight[];

// Trainer sprite tables
extern const struct SpriteFrameImage gTrainerBackPicTable_Brendan[];
extern const struct SpriteFrameImage gTrainerBackPicTable_May[];
extern const struct SpriteFrameImage gTrainerFrontPicTable_Brendan[];
extern const struct SpriteFrameImage gTrainerFrontPicTable_May[];

// Pokemon sprite tables
extern const struct CompressedSpriteSheet gMonFrontPicTable[];
extern const struct CompressedSpriteSheet gMonStillFrontPicTable[];
extern const struct CompressedSpriteSheet gMonBackPicTable[];

// Pokemon palette tables
extern const struct CompressedSpritePalette gMonPaletteTable[];
extern const struct CompressedSpritePalette gMonShinyPaletteTable[];

// Pokemon animation tables
extern const union AnimCmd *const *const gMonFrontAnimsPtrTable[];

// Base stats and evolution data
extern const struct BaseStats gBaseStats[];
extern struct Evolution gEvolutionTable[][5];

// Battler affine animation constants
enum {
    BATTLER_AFFINE_NORMAL,
    BATTLER_AFFINE_EMERGE,
    BATTLER_AFFINE_RETURN,
};

// Battler sprite templates
extern const struct SpriteTemplate gBattlerSpriteTemplates[];

// Battler affine animations
extern const union AffineAnimCmd *const gAffineAnims_BattleSpritePlayerSide[];
extern const union AffineAnimCmd *const gAffineAnims_BattleSpriteOpponentSide[];

#endif // GUARD_DATA_H
