#include "global.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "battle.h"
#include "battle_message.h"
#include "data2.h"
#include "event_data.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "move_tutor_menu.h"
#include "pokemon.h"
#include "pokemon_animation.h"
#include "pokedex.h"
#include "random.h"
#include "overworld.h"
#include "rom_8077ABC.h"
#include "rom_8094928.h"
#include "rtc.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "sprite.h"
#include "string_util.h"
#include "text.h"
#include "trainer.h"
#include "util.h"
#include "ewram.h"
#include "task.h"

extern u8 gPlayerPartyCount;
extern u8 gEnemyPartyCount;
extern struct BattlePokemon gBattleMons[4];
extern u8 gActiveBattler;
extern struct BattleEnigmaBerry gEnigmaBerries[];
extern u16 gSpeciesToHoennPokedexNum[];
extern u16 gSpeciesToNationalPokedexNum[];
extern u16 gHoennToNationalOrder[];
extern u16 gSpeciesIdToCryId[];
extern u8 gBattleTextBuff1[];
extern u8 gBattleTextBuff2[];
extern u8 gDisplayedStringBattle[];
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gPotentialItemEffectBattler;
extern u8 gBankInMenu;
extern struct SpindaSpot gSpindaSpotGraphics[];
extern s8 gNatureStatTable[][5];
extern u16 gTrainerBattleOpponent;
extern u16 gBattleTypeFlags;
extern u32 gHitMarker;
extern u32 gTMHMLearnsets[][2];
extern u8 gBattleMonForms[];
extern const u8 BattleText_Wally[];
extern s8 gPokeblockFlavorCompatibilityTable[];
extern u8 gLastUsedAbility;
extern const u8 BattleText_PreventedSwitch[];
extern u16 gBattlerPartyIndexes[];

extern u8 BattleText_Rose[];
extern u8 BattleText_UnknownString3[];
extern u8 BattleText_MistShroud[];
extern u8 BattleText_GetPumped[];
extern u8 *gUnknown_08400F58[];

bool8 HealStatusConditions(struct Pokemon *mon, u32 unused, u32 healMask, u8 battleId)
{
    u32 status = GetMonData(mon, MON_DATA_STATUS, 0);

    if (status & healMask)
    {
        status &= ~healMask;
        SetMonData(mon, MON_DATA_STATUS, &status);
        if (gMain.inBattle && battleId != 4)
            gBattleMons[battleId].status1 &= ~healMask;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

u8 GetItemEffectParamOffset(u16 itemId, u8 effectByte, u8 effectBit)
{
    const u8 *temp;
    const u8 *itemEffect;
    u8 offset;
    int i;
    u8 j;
    u8 val;

    offset = 6;

    temp = gItemEffectTable[itemId - 13];

    if (!temp && itemId != ITEM_ENIGMA_BERRY)
        return 0;

    if (itemId == ITEM_ENIGMA_BERRY)
    {
        temp = gEnigmaBerries[gActiveBattler].itemEffect;
    }

    itemEffect = temp;

    for (i = 0; i < 6; i++)
    {
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (i == effectByte)
                return 0;
            break;
        case 4:
            val = itemEffect[4];
            if (val & 0x20)
                val &= 0xDF;
            j = 0;
            while (val)
            {
                if (val & 1)
                {
                    switch (j)
                    {
                    case 2:
                        if (val & 0x10)
                            val &= 0xEF;
                    case 0:
                        if (i == effectByte && (val & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 1:
                        if (i == effectByte && (val & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 3:
                        if (i == effectByte && (val & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 7:
                        if (i == effectByte)
                            return 0;
                        break;
                    }
                }
                j++;
                val >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        case 5:
            val = itemEffect[5];
            j = 0;
            while (val)
            {
                if (val & 1)
                {
                    switch (j)
                    {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        if (i == effectByte && (val & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 7:
                        if (i == effectByte)
                            return 0;
                        break;
                    }
                }
                j++;
                val >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        }
    }

    return offset;
}

const u8 gUnknown_082082F8[] = {1, 1, 3, 2, 4, 6};

void sub_803F324(int stat)
{
    gBattlerTarget = gBankInMenu;
    StringCopy(gBattleTextBuff1, gUnknown_08400F58[gUnknown_082082F8[stat]]);
    StringCopy(gBattleTextBuff2, BattleText_Rose);
    BattleStringExpandPlaceholdersToDisplayedString(BattleText_UnknownString3);
}

u8 *sub_803F378(u16 itemId)
{
    int i;
    const u8 *itemEffect;

    if (itemId == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
        {
            itemEffect = gEnigmaBerries[gBankInMenu].itemEffect;
        }
        else
        {
            itemEffect = gSaveBlock1.enigmaBerry.itemEffect;
        }
    }
    else
    {
        itemEffect = gItemEffectTable[itemId - 13];
    }

    gPotentialItemEffectBattler = gBankInMenu;

    for (i = 0; i < 3; i++)
    {
        if (itemEffect[i] & 0xF)
            sub_803F324(i * 2);
        if (itemEffect[i] & 0xF0)
        {
            if (i)
            {
                sub_803F324(i * 2 + 1);
            }
            else
            {
                gBattlerAttacker = gBankInMenu;
                BattleStringExpandPlaceholdersToDisplayedString(BattleText_GetPumped);
            }
        }
    }

    if (itemEffect[3] & 0x80)
    {
        gBattlerAttacker = gBankInMenu;
        BattleStringExpandPlaceholdersToDisplayedString(BattleText_MistShroud);
    }

    return gDisplayedStringBattle;
}

u8 GetNature(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_PERSONALITY, 0) % 25;
}

u8 GetNatureFromPersonality(u32 personality)
{
    return personality % 25;
}

u16 GetEvolutionTargetSpecies(struct Pokemon *mon, u8 type, u16 evolutionItem)
{
    int i;
    u16 targetSpecies = 0;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u16 heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    u8 level;
    u16 friendship;
    u8 beauty = GetMonData(mon, MON_DATA_BEAUTY, 0);
    u16 upperPersonality = personality >> 16;
    u8 holdEffect;

    if (heldItem == ITEM_ENIGMA_BERRY)
        holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
    else
        holdEffect = ItemId_GetHoldEffect(heldItem);

    if (holdEffect == HOLD_EFFECT_PREVENT_EVOLVE && type != 3)
        return 0;

    switch (type)
    {
    case 0:
        level = GetMonData(mon, MON_DATA_LEVEL, 0);
        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, 0);

        for (i = 0; i < 5; i++)
        {
            switch (gEvolutionTable[species][i].method)
            {
            case EVO_FRIENDSHIP:
                if (friendship >= 220)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_FRIENDSHIP_DAY:
                RtcCalcLocalTime();
                if (gLocalTime.hours >= 12 && gLocalTime.hours < 24 && friendship >= 220)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_FRIENDSHIP_NIGHT:
                RtcCalcLocalTime();
                if (gLocalTime.hours >= 0 && gLocalTime.hours < 12 && friendship >= 220)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL:
                if (gEvolutionTable[species][i].param <= level)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_ATK_GT_DEF:
                if (gEvolutionTable[species][i].param <= level)
                    if (GetMonData(mon, MON_DATA_ATK, 0) > GetMonData(mon, MON_DATA_DEF, 0))
                        targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_ATK_EQ_DEF:
                if (gEvolutionTable[species][i].param <= level)
                    if (GetMonData(mon, MON_DATA_ATK, 0) == GetMonData(mon, MON_DATA_DEF, 0))
                        targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_ATK_LT_DEF:
                if (gEvolutionTable[species][i].param <= level)
                    if (GetMonData(mon, MON_DATA_ATK, 0) < GetMonData(mon, MON_DATA_DEF, 0))
                        targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_SILCOON:
                if (gEvolutionTable[species][i].param <= level && (upperPersonality % 10) <= 4)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_CASCOON:
                if (gEvolutionTable[species][i].param <= level && (upperPersonality % 10) > 4)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_LEVEL_NINJASK:
                if (gEvolutionTable[species][i].param <= level)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_BEAUTY:
                if (gEvolutionTable[species][i].param <= beauty)
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            }
        }
        break;
    case 1:
        for (i = 0; i < 5; i++)
        {
            switch (gEvolutionTable[species][i].method)
            {
            case EVO_TRADE:
                targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_TRADE_ITEM:
                if (gEvolutionTable[species][i].param == heldItem)
                {
                    heldItem = 0;
                    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                }
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < 5; i++)
        {
            if (gEvolutionTable[species][i].method == EVO_ITEM
             && gEvolutionTable[species][i].param == evolutionItem)
            {
                targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            }
        }
        break;
    }

    return targetSpecies;
}

u16 HoennPokedexNumToSpecies(u16 hoennNum)
{
    u16 species;

    if (!hoennNum)
        return 0;

    species = 0;

    while (species < POKEMON_SLOTS_NUMBER - 1 && gSpeciesToHoennPokedexNum[species] != hoennNum)
        species++;

    if (species == POKEMON_SLOTS_NUMBER - 1)
        return 0;

    return species + 1;
}

u16 NationalPokedexNumToSpecies(u16 nationalNum)
{
    u16 species;

    if (!nationalNum)
        return 0;

    species = 0;

    while (species < POKEMON_SLOTS_NUMBER - 1 && gSpeciesToNationalPokedexNum[species] != nationalNum)
        species++;

    if (species == POKEMON_SLOTS_NUMBER - 1)
        return 0;

    return species + 1;
}

u16 NationalToHoennOrder(u16 nationalNum)
{
    u16 hoennNum;

    if (!nationalNum)
        return 0;

    hoennNum = 0;

    while (hoennNum < POKEMON_SLOTS_NUMBER - 1 && gHoennToNationalOrder[hoennNum] != nationalNum)
        hoennNum++;

    if (hoennNum == POKEMON_SLOTS_NUMBER - 1)
        return 0;

    return hoennNum + 1;
}

u16 SpeciesToNationalPokedexNum(u16 species)
{
    if (!species)
        return 0;

    return gSpeciesToNationalPokedexNum[species - 1];
}

u16 SpeciesToHoennPokedexNum(u16 species)
{
    if (!species)
        return 0;

    return gSpeciesToHoennPokedexNum[species - 1];
}

u16 HoennToNationalOrder(u16 hoennNum)
{
    if (!hoennNum)
        return 0;

    return gHoennToNationalOrder[hoennNum - 1];
}

u16 SpeciesToCryId(u16 species)
{
    if (species < SPECIES_OLD_UNOWN_B - 1)
        return species;

    if (species <= SPECIES_OLD_UNOWN_Z - 1)
        return SPECIES_UNOWN - 1;

    return gSpeciesIdToCryId[species - ((SPECIES_OLD_UNOWN_Z + 1) - 1)];
}

void unref_sub_803F938(u16 species, u32 personality, u8 *dest)
{
    if (species == SPECIES_SPINDA && dest != gMonSpriteGfx_Sprite_ptr[0] && dest != gMonSpriteGfx_Sprite_ptr[2])
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            int j;
            u8 x = gSpindaSpotGraphics[i].x + ((personality & 0x0F) - 8);
            u8 y = gSpindaSpotGraphics[i].y + (((personality & 0xF0) >> 4) - 8);

            for (j = 0; j < 16; j++)
            {
                int k;
                s32 row = gSpindaSpotGraphics[i].image[j];

                for (k = x; k < x + 16; k++)
                {
                    u8 *val = dest + ((k / 8) * 32) + ((k % 8) / 2) + ((y >> 3) << 8) + ((y & 7) << 2);

                    if (row & 1)
                    {
                        if (k & 1)
                        {
                            if ((u8)((*val & 0xF0) - 0x10) <= 0x20)
                                *val += 0x40;
                        }
                        else
                        {
                            if ((u8)((*val & 0xF) - 0x01) <= 0x02)
                                *val += 0x04;
                        }
                    }

                    row >>= 1;
                }

                y++;
            }

            personality >>= 8;
        }
    }
}

void DrawSpindaSpots(u16 species, u32 personality, u8 *dest, u8 a4)
{
    if (species == SPECIES_SPINDA && a4)
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            int j;
            u8 x = gSpindaSpotGraphics[i].x + ((personality & 0x0F) - 8);
            u8 y = gSpindaSpotGraphics[i].y + (((personality & 0xF0) >> 4) - 8);

            for (j = 0; j < 16; j++)
            {
                int k;
                s32 row = gSpindaSpotGraphics[i].image[j];

                for (k = x; k < x + 16; k++)
                {
                    u8 *val = dest + ((k / 8) * 32) + ((k % 8) / 2) + ((y >> 3) << 8) + ((y & 7) << 2);

                    if (row & 1)
                    {
                        if (k & 1)
                        {
                            if ((u8)((*val & 0xF0) - 0x10) <= 0x20)
                                *val += 0x40;
                        }
                        else
                        {
                            if ((u8)((*val & 0xF) - 0x01) <= 0x02)
                                *val += 0x04;
                        }
                    }

                    row >>= 1;
                }

                y++;
            }

            personality >>= 8;
        }
    }
}

void EvolutionRenameMon(struct Pokemon *mon, u16 oldSpecies, u16 newSpecies)
{
#ifdef BUGFIX_EVO_NAME
    u8 language;
    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    language = GetMonData(mon, MON_DATA_LANGUAGE, &language);
    if (language == GAME_LANGUAGE && !StringCompareWithoutExtCtrlCodes(gSpeciesNames[oldSpecies], gStringVar1))
        SetMonData(mon, MON_DATA_NICKNAME, gSpeciesNames[newSpecies]);
#else
    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    if (!StringCompareWithoutExtCtrlCodes(gSpeciesNames[oldSpecies], gStringVar1))
        SetMonData(mon, MON_DATA_NICKNAME, gSpeciesNames[newSpecies]);
#endif
}

bool8 sub_803FBBC(void)
{
    bool8 retVal = FALSE;
    switch (gLinkPlayers[GetMultiplayerId()].id)
    {
    case 0:
    case 3:
        retVal = FALSE;
        break;
    case 1:
    case 2:
        retVal = TRUE;
        break;
    }
    return retVal;
}

bool8 sub_803FBFC(u8 id)
{
    bool8 retVal = FALSE;
    switch (gLinkPlayers[id].id)
    {
    case 0:
    case 3:
        retVal = FALSE;
        break;
    case 1:
    case 2:
        retVal = TRUE;
        break;
    }
    return retVal;
}

s32 sub_803FC34(u16 a1)
{
    s32 id;
    for (id = 0; id < MAX_LINK_PLAYERS; id++)
        if (gLinkPlayers[id].id == a1)
            break;
    return id;
}

u8 sub_803FC58(u16 trainer)
{
    return gTrainers[trainer].encounterMusic_gender & 0x7F;
}

u16 nature_stat_mod(u8 nature, u16 n, u8 statIndex)
{
    if (statIndex < 1 || statIndex > 5)
    {
        // should just be "return n", but it wouldn't match without this
        u16 retVal = n;
        retVal++;
        retVal--;
        return retVal;
    }

    switch (gNatureStatTable[nature][statIndex - 1])
    {
    case 1:
        return (u16)(n * 110) / 100;
    case -1:
        return (u16)(n * 90) / 100;
    }

    return n;
}

// SPECIES_NONE are ignored in the following two tables, so decrement before accessing these arrays to get the right result

static const u8 sMonFrontAnimIdsTable[NUM_SPECIES - 1] =
{
    [SPECIES_BULBASAUR - 1]   = ANIM_V_JUMPS_H_JUMPS,
    [SPECIES_IVYSAUR - 1]     = ANIM_V_STRETCH,
    [SPECIES_VENUSAUR - 1]    = ANIM_ROTATE_UP_SLAM_DOWN,
    [SPECIES_CHARMANDER - 1]  = ANIM_V_JUMPS_SMALL,
    [SPECIES_CHARMELEON - 1]  = ANIM_BACK_AND_LUNGE,
    [SPECIES_CHARIZARD - 1]   = ANIM_V_SHAKE,
    [SPECIES_SQUIRTLE - 1]    = ANIM_SWING_CONCAVE,
    [SPECIES_WARTORTLE - 1]   = ANIM_SHRINK_GROW,
    [SPECIES_BLASTOISE - 1]   = ANIM_V_SHAKE_TWICE,
    [SPECIES_CATERPIE - 1]    = ANIM_SWING_CONCAVE,
    [SPECIES_METAPOD - 1]     = ANIM_SWING_CONCAVE,
    [SPECIES_BUTTERFREE - 1]  = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_WEEDLE - 1]      = ANIM_H_SLIDE_SLOW,
    [SPECIES_KAKUNA - 1]      = ANIM_GLOW_ORANGE,
    [SPECIES_BEEDRILL - 1]    = ANIM_H_VIBRATE,
    [SPECIES_PIDGEY - 1]      = ANIM_V_SLIDE_SLOW,
    [SPECIES_PIDGEOTTO - 1]   = ANIM_V_STRETCH,
    [SPECIES_PIDGEOT - 1]     = ANIM_FRONT_FLIP,
    [SPECIES_RATTATA - 1]     = ANIM_RAPID_H_HOPS,
    [SPECIES_RATICATE - 1]    = ANIM_FIGURE_8,
    [SPECIES_SPEAROW - 1]     = ANIM_RISING_WOBBLE,
    [SPECIES_FEAROW - 1]      = ANIM_FIGURE_8,
    [SPECIES_EKANS - 1]       = ANIM_H_STRETCH,
    [SPECIES_ARBOK - 1]       = ANIM_V_STRETCH,
    [SPECIES_PIKACHU - 1]     = ANIM_FLASH_YELLOW,
    [SPECIES_RAICHU - 1]      = ANIM_V_STRETCH,
    [SPECIES_SANDSHREW - 1]   = ANIM_SWING_CONCAVE_FAST_SHORT,
    [SPECIES_SANDSLASH - 1]   = ANIM_V_STRETCH,
    [SPECIES_NIDORAN_F - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_NIDORINA - 1]    = ANIM_V_STRETCH,
    [SPECIES_NIDOQUEEN - 1]   = ANIM_H_SHAKE,
    [SPECIES_NIDORAN_M - 1]   = ANIM_GROW_VIBRATE,
    [SPECIES_NIDORINO - 1]    = ANIM_SHRINK_GROW,
    [SPECIES_NIDOKING - 1]    = ANIM_H_SHAKE,
    [SPECIES_CLEFAIRY - 1]    = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_CLEFABLE - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL_SLOW,
    [SPECIES_VULPIX - 1]      = ANIM_V_STRETCH,
    [SPECIES_NINETALES - 1]   = ANIM_V_SHAKE,
    [SPECIES_JIGGLYPUFF - 1]  = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_WIGGLYTUFF - 1]  = ANIM_H_JUMPS,
    [SPECIES_ZUBAT - 1]       = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GOLBAT - 1]      = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_ODDISH - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GLOOM - 1]       = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_VILEPLUME - 1]   = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_PARAS - 1]       = ANIM_H_SLIDE_SLOW,
    [SPECIES_PARASECT - 1]    = ANIM_H_SHAKE,
    [SPECIES_VENONAT - 1]     = ANIM_V_JUMPS_H_JUMPS,
    [SPECIES_VENOMOTH - 1]    = ANIM_ZIGZAG_SLOW,
    [SPECIES_DIGLETT - 1]     = ANIM_V_SHAKE,
    [SPECIES_DUGTRIO - 1]     = ANIM_H_SHAKE_SLOW,
    [SPECIES_MEOWTH - 1]      = ANIM_V_JUMPS_SMALL,
    [SPECIES_PERSIAN - 1]     = ANIM_V_STRETCH,
    [SPECIES_PSYDUCK - 1]     = ANIM_V_JUMPS_H_JUMPS,
    [SPECIES_GOLDUCK - 1]     = ANIM_H_SHAKE_SLOW,
    [SPECIES_MANKEY - 1]      = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_PRIMEAPE - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_GROWLITHE - 1]   = ANIM_BACK_AND_LUNGE,
    [SPECIES_ARCANINE - 1]    = ANIM_H_VIBRATE,
    [SPECIES_POLIWAG - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_POLIWHIRL - 1]   = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_POLIWRATH - 1]   = ANIM_V_SHAKE_TWICE,
    [SPECIES_ABRA - 1]        = ANIM_H_JUMPS,
    [SPECIES_KADABRA - 1]     = ANIM_GROW_VIBRATE,
    [SPECIES_ALAKAZAM - 1]    = ANIM_V_STRETCH,
    [SPECIES_MACHOP - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MACHOKE - 1]     = ANIM_V_SHAKE,
    [SPECIES_MACHAMP - 1]     = ANIM_H_JUMPS,
    [SPECIES_BELLSPROUT - 1]  = ANIM_V_STRETCH,
    [SPECIES_WEEPINBELL - 1]  = ANIM_SWING_CONVEX,
    [SPECIES_VICTREEBEL - 1]  = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_TENTACOOL - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_TENTACRUEL - 1]  = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GEODUDE - 1]     = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_GRAVELER - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_GOLEM - 1]       = ANIM_ROTATE_UP_SLAM_DOWN,
    [SPECIES_PONYTA - 1]      = ANIM_GLOW_ORANGE,
    [SPECIES_RAPIDASH - 1]    = ANIM_CIRCULAR_VIBRATE,
    [SPECIES_SLOWPOKE - 1]    = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_SLOWBRO - 1]     = ANIM_SWING_CONCAVE,
    [SPECIES_MAGNEMITE - 1]   = ANIM_TUMBLING_FRONT_FLIP_TWICE,
    [SPECIES_MAGNETON - 1]    = ANIM_FLASH_YELLOW,
    [SPECIES_FARFETCHD - 1]   = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_DODUO - 1]       = ANIM_H_SHAKE_SLOW,
    [SPECIES_DODRIO - 1]      = ANIM_LUNGE_GROW,
    [SPECIES_SEEL - 1]        = ANIM_SWING_CONCAVE,
    [SPECIES_DEWGONG - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_GRIMER - 1]      = ANIM_H_SLIDE_SLOW,
    [SPECIES_MUK - 1]         = ANIM_DEEP_V_SQUISH_AND_BOUNCE,
    [SPECIES_SHELLDER - 1]    = ANIM_TWIST,
    [SPECIES_CLOYSTER - 1]    = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_GASTLY - 1]      = ANIM_GLOW_BLACK,
    [SPECIES_HAUNTER - 1]     = ANIM_FLICKER_INCREASING,
    [SPECIES_GENGAR - 1]      = ANIM_GROW_IN_STAGES,
    [SPECIES_ONIX - 1]        = ANIM_RAPID_H_HOPS,
    [SPECIES_DROWZEE - 1]     = ANIM_CIRCLE_C_CLOCKWISE_SLOW,
    [SPECIES_HYPNO - 1]       = ANIM_GROW_VIBRATE,
    [SPECIES_KRABBY - 1]      = ANIM_H_SLIDE,
    [SPECIES_KINGLER - 1]     = ANIM_ZIGZAG_SLOW,
    [SPECIES_VOLTORB - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_ELECTRODE - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_EXEGGCUTE - 1]   = ANIM_H_SLIDE_SLOW,
    [SPECIES_EXEGGUTOR - 1]   = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_CUBONE - 1]      = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_MAROWAK - 1]     = ANIM_BOUNCE_ROTATE_TO_SIDES,
    [SPECIES_HITMONLEE - 1]   = ANIM_H_STRETCH,
    [SPECIES_HITMONCHAN - 1]  = ANIM_GROW_VIBRATE,
    [SPECIES_LICKITUNG - 1]   = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_KOFFING - 1]     = ANIM_SHRINK_GROW,
    [SPECIES_WEEZING - 1]     = ANIM_V_SLIDE,
    [SPECIES_RHYHORN - 1]     = ANIM_V_SHAKE,
    [SPECIES_RHYDON - 1]      = ANIM_SHRINK_GROW,
    [SPECIES_CHANSEY - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_TANGELA - 1]     = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_KANGASKHAN - 1]  = ANIM_V_STRETCH,
    [SPECIES_HORSEA - 1]      = ANIM_TWIST,
    [SPECIES_SEADRA - 1]      = ANIM_V_SLIDE,
    [SPECIES_GOLDEEN - 1]     = ANIM_SWING_CONVEX,
    [SPECIES_SEAKING - 1]     = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_STARYU - 1]      = ANIM_TWIST_TWICE,
    [SPECIES_STARMIE - 1]     = ANIM_TWIST,
    [SPECIES_MR_MIME - 1]     = ANIM_H_SLIDE_SLOW,
    [SPECIES_SCYTHER - 1]     = ANIM_H_VIBRATE,
    [SPECIES_JYNX - 1]        = ANIM_V_STRETCH,
    [SPECIES_ELECTABUZZ - 1]  = ANIM_FLASH_YELLOW,
    [SPECIES_MAGMAR - 1]      = ANIM_H_SHAKE,
    [SPECIES_PINSIR - 1]      = ANIM_GROW_VIBRATE,
    [SPECIES_TAUROS - 1]      = ANIM_V_SHAKE_TWICE,
    [SPECIES_MAGIKARP - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES,
    [SPECIES_GYARADOS - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL,
    [SPECIES_LAPRAS - 1]      = ANIM_V_STRETCH,
    [SPECIES_DITTO - 1]       = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_EEVEE - 1]       = ANIM_V_STRETCH,
    [SPECIES_VAPOREON - 1]    = ANIM_V_STRETCH,
    [SPECIES_JOLTEON - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_FLAREON - 1]     = ANIM_V_STRETCH,
    [SPECIES_PORYGON - 1]     = ANIM_V_JUMPS_SMALL,
    [SPECIES_OMANYTE - 1]     = ANIM_V_SLIDE_WOBBLE_SMALL,
    [SPECIES_OMASTAR - 1]     = ANIM_GROW_VIBRATE,
    [SPECIES_KABUTO - 1]      = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_KABUTOPS - 1]    = ANIM_H_SHAKE,
    [SPECIES_AERODACTYL - 1]  = ANIM_V_SLIDE_SLOW,
    [SPECIES_SNORLAX - 1]     = ANIM_SWING_CONCAVE,
    [SPECIES_ARTICUNO - 1]    = ANIM_GROW_VIBRATE,
    [SPECIES_ZAPDOS - 1]      = ANIM_FLASH_YELLOW,
    [SPECIES_MOLTRES - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_DRATINI - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_DRAGONAIR - 1]   = ANIM_V_SHAKE,
    [SPECIES_DRAGONITE - 1]   = ANIM_V_SLIDE_SLOW,
    [SPECIES_MEWTWO - 1]      = ANIM_GROW_VIBRATE,
    [SPECIES_MEW - 1]         = ANIM_SWING_CONVEX,
    [SPECIES_CHIKORITA - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_BAYLEEF - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MEGANIUM - 1]    = ANIM_V_STRETCH,
    [SPECIES_CYNDAQUIL - 1]   = ANIM_V_JUMPS_SMALL,
    [SPECIES_QUILAVA - 1]     = ANIM_V_STRETCH,
    [SPECIES_TYPHLOSION - 1]  = ANIM_V_SHAKE,
    [SPECIES_TOTODILE - 1]    = ANIM_H_JUMPS,
    [SPECIES_CROCONAW - 1]    = ANIM_H_SHAKE,
    [SPECIES_FERALIGATR - 1]  = ANIM_H_SHAKE,
    [SPECIES_SENTRET - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_FURRET - 1]      = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_HOOTHOOT - 1]    = ANIM_V_SLIDE_SLOW,
    [SPECIES_NOCTOWL - 1]     = ANIM_V_STRETCH,
    [SPECIES_LEDYBA - 1]      = ANIM_V_JUMPS_SMALL,
    [SPECIES_LEDIAN - 1]      = ANIM_V_SLIDE_SLOW,
    [SPECIES_SPINARAK - 1]    = ANIM_CIRCLE_C_CLOCKWISE_SLOW,
    [SPECIES_ARIADOS - 1]     = ANIM_H_SHAKE,
    [SPECIES_CROBAT - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_CHINCHOU - 1]    = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_LANTURN - 1]     = ANIM_V_SLIDE_WOBBLE_SMALL,
    [SPECIES_PICHU - 1]       = ANIM_V_JUMPS_BIG,
    [SPECIES_CLEFFA - 1]      = ANIM_V_JUMPS_SMALL,
    [SPECIES_IGGLYBUFF - 1]   = ANIM_SWING_CONCAVE_FAST,
    [SPECIES_TOGEPI - 1]      = ANIM_SWING_CONCAVE,
    [SPECIES_TOGETIC - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_NATU - 1]        = ANIM_H_JUMPS,
    [SPECIES_XATU - 1]        = ANIM_GROW_VIBRATE,
    [SPECIES_MAREEP - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_FLAAFFY - 1]     = ANIM_V_JUMPS_BIG,
    [SPECIES_AMPHAROS - 1]    = ANIM_FLASH_YELLOW,
    [SPECIES_BELLOSSOM - 1]   = ANIM_SWING_CONCAVE,
    [SPECIES_MARILL - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_AZUMARILL - 1]   = ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL_SLOW,
    [SPECIES_SUDOWOODO - 1]   = ANIM_H_SLIDE_SLOW,
    [SPECIES_POLITOED - 1]    = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_HOPPIP - 1]      = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_SKIPLOOM - 1]    = ANIM_RISING_WOBBLE,
    [SPECIES_JUMPLUFF - 1]    = ANIM_V_SLIDE_WOBBLE_SMALL,
    [SPECIES_AIPOM - 1]       = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_SUNKERN - 1]     = ANIM_V_JUMPS_SMALL,
    [SPECIES_SUNFLORA - 1]    = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_YANMA - 1]       = ANIM_FIGURE_8,
    [SPECIES_WOOPER - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_QUAGSIRE - 1]    = ANIM_H_STRETCH,
    [SPECIES_ESPEON - 1]      = ANIM_GROW_VIBRATE,
    [SPECIES_UMBREON - 1]     = ANIM_V_SHAKE,
    [SPECIES_MURKROW - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SLOWKING - 1]    = ANIM_SHRINK_GROW,
    [SPECIES_MISDREAVUS - 1]  = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_UNOWN - 1]       = ANIM_ZIGZAG_FAST,
    [SPECIES_WOBBUFFET - 1]   = ANIM_DEEP_V_SQUISH_AND_BOUNCE,
    [SPECIES_GIRAFARIG - 1]   = ANIM_V_JUMPS_BIG,
    [SPECIES_PINECO - 1]      = ANIM_SWING_CONCAVE,
    [SPECIES_FORRETRESS - 1]  = ANIM_V_SHAKE,
    [SPECIES_DUNSPARCE - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GLIGAR - 1]      = ANIM_SHRINK_GROW,
    [SPECIES_STEELIX - 1]     = ANIM_H_SHAKE,
    [SPECIES_SNUBBULL - 1]    = ANIM_V_STRETCH,
    [SPECIES_GRANBULL - 1]    = ANIM_V_SHAKE,
    [SPECIES_QWILFISH - 1]    = ANIM_GROW_IN_STAGES,
    [SPECIES_SCIZOR - 1]      = ANIM_H_VIBRATE,
    [SPECIES_SHUCKLE - 1]     = ANIM_SWING_CONCAVE,
    [SPECIES_HERACROSS - 1]   = ANIM_LUNGE_GROW,
    [SPECIES_SNEASEL - 1]     = ANIM_H_STRETCH,
    [SPECIES_TEDDIURSA - 1]   = ANIM_V_STRETCH,
    [SPECIES_URSARING - 1]    = ANIM_V_SHAKE,
    [SPECIES_SLUGMA - 1]      = ANIM_V_STRETCH,
    [SPECIES_MAGCARGO - 1]    = ANIM_V_STRETCH,
    [SPECIES_SWINUB - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_PILOSWINE - 1]   = ANIM_H_SHAKE,
    [SPECIES_CORSOLA - 1]     = ANIM_H_SLIDE,
    [SPECIES_REMORAID - 1]    = ANIM_V_JUMPS_SMALL,
    [SPECIES_OCTILLERY - 1]   = ANIM_V_STRETCH,
    [SPECIES_DELIBIRD - 1]    = ANIM_V_JUMPS_SMALL,
    [SPECIES_MANTINE - 1]     = ANIM_SWING_CONVEX,
    [SPECIES_SKARMORY - 1]    = ANIM_V_STRETCH,
    [SPECIES_HOUNDOUR - 1]    = ANIM_V_STRETCH,
    [SPECIES_HOUNDOOM - 1]    = ANIM_V_SHAKE,
    [SPECIES_KINGDRA - 1]     = ANIM_CIRCLE_INTO_BG,
    [SPECIES_PHANPY - 1]      = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_DONPHAN - 1]     = ANIM_V_SHAKE_TWICE,
    [SPECIES_PORYGON2 - 1]    = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_STANTLER - 1]    = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SMEARGLE - 1]    = ANIM_H_JUMPS,
    [SPECIES_TYROGUE - 1]     = ANIM_H_STRETCH,
    [SPECIES_HITMONTOP - 1]   = ANIM_H_VIBRATE,
    [SPECIES_SMOOCHUM - 1]    = ANIM_GROW_VIBRATE,
    [SPECIES_ELEKID - 1]      = ANIM_FLASH_YELLOW,
    [SPECIES_MAGBY - 1]       = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MILTANK - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_BLISSEY - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_RAIKOU - 1]      = ANIM_FLASH_YELLOW,
    [SPECIES_ENTEI - 1]       = ANIM_GROW_VIBRATE,
    [SPECIES_SUICUNE - 1]     = ANIM_V_SHAKE,
    [SPECIES_LARVITAR - 1]    = ANIM_V_JUMPS_SMALL,
    [SPECIES_PUPITAR - 1]     = ANIM_V_SHAKE,
    [SPECIES_TYRANITAR - 1]   = ANIM_H_SHAKE,
    [SPECIES_LUGIA - 1]       = ANIM_GROW_IN_STAGES,
    [SPECIES_HO_OH - 1]       = ANIM_GROW_VIBRATE,
    [SPECIES_CELEBI - 1]      = ANIM_RISING_WOBBLE,
    [SPECIES_TREECKO - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GROVYLE - 1]     = ANIM_V_STRETCH,
    [SPECIES_SCEPTILE - 1]    = ANIM_V_SHAKE,
    [SPECIES_TORCHIC - 1]     = ANIM_H_STRETCH,
    [SPECIES_COMBUSKEN - 1]   = ANIM_V_JUMPS_H_JUMPS,
    [SPECIES_BLAZIKEN - 1]    = ANIM_H_SHAKE,
    [SPECIES_MUDKIP - 1]      = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_MARSHTOMP - 1]   = ANIM_V_SLIDE,
    [SPECIES_SWAMPERT - 1]    = ANIM_V_JUMPS_BIG,
    [SPECIES_POOCHYENA - 1]   = ANIM_V_SHAKE,
    [SPECIES_MIGHTYENA - 1]   = ANIM_V_SHAKE,
    [SPECIES_ZIGZAGOON - 1]   = ANIM_H_SLIDE,
    [SPECIES_LINOONE - 1]     = ANIM_GROW_VIBRATE,
    [SPECIES_WURMPLE - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SILCOON - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_BEAUTIFLY - 1]   = ANIM_V_SLIDE,
    [SPECIES_CASCOON - 1]     = ANIM_V_SLIDE,
    [SPECIES_DUSTOX - 1]      = ANIM_V_JUMPS_H_JUMPS,
    [SPECIES_LOTAD - 1]       = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_LOMBRE - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_LUDICOLO - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_SEEDOT - 1]      = ANIM_BOUNCE_ROTATE_TO_SIDES,
    [SPECIES_NUZLEAF - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SHIFTRY - 1]     = ANIM_H_VIBRATE,
    [SPECIES_NINCADA - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_NINJASK - 1]     = ANIM_H_SLIDE_SLOW,
    [SPECIES_SHEDINJA - 1]    = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_TAILLOW - 1]     = ANIM_V_JUMPS_BIG,
    [SPECIES_SWELLOW - 1]     = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_SHROOMISH - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_BRELOOM - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SPINDA - 1]      = ANIM_H_JUMPS,
    [SPECIES_WINGULL - 1]     = ANIM_H_PIVOT,
    [SPECIES_PELIPPER - 1]    = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_SURSKIT - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MASQUERAIN - 1]  = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_WAILMER - 1]     = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_WAILORD - 1]     = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_SKITTY - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_DELCATTY - 1]    = ANIM_V_STRETCH,
    [SPECIES_KECLEON - 1]     = ANIM_FLICKER_INCREASING,
    [SPECIES_BALTOY - 1]      = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_CLAYDOL - 1]     = ANIM_V_SLIDE_WOBBLE_SMALL,
    [SPECIES_NOSEPASS - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_TORKOAL - 1]     = ANIM_V_STRETCH,
    [SPECIES_SABLEYE - 1]     = ANIM_GLOW_BLACK,
    [SPECIES_BARBOACH - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_WHISCASH - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_LUVDISC - 1]     = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_CORPHISH - 1]    = ANIM_V_SHAKE,
    [SPECIES_CRAWDAUNT - 1]   = ANIM_GROW_VIBRATE,
    [SPECIES_FEEBAS - 1]      = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_MILOTIC - 1]     = ANIM_GLOW_BLUE,
    [SPECIES_CARVANHA - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_SHARPEDO - 1]    = ANIM_H_JUMPS_V_STRETCH_TWICE,
    [SPECIES_TRAPINCH - 1]    = ANIM_V_SHAKE,
    [SPECIES_VIBRAVA - 1]     = ANIM_H_SHAKE,
    [SPECIES_FLYGON - 1]      = ANIM_ZIGZAG_SLOW,
    [SPECIES_MAKUHITA - 1]    = ANIM_SWING_CONCAVE,
    [SPECIES_HARIYAMA - 1]    = ANIM_ROTATE_UP_TO_SIDES,
    [SPECIES_ELECTRIKE - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MANECTRIC - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_NUMEL - 1]       = ANIM_V_SLIDE,
    [SPECIES_CAMERUPT - 1]    = ANIM_V_SHAKE,
    [SPECIES_SPHEAL - 1]      = ANIM_SPIN,
    [SPECIES_SEALEO - 1]      = ANIM_V_STRETCH,
    [SPECIES_WALREIN - 1]     = ANIM_H_SHAKE,
    [SPECIES_CACNEA - 1]      = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_CACTURNE - 1]    = ANIM_V_SLIDE,
    [SPECIES_SNORUNT - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_GLALIE - 1]      = ANIM_ZIGZAG_FAST,
    [SPECIES_LUNATONE - 1]    = ANIM_SWING_CONVEX_FAST,
    [SPECIES_SOLROCK - 1]     = ANIM_ROTATE_TO_SIDES_TWICE,
    [SPECIES_AZURILL - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SPOINK - 1]      = ANIM_H_JUMPS_V_STRETCH_TWICE,
    [SPECIES_GRUMPIG - 1]     = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_PLUSLE - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MINUN - 1]       = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_MAWILE - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_MEDITITE - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES,
    [SPECIES_MEDICHAM - 1]    = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_SWABLU - 1]      = ANIM_V_SLIDE,
    [SPECIES_ALTARIA - 1]     = ANIM_H_STRETCH,
    [SPECIES_WYNAUT - 1]      = ANIM_H_JUMPS_V_STRETCH,
    [SPECIES_DUSKULL - 1]     = ANIM_ZIGZAG_FAST,
    [SPECIES_DUSCLOPS - 1]    = ANIM_H_VIBRATE,
    [SPECIES_ROSELIA - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_SLAKOTH - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_VIGOROTH - 1]    = ANIM_H_JUMPS,
    [SPECIES_SLAKING - 1]     = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_GULPIN - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_SWALOT - 1]      = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_TROPIUS - 1]     = ANIM_V_SHAKE,
    [SPECIES_WHISMUR - 1]     = ANIM_H_SLIDE,
    [SPECIES_LOUDRED - 1]     = ANIM_BOUNCE_ROTATE_TO_SIDES_SLOW,
    [SPECIES_EXPLOUD - 1]     = ANIM_V_SHAKE_TWICE,
    [SPECIES_CLAMPERL - 1]    = ANIM_TWIST,
    [SPECIES_HUNTAIL - 1]     = ANIM_GROW_VIBRATE,
    [SPECIES_GOREBYSS - 1]    = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_ABSOL - 1]       = ANIM_CIRCULAR_VIBRATE,
    [SPECIES_SHUPPET - 1]     = ANIM_V_SLIDE_WOBBLE,
    [SPECIES_BANETTE - 1]     = ANIM_SWING_CONVEX,
    [SPECIES_SEVIPER - 1]     = ANIM_V_STRETCH,
    [SPECIES_ZANGOOSE - 1]    = ANIM_GROW_VIBRATE,
    [SPECIES_RELICANTH - 1]   = ANIM_TIP_MOVE_FORWARD,
    [SPECIES_ARON - 1]        = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_LAIRON - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_AGGRON - 1]      = ANIM_V_SHAKE_TWICE,
    [SPECIES_CASTFORM - 1]    = ANIM_H_SLIDE_WOBBLE,
    [SPECIES_VOLBEAT - 1]     = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_ILLUMISE - 1]    = ANIM_BOUNCE_ROTATE_TO_SIDES,
    [SPECIES_LILEEP - 1]      = ANIM_V_STRETCH,
    [SPECIES_CRADILY - 1]     = ANIM_V_SHAKE_TWICE,
    [SPECIES_ANORITH - 1]     = ANIM_TWIST,
    [SPECIES_ARMALDO - 1]     = ANIM_V_SHAKE,
    [SPECIES_RALTS - 1]       = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
    [SPECIES_KIRLIA - 1]      = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_GARDEVOIR - 1]   = ANIM_V_SQUISH_AND_BOUNCE,
    [SPECIES_BAGON - 1]       = ANIM_V_SHAKE_TWICE,
    [SPECIES_SHELGON - 1]     = ANIM_V_SLIDE,
    [SPECIES_SALAMENCE - 1]   = ANIM_H_SHAKE,
    [SPECIES_BELDUM - 1]      = ANIM_H_SHAKE,
    [SPECIES_METANG - 1]      = ANIM_V_SLIDE,
    [SPECIES_METAGROSS - 1]   = ANIM_V_SHAKE,
    [SPECIES_REGIROCK - 1]    = ANIM_CIRCULAR_STRETCH_TWICE,
    [SPECIES_REGICE - 1]      = ANIM_FOUR_PETAL,
    [SPECIES_REGISTEEL - 1]   = ANIM_GROW_VIBRATE,
    [SPECIES_KYOGRE - 1]      = ANIM_SWING_CONCAVE_FAST_SHORT,
    [SPECIES_GROUDON - 1]     = ANIM_V_SHAKE,
    [SPECIES_RAYQUAZA - 1]    = ANIM_H_SHAKE,
    [SPECIES_LATIAS - 1]      = ANIM_SWING_CONCAVE_FAST_SHORT,
    [SPECIES_LATIOS - 1]      = ANIM_V_SHAKE,
    [SPECIES_JIRACHI - 1]     = ANIM_SWING_CONVEX,
    [SPECIES_DEOXYS - 1]      = ANIM_H_PIVOT,
    [SPECIES_CHIMECHO - 1]    = ANIM_H_SLIDE_WOBBLE,
};

static const u8 sMonAnimationDelayTable[NUM_SPECIES - 1] =
{
    [SPECIES_BLASTOISE - 1]  = 50,
    [SPECIES_WEEDLE - 1]     = 10,
    [SPECIES_KAKUNA - 1]     = 20,
    [SPECIES_BEEDRILL - 1]   = 35,
    [SPECIES_PIDGEOTTO - 1]  = 25,
    [SPECIES_FEAROW - 1]     = 2,
    [SPECIES_EKANS - 1]      = 30,
    [SPECIES_NIDORAN_F - 1]  = 28,
    [SPECIES_NIDOKING - 1]   = 25,
    [SPECIES_PARAS - 1]      = 10,
    [SPECIES_PARASECT - 1]   = 45,
    [SPECIES_VENONAT - 1]    = 20,
    [SPECIES_DIGLETT - 1]    = 25,
    [SPECIES_DUGTRIO - 1]    = 35,
    [SPECIES_MEOWTH - 1]     = 40,
    [SPECIES_PERSIAN - 1]    = 20,
    [SPECIES_MANKEY - 1]     = 20,
    [SPECIES_GROWLITHE - 1]  = 30,
    [SPECIES_ARCANINE - 1]   = 40,
    [SPECIES_POLIWHIRL - 1]  = 5,
    [SPECIES_WEEPINBELL - 1] = 3,
    [SPECIES_MUK - 1]        = 45,
    [SPECIES_SHELLDER - 1]   = 20,
    [SPECIES_HAUNTER - 1]    = 23,
    [SPECIES_DROWZEE - 1]    = 48,
    [SPECIES_HYPNO - 1]      = 40,
    [SPECIES_HITMONCHAN - 1] = 25,
    [SPECIES_SCYTHER - 1]    = 10,
    [SPECIES_TAUROS - 1]     = 10,
    [SPECIES_TYPHLOSION - 1] = 20,
    [SPECIES_FERALIGATR - 1] = 5,
    [SPECIES_NATU - 1]       = 30,
    [SPECIES_MAREEP - 1]     = 50,
    [SPECIES_AMPHAROS - 1]   = 10,
    [SPECIES_POLITOED - 1]   = 40,
    [SPECIES_DUNSPARCE - 1]  = 10,
    [SPECIES_STEELIX - 1]    = 45,
    [SPECIES_QWILFISH - 1]   = 39,
    [SPECIES_SCIZOR - 1]     = 19,
    [SPECIES_OCTILLERY - 1]  = 20,
    [SPECIES_SMOOCHUM - 1]   = 40,
    [SPECIES_TYRANITAR - 1]  = 10,
    [SPECIES_LUGIA - 1]      = 20,
    [SPECIES_WAILORD - 1]    = 10,
    [SPECIES_KECLEON - 1]    = 30,
    [SPECIES_MILOTIC - 1]    = 45,
    [SPECIES_SPHEAL - 1]     = 15,
    [SPECIES_SNORUNT - 1]    = 20,
    [SPECIES_GRUMPIG - 1]    = 15,
    [SPECIES_WYNAUT - 1]     = 15,
    [SPECIES_DUSCLOPS - 1]   = 30,
    [SPECIES_ABSOL - 1]      = 45,
    [SPECIES_SALAMENCE - 1]  = 70,
    [SPECIES_KYOGRE - 1]     = 60,
    [SPECIES_RAYQUAZA - 1]   = 60,
};

#define STORE_PTR_IN_TASK(ptr, taskId, dataId)                 \
{                                                              \
    gTasks[taskId].data[dataId] = (u32)(ptr);                  \
    gTasks[taskId].data[dataId + 1] = (u32)(ptr) >> 16;        \
}

#define READ_PTR_FROM_TASK(taskId, dataId) \
    (struct Sprite *)((u32)(gTasks[taskId].data[dataId]) | ((u32)(gTasks[taskId].data[dataId + 1]) << 16))

#define sAnimId    data[2]
#define sAnimDelay data[3]

static void Task_AnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        LaunchAnimationTaskForFrontSprite(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        DestroyTask(taskId);
    }
}

static void __attribute__((unused)) Task_PokemonSummaryAnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        StartMonSummaryAnimation(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        // SummaryScreen_SetAnimDelayTaskId will be added in a later step
        // SummaryScreen_SetAnimDelayTaskId(0xFF);
        DestroyTask(taskId);
    }
}

void BattleAnimateFrontSprite(struct Sprite *sprite, u16 species, bool8 noCry, u8 panMode)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode | SKIP_FRONT_ANIM);
    else
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode);
}

void DoMonFrontSpriteAnimation(struct Sprite *sprite, u16 species, bool8 noCry, u8 panModeAnimFlag)
{
    s8 pan;
    switch (panModeAnimFlag & (u8)~SKIP_FRONT_ANIM) // Exclude anim flag to get pan mode
    {
    case 0:
        pan = -25;
        break;
    case 1:
        pan = 25;
        break;
    default:
        pan = 0;
        break;
    }
    if (panModeAnimFlag & SKIP_FRONT_ANIM)
    {
        // No animation, only check if cry needs to be played
        if (!noCry)
            PlayCry1(species, pan);
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (!noCry)
        {
            PlayCry1(species, pan);
            if (HasTwoFramesAnimation(species))
                StartSpriteAnim(sprite, 1);
        }
        if (sMonAnimationDelayTable[species - 1] != 0)
        {
            // Animation has delay, start delay task
            u8 taskId = CreateTask(Task_AnimateAfterDelay, 0);
            STORE_PTR_IN_TASK(sprite, taskId, 0);
            gTasks[taskId].sAnimId = sMonFrontAnimIdsTable[species - 1];
            gTasks[taskId].sAnimDelay = sMonAnimationDelayTable[species - 1];
        }
        else
        {
            // No delay, start animation
            LaunchAnimationTaskForFrontSprite(sprite, sMonFrontAnimIdsTable[species - 1]);
        }
        sprite->callback = SpriteCallbackDummy_2;
    }
}

// Friendship deltas. Each event has 3 separate values, depending on the mon's
// current friendship value. In general, a mon's friendship grows faster if
// its current friendship is lower. The 3 tiers are 0-99, 100-199, and 200-255.
static const s8 sFriendshipEventDeltas[][3] = {
    { 5,  3,   2}, // FRIENDSHIP_EVENT_GROW_LEVEL
    { 5,  3,   2}, // FRIENDSHIP_EVENT_VITAMIN
    { 1,  1,   0}, // FRIENDSHIP_EVENT_BATTLE_ITEM
    { 3,  2,   1}, // FRIENDSHIP_EVENT_LEAGUE_BATTLE
    { 1,  1,   0}, // FRIENDSHIP_EVENT_LEARN_TMHM
    { 1,  1,   1}, // FRIENDSHIP_EVENT_WALKING
    {-1, -1,  -1}, // FRIENDSHIP_EVENT_FAINT_SMALL
    {-5, -5, -10}, // FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE
    {-5, -5, -10}, // FRIENDSHIP_EVENT_FAINT_LARGE
};

void AdjustFriendship(struct Pokemon *mon, u8 event)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, 0);
    u16 heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    u8 holdEffect;

    if (heldItem == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            holdEffect = gEnigmaBerries[0].holdEffect;
        else
            holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
    }
    else
    {
        holdEffect = ItemId_GetHoldEffect(heldItem);
    }

    if (species && species != SPECIES_EGG)
    {
        u8 friendshipLevel = 0;
        s16 friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, 0);
        if (friendship > 99)
            friendshipLevel++;
        if (friendship > 199)
            friendshipLevel++;

        if ((event != FRIENDSHIP_EVENT_WALKING || !(Random() & 1))
         && (event != FRIENDSHIP_EVENT_LEAGUE_BATTLE
          || ((gBattleTypeFlags & BATTLE_TYPE_TRAINER)
           && (gTrainers[gTrainerBattleOpponent].trainerClass == TRAINER_CLASS_ELITE_FOUR
            || gTrainers[gTrainerBattleOpponent].trainerClass == TRAINER_CLASS_LEADER
            || gTrainers[gTrainerBattleOpponent].trainerClass == TRAINER_CLASS_CHAMPION))))
        {
            s8 delta = sFriendshipEventDeltas[event][friendshipLevel];
            if (delta > 0 && holdEffect == HOLD_EFFECT_HAPPINESS_UP)
                delta = (150 * delta) / 100;

            friendship += delta;
            if (delta > 0)
            {
                if (GetMonData(mon, MON_DATA_POKEBALL, 0) == ITEM_LUXURY_BALL)
                    friendship++;
                if (GetMonData(mon, MON_DATA_MET_LOCATION, 0) == sav1_map_get_name())
                    friendship++;
            }

            if (friendship < 0)
                friendship = 0;
            if (friendship > 255)
                friendship = 255;

            SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
        }
    }
}

void MonGainEVs(struct Pokemon *mon, u16 defeatedSpecies)
{
    u8 evs[NUM_STATS];
    u16 evIncrease = 0;
    u16 totalEVs = 0;
    u16 heldItem;
    u8 holdEffect;
    int i;

    for (i = 0; i < NUM_STATS; i++)
    {
        evs[i] = GetMonData(mon, MON_DATA_HP_EV + i, 0);
        totalEVs += evs[i];
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        u8 hasHadPokerus;
        int multiplier;

        if (totalEVs >= MAX_TOTAL_EVS)
            break;

        hasHadPokerus = CheckPartyHasHadPokerus(mon, 0);

        if (hasHadPokerus)
            multiplier = 2;
        else
            multiplier = 1;

        switch (i)
        {
        case 0:
            evIncrease = gBaseStats[defeatedSpecies].evYield_HP * multiplier;
            break;
        case 1:
            evIncrease = gBaseStats[defeatedSpecies].evYield_Attack * multiplier;
            break;
        case 2:
            evIncrease = gBaseStats[defeatedSpecies].evYield_Defense * multiplier;
            break;
        case 3:
            evIncrease = gBaseStats[defeatedSpecies].evYield_Speed * multiplier;
            break;
        case 4:
            evIncrease = gBaseStats[defeatedSpecies].evYield_SpAttack * multiplier;
            break;
        case 5:
            evIncrease = gBaseStats[defeatedSpecies].evYield_SpDefense * multiplier;
            break;
        }

        heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);

        if (heldItem == ITEM_ENIGMA_BERRY)
        {
            if (gMain.inBattle)
                holdEffect = gEnigmaBerries[0].holdEffect;
            else
                holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
        }
        else
        {
            holdEffect = ItemId_GetHoldEffect(heldItem);
        }

        if (holdEffect == HOLD_EFFECT_MACHO_BRACE)
            evIncrease *= 2;

        if (totalEVs + (s16)evIncrease > MAX_TOTAL_EVS)
            evIncrease = ((s16)evIncrease + MAX_TOTAL_EVS) - (totalEVs + evIncrease);

        if (evs[i] + (s16)evIncrease > 255)
        {
            int val1 = (s16)evIncrease + 255;
            int val2 = evs[i] + evIncrease;
            evIncrease = val1 - val2;
        }

        evs[i] += evIncrease;
        totalEVs += evIncrease;
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }
}

u16 GetMonEVCount(struct Pokemon *mon)
{
    int i;
    u16 count = 0;

    for (i = 0; i < NUM_STATS; i++)
        count += GetMonData(mon, MON_DATA_HP_EV + i, 0);

    return count;
}

void RandomlyGivePartyPokerus(struct Pokemon *party)
{
    u16 rnd = Random();
    if (rnd == 0x4000 || rnd == 0x8000 || rnd == 0xC000)
    {
        struct Pokemon *mon;

        do
        {
            do
            {
                rnd = Random() % PARTY_SIZE;
                mon = &party[rnd];
            }
            while (!GetMonData(mon, MON_DATA_SPECIES, 0));
        }
        while (GetMonData(mon, MON_DATA_IS_EGG, 0));

        if (!(CheckPartyHasHadPokerus(party, gBitTable[rnd])))
        {
            u8 rnd2;

            do
            {
                rnd2 = Random();
            }
            while (rnd2 == 0);

            if (rnd2 & 0xF0)
                rnd2 &= 0x07;

            rnd2 |= (rnd2 << 4);
            rnd2 &= 0xF3;
            rnd2++;

            SetMonData(&party[rnd], MON_DATA_POKERUS, &rnd2);
        }
    }
}

u8 CheckPartyPokerus(struct Pokemon *party, u8 selection)
{
    u8 retVal;

    int partyIndex = 0;
    unsigned curBit = 1;
    retVal = 0;

    if (selection)
    {
        do
        {
            if ((selection & 1) && (GetMonData(&party[partyIndex], MON_DATA_POKERUS, 0) & 0xF))
                retVal |= curBit;
            partyIndex++;
            curBit <<= 1;
            selection >>= 1;
        }
        while (selection);
    }
    else if (GetMonData(&party[0], MON_DATA_POKERUS, 0) & 0xF)
    {
        retVal = 1;
    }

    return retVal;
}

u8 CheckPartyHasHadPokerus(struct Pokemon *party, u8 selection)
{
    u8 retVal;

    int partyIndex = 0;
    unsigned curBit = 1;
    retVal = 0;

    if (selection)
    {
        do
        {
            if ((selection & 1) && GetMonData(&party[partyIndex], MON_DATA_POKERUS, 0))
                retVal |= curBit;
            partyIndex++;
            curBit <<= 1;
            selection >>= 1;
        }
        while (selection);
    }
    else if (GetMonData(&party[0], MON_DATA_POKERUS, 0))
    {
        retVal = 1;
    }

    return retVal;
}

void UpdatePartyPokerusTime(u16 days)
{
    int i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, 0))
        {
            u8 pokerus = GetMonData(&gPlayerParty[i], MON_DATA_POKERUS, 0);
            if (pokerus & 0xF)
            {
                if ((pokerus & 0xF) < days || days > 4)
                    pokerus &= 0xF0;
                else
                    pokerus -= days;

                SetMonData(&gPlayerParty[i], MON_DATA_POKERUS, &pokerus);
            }
        }
    }
}

void PartySpreadPokerus(struct Pokemon *party)
{
    if ((Random() % 3) == 0)
    {
        int i;
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&party[i], MON_DATA_SPECIES, 0))
            {
                u8 pokerus = GetMonData(&party[i], MON_DATA_POKERUS, 0);
                u8 curPokerus = pokerus;
                if (pokerus)
                {
                    if (pokerus & 0xF)
                    {
                        // spread to adjacent party members
                        if (i != 0 && !(GetMonData(&party[i - 1], MON_DATA_POKERUS, 0) & 0xF0))
                            SetMonData(&party[i - 1], MON_DATA_POKERUS, &curPokerus);
                        if (i != (PARTY_SIZE - 1) && !(GetMonData(&party[i + 1], MON_DATA_POKERUS, 0) & 0xF0))
                        {
                            SetMonData(&party[i + 1], MON_DATA_POKERUS, &curPokerus);
                            i++;
                        }
                    }
                }
            }
        }
    }
}

bool8 TryIncrementMonLevel(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 nextLevel = GetMonData(mon, MON_DATA_LEVEL, 0) + 1;
    if (GetMonData(mon, MON_DATA_EXP, 0) > gExperienceTables[gBaseStats[species].growthRate][nextLevel])
    {
        SetMonData(mon, MON_DATA_LEVEL, &nextLevel);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u32 CanMonLearnTMHM(struct Pokemon *mon, u8 tm)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, 0);
    if (species == SPECIES_EGG)
    {
        return 0;
    }
    else if (tm < 32)
    {
        u32 mask = 1 << tm;
        return gTMHMLearnsets[species][0] & mask;
    }
    else
    {
        u32 mask = 1 << (tm - 32);
        return gTMHMLearnsets[species][1] & mask;
    }
}

u8 GetMoveTutorMoves(struct Pokemon *mon, u16 *moves)
{
    u16 knownMoves[4];
    u8 numMoves = 0;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, 0);
    int i, j, k;

    for (i = 0; i < 4; i++)
        knownMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, 0);

    for (i = 0; i < MAX_MOVE_TUTOR_MOVES; i++)
    {
        u16 moveLevel;

        if (gLevelUpLearnsets[species][i] == 0xFFFF)
            break;

        moveLevel = gLevelUpLearnsets[species][i] & 0xFE00;
        if (moveLevel <= (level << 9))
        {
            for (j = 0; j < 4 && knownMoves[j] != (gLevelUpLearnsets[species][i] & 0x1FF); j++)
                ;

            if (j == 4)
            {
                for (k = 0; k < numMoves && moves[k] != (gLevelUpLearnsets[species][i] & 0x1FF); k++)
                    ;

                if (k == numMoves)
                    moves[numMoves++] = gLevelUpLearnsets[species][i] & 0x1FF;
            }
        }
    }

    return numMoves;
}

u8 GetLevelUpMovesBySpecies(u16 species, u16 *moves)
{
    u8 numMoves = 0;
    int i;

    for (i = 0; i < 20 && gLevelUpLearnsets[species][i] != 0xFFFF; i++)
         moves[numMoves++] = gLevelUpLearnsets[species][i] & 0x1FF;

     return numMoves;
}

u8 sub_8040574(struct Pokemon *mon)
{
    u16 learnedMoves[4];
    u16 moves[20];
    u8 numMoves = 0;
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, 0);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, 0);
    int i, j, k;

    if (species == SPECIES_EGG)
        return 0;

    for (i = 0; i < 4; i++)
        learnedMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, 0);

    for (i = 0; i < 20; i++)
    {
        u16 moveLevel;

        if (gLevelUpLearnsets[species][i] == 0xFFFF)
            break;

        moveLevel = gLevelUpLearnsets[species][i] & 0xFE00;

        if (moveLevel <= (level << 9))
        {
            for (j = 0; j < 4 && learnedMoves[j] != (gLevelUpLearnsets[species][i] & 0x1FF); j++)
                ;

            if (j == 4)
            {
                for (k = 0; k < numMoves && moves[k] != (gLevelUpLearnsets[species][i] & 0x1FF); k++)
                    ;

                if (k == numMoves)
                    moves[numMoves++] = gLevelUpLearnsets[species][i] & 0x1FF;
            }
        }
    }

    return numMoves;
}

u16 SpeciesToPokedexNum(u16 species)
{
    if (IsNationalPokedexEnabled())
    {
        return SpeciesToNationalPokedexNum(species);
    }
    else
    {
        species = SpeciesToHoennPokedexNum(species);
        if (species <= HOENN_DEX_COUNT)
            return species;
        return 0xFFFF;
    }
}

void ClearBattleMonForms(void)
{
    int i;
    for (i = 0; i < 4; i++)
        gBattleMonForms[i] = 0;
}

u16 GetMUS_ForBattle(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON)
        return MUS_VS_KYOGRE_GROUDON;
    if (gBattleTypeFlags & BATTLE_TYPE_REGI)
        return MUS_VS_REGI;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        return MUS_VS_TRAINER;
    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        switch (gTrainers[gTrainerBattleOpponent].trainerClass)
        {
        case 2:
        case 0x31:
            return MUS_VS_AQUA_MAGMA_LEADER;
        case 3:
        case 4:
        case 0x32:
        case 0x33:
            return MUS_VS_AQUA_MAGMA;
        case 0x19:
            return MUS_VS_GYM_LEADER;
        case 0x20:
            return MUS_VS_CHAMPION;
        case 0x2E:
            if (!StringCompare(gTrainers[gTrainerBattleOpponent].trainerName, BattleText_Wally))
                return MUS_VS_TRAINER;
            return MUS_VS_RIVAL;
        case 0x18:
            return MUS_VS_ELITE_FOUR;
        default:
            return MUS_VS_TRAINER;
        }
    }
    return MUS_VS_WILD;
}

void sub_80408BC(void)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    PlayBGM(GetMUS_ForBattle());
}

void current_map_music_set__default_for_battle(u16 song)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    if (song)
        PlayNewMapMusic(song);
    else
        PlayNewMapMusic(GetMUS_ForBattle());
}

const u8 *GetMonSpritePal(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, 0);
    u32 otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    return GetMonSpritePalFromOtIdPersonality(species, otId, personality);
}

//Extracts the upper 16 bits of a 32-bit number
#define HIHALF(n) (((n) & 0xFFFF0000) >> 16)

//Extracts the lower 16 bits of a 32-bit number
#define LOHALF(n) ((n) & 0xFFFF)

const u8 *GetMonSpritePalFromOtIdPersonality(u16 species, u32 otId, u32 personality)
{
    u32 shinyValue;

    if (species > SPECIES_EGG)
        return gMonPaletteTable[0].data;

    shinyValue = HIHALF(otId) ^ LOHALF(otId) ^ HIHALF(personality) ^ LOHALF(personality);
    if (shinyValue < 8)
        return gMonShinyPaletteTable[species].data;
    else
        return gMonPaletteTable[species].data;
}

const struct CompressedSpritePalette *GetMonSpritePalStruct(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, 0);
    u32 otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    return GetMonSpritePalStructFromOtIdPersonality(species, otId, personality);
}

const struct CompressedSpritePalette *GetMonSpritePalStructFromOtIdPersonality(u16 species, u32 otId , u32 personality)
{
    u32 shinyValue;

    shinyValue = HIHALF(otId) ^ LOHALF(otId) ^ HIHALF(personality) ^ LOHALF(personality);
    if (shinyValue < 8)
        return &gMonShinyPaletteTable[species];
    else
        return &gMonPaletteTable[species];
}

const u16 gHMMoves[] =
{
    MOVE_CUT,
    MOVE_FLY,
    MOVE_SURF,
    MOVE_STRENGTH,
    MOVE_FLASH,
    MOVE_ROCK_SMASH,
    MOVE_WATERFALL,
    MOVE_DIVE,
    0xffff
};

bool32 IsHMMove2(u16 move)
{
    int i = 0;
    while (gHMMoves[i] != 0xFFFF)
    {
        if (gHMMoves[i++] == move)
            return TRUE;
    }
    return FALSE;
}

bool8 IsPokeSpriteNotFlipped(u16 species)
{
    return gBaseStats[species].noFlip;
}

s8 sub_8040A54(struct Pokemon *mon, u8 a2)
{
    u8 nature = GetNature(mon);
    return gPokeblockFlavorCompatibilityTable[nature * 5 + a2];
}

s8 GetPokeFlavourRelation(u32 personality, u8 a2)
{
    u8 nature = GetNatureFromPersonality(personality);
    return gPokeblockFlavorCompatibilityTable[nature * 5 + a2];
}

bool8 IsTradedMon(struct Pokemon *mon)
{
    u8 otName[8];
    u32 otId;
    GetMonData(mon, MON_DATA_OT_NAME, otName);
    otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    return IsOtherTrainer(otId, otName);
}

bool8 IsOtherTrainer(u32 otId, u8 *otName)
{
    if (otId == (gSaveBlock2.playerTrainerId[0] | (gSaveBlock2.playerTrainerId[1] << 8) | (gSaveBlock2.playerTrainerId[2] << 16) | (gSaveBlock2.playerTrainerId[3] << 24)))
    {
        int i;

        for (i = 0; otName[i] != EOS; i++)
            if (otName[i] != gSaveBlock2.playerName[i])
                return TRUE;
        return FALSE;
    }

    return TRUE;
}

void BoxMonRestorePP(struct BoxPokemon *);

void MonRestorePP(struct Pokemon *mon)
{
    BoxMonRestorePP(&mon->box);
}

void BoxMonRestorePP(struct BoxPokemon *boxMon)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (GetBoxMonData(boxMon, MON_DATA_MOVE1 + i, 0))
        {
            u16 move = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i, 0);
            u16 bonus = GetBoxMonData(boxMon, MON_DATA_PP_BONUSES, 0);
            u8 pp = CalculatePPWithBonus(move, bonus, i);
            SetBoxMonData(boxMon, MON_DATA_PP1 + i, &pp);
        }
    }
}

void SetMonPreventsSwitchingString(void)
{
    gLastUsedAbility = gBattleStruct->abilityPreventingSwitchout;;
    gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
    gBattleTextBuff1[1] = B_BUFF_MON_NICK_WITH_PREFIX;
    gBattleTextBuff1[2] = gBattleStruct->battlerPreventingSwitchout;
    gBattleTextBuff1[4] = B_BUFF_EOS;
    if (GetBattlerSide(gBattleStruct->battlerPreventingSwitchout) == B_SIDE_PLAYER)
        gBattleTextBuff1[3] = pokemon_order_func(gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout]);
    else
        gBattleTextBuff1[3] = gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout];
    gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
    gBattleTextBuff2[1] = B_BUFF_MON_NICK_WITH_PREFIX;
    gBattleTextBuff2[2] = gBankInMenu;
    gBattleTextBuff2[3] = pokemon_order_func(gBattlerPartyIndexes[gBankInMenu]);
    gBattleTextBuff2[4] = B_BUFF_EOS;
    BattleStringExpandPlaceholders(BattleText_PreventedSwitch, gStringVar4);
}

void SetWildMonHeldItem(void)
{
    if (!(gBattleTypeFlags & (BATTLE_TYPE_LEGENDARY | BATTLE_TYPE_TRAINER)))
    {
        u16 rnd = Random() % 100;
        u16 species = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES, 0);
        if (gBaseStats[species].item1 == gBaseStats[species].item2)
        {
            SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &gBaseStats[species].item1);
            return;
        }

        if (rnd > 44)
        {
            if (rnd <= 94)
                SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &gBaseStats[species].item1);
            else
                SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &gBaseStats[species].item2);
        }
    }
}

bool8 IsShinyOtIdPersonality(u32, u32);

bool8 IsShiny(struct Pokemon *mon)
{
    u32 otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    return IsShinyOtIdPersonality(otId, personality);
}

bool8 IsShinyOtIdPersonality(u32 otId, u32 personality)
{
    bool8 retVal = FALSE;
    u32 shinyValue = HIHALF(otId) ^ LOHALF(otId) ^ HIHALF(personality) ^ LOHALF(personality);
    if (shinyValue < 8)
        retVal = TRUE;
    return retVal;
}

u8 *sub_8040D08(void)
{
    u8 id = GetMultiplayerId();
    return gLinkPlayers[sub_803FC34(gLinkPlayers[id].id ^ 2)].name;
}

const u8 gJapaneseNidoranNames[][11] = {_("ニドラン♂"), _("ニドラン♀")};

bool32 ShouldHideGenderIconForLanguage(u16 species, u8 *name, u8 language)
{
    bool32 retVal = FALSE;
    if (species == SPECIES_NIDORAN_M || species == SPECIES_NIDORAN_F)
    {
        const u8 *speciesName;
        if (language == GAME_LANGUAGE)
        {
            speciesName = gSpeciesNames[species];
        }
        else
        {
            if (species == SPECIES_NIDORAN_M)
                speciesName = gJapaneseNidoranNames[0];
            else
                speciesName = gJapaneseNidoranNames[1];
        }
        if (!StringCompareWithoutExtCtrlCodes(name, speciesName))
            retVal = TRUE;
        else
            retVal = FALSE;
    }
    return retVal;
}

bool32 ShouldHideGenderIcon(u16 species, u8 *name)
{
    u8 language = GAME_LANGUAGE;
    if (name[0] == 0xFC && name[1] == 21)
        language = LANGUAGE_JAPANESE;
    return ShouldHideGenderIconForLanguage(species, name, language);
}

bool32 unref_sub_8040DAC(struct Pokemon *mon)
{
    u8 name[12];
    u16 species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 language = GetMonData(mon, MON_DATA_LANGUAGE, 0);
    GetMonData(mon, MON_DATA_NICKNAME, name);
    return ShouldHideGenderIconForLanguage(species, name, language);
}
