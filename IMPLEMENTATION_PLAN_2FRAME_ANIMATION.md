# Implementation Plan: 2-Frame Animation System Port

## Overview
Port the 2-frame animation system from Pokemon Emerald to Pokemon Ruby/Sapphire. This enables Pokemon to animate using two frames (frame 0 and frame 1) when first sent out in battle or viewed on the summary screen.

## Current State Analysis

### ✅ What Already Exists:
1. `HasTwoFramesAnimation()` in `src/pokemon_animation.c` (but uses different logic - counts animation pointers)
2. `PokemonSummaryDoMonAnimation()` exists (referenced in summary screen)
3. `LaunchAnimationTaskForFrontSprite()` in `src/pokemon_animation.c`
4. `StartMonSummaryAnimation()` in `src/pokemon_animation.c`
5. `SetSpriteCB_MonAnimDummy()` in `src/pokemon_animation.c`
6. `SummaryScreen_SetAnimDelayTaskId()` in `src/pokemon_summary_screen.c`
7. `StopPokemonAnimationDelayTask()` exists
8. `BattleAnimateBackSprite()` exists
9. `BATTLER_AFFINE_EMERGE` and `BATTLER_AFFINE_NORMAL` constants exist
10. Summary screen already calls `PokemonSummaryDoMonAnimation()`

### ❌ What's Missing:
1. `DoMonFrontSpriteAnimation()` function
2. `BattleAnimateFrontSprite()` function
3. `SpriteCB_OpponentMonFromBall()` callback
4. `SpriteCB_PlayerMonFromBall()` callback
5. `Task_AnimateAfterDelay()` function
6. `Task_PokemonSummaryAnimateAfterDelay()` function
7. `sMonFrontAnimIdsTable[]` data table
8. `sMonAnimationDelayTable[]` data table
9. Integration in pokeball release callbacks
10. Updated `HasTwoFramesAnimation()` to match Emerald's exclusion-based logic
11. `SKIP_FRONT_ANIM` constant
12. `SpriteCallbackDummy_2` function (may need to verify)

## Implementation Steps

### Phase 1: Update Core Animation Detection

#### Step 1.1: Update `HasTwoFramesAnimation()` in `src/pokemon_animation.c`

**Current implementation (lines 7-24):**
```c
bool8 HasTwoFramesAnimation(u16 species)
{
    const union AnimCmd *const *anims;
    u32 count;
    
    if (species >= NUM_SPECIES)
        return FALSE;
    
    anims = gMonFrontAnimsPtrTable[species];
    count = 0;
    
    // Count non-NULL pointers
    while (anims[count] != NULL)
        count++;
    
    // If there are 3 pointers (Frame 0, Frame 1, Frame 2), it's a two-frame animation
    return (count == 3);
}
```

**Replace with Emerald's implementation:**
```c
bool8 HasTwoFramesAnimation(u16 species)
{
    return (species != SPECIES_CASTFORM
         && species != SPECIES_DEOXYS
         && species != SPECIES_SPINDA
         && species != SPECIES_UNOWN);
}
```

**Rationale:** Emerald uses a simpler exclusion-based approach. Castform has 4 frames, Deoxys has 2 frames (for forms), Spinda and Unown have 1 frame.

---

### Phase 2: Add Animation Data Tables

#### Step 2.1: Add Animation ID and Delay Tables

**File:** Determine best location - likely `src/pokemon_3.c` (since pokemon_1.c and pokemon_2.c exist)

**Add these tables before any functions that use them:**

1. **`sMonFrontAnimIdsTable[NUM_SPECIES - 1]`** - Maps species to animation ID
   - Source: Emerald `src/pokemon.c` lines 1405-1793
   - Size: ~400 entries (one per species minus SPECIES_NONE)
   - Format: `[SPECIES_X - 1] = ANIM_Y`

2. **`sMonAnimationDelayTable[NUM_SPECIES - 1]`** - Maps species to animation delay frames
   - Source: Emerald `src/pokemon.c` lines 1795-1804+ (continues for many entries)
   - Size: ~400 entries (most are 0, only some species have delays)
   - Format: `[SPECIES_X - 1] = delay_frames`

**Important:** Verify `NUM_SPECIES` matches between projects. Ruby/Sapphire may have fewer species than Emerald.

**Location in file:** Add near the top after includes, before any functions.

---

### Phase 3: Add Animation Delay Task Functions

#### Step 3.1: Add Task Functions and Macros

**File:** Same file as data tables (likely `src/pokemon_3.c`)

**Add these macros and functions:**

```c
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

static void Task_PokemonSummaryAnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        StartMonSummaryAnimation(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        SummaryScreen_SetAnimDelayTaskId(TASK_NONE);
        DestroyTask(taskId);
    }
}
```

**Source:** Emerald `src/pokemon.c` lines 6761-6787

**Dependencies:**
- `LaunchAnimationTaskForFrontSprite()` - ✅ exists
- `StartMonSummaryAnimation()` - ✅ exists
- `SummaryScreen_SetAnimDelayTaskId()` - ✅ exists
- `TASK_NONE` constant - need to verify

---

### Phase 4: Add Core Animation Functions

#### Step 4.1: Add `BattleAnimateFrontSprite()` and `DoMonFrontSpriteAnimation()`

**File:** Same file as above (likely `src/pokemon_3.c`)

**Add these functions:**

```c
void BattleAnimateFrontSprite(struct Sprite *sprite, u16 species, bool8 noCry, u8 panMode)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)))
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
            PlayCry_Normal(species, pan);
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (!noCry)
        {
            PlayCry_Normal(species, pan);
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
```

**Source:** Emerald `src/pokemon.c` lines 6789-6842

**Dependencies to verify:**
- `PlayCry_Normal()` - need to check if exists or use alternative
- `SpriteCallbackDummy_2` - need to verify exists
- `SKIP_FRONT_ANIM` - need to add constant
- `StartSpriteAnim()` - ✅ exists
- `CreateTask()` - ✅ exists
- `gTasks[]` - ✅ exists

---

### Phase 5: Add Battle Entry Callbacks

#### Step 5.1: Add Sprite Callbacks for Pokemon from Ball

**File:** `src/battle_main.c`

**Add these callbacks (add near other sprite callbacks, around line 3200):**

```c
void SpriteCB_OpponentMonFromBall(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
    {
        if (!(gHitMarker & HITMARKER_NO_ANIMATIONS) || gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
        {
            if (HasTwoFramesAnimation(sprite->sSpeciesId))
                StartSpriteAnim(sprite, 1);
        }
        BattleAnimateFrontSprite(sprite, sprite->sSpeciesId, TRUE, 1);
    }
}

void SpriteCB_PlayerMonFromBall(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        BattleAnimateBackSprite(sprite, sprite->sSpeciesId);
}
```

**Source:** Emerald `src/battle_main.c` lines 2837-2991

**Dependencies:**
- `sprite->sSpeciesId` - need to verify sprite data structure
- `BattleAnimateBackSprite()` - ✅ exists
- `BattleAnimateFrontSprite()` - will be added in Phase 4

**Note:** In Ruby/Sapphire, species might be stored in `sprite->data[2]` instead of `sprite->sSpeciesId`. Check the pokeball code to see how species is stored.

---

#### Step 5.2: Update Pokeball Release Callback

**File:** `src/pokeball.c`

**Find the function that releases Pokemon from ball (around line 700-760):**

**Current code (around line 757-759):**
```c
StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[sprite->data[6]]], 1);
AnimateSprite(&gSprites[gBattlerSpriteIds[sprite->data[6]]]);
gSprites[gBattlerSpriteIds[sprite->data[6]]].data[1] = 0x1000;
```

**Replace with:**
```c
u8 battler = sprite->data[6];
u16 species = GetMonData(/* get from appropriate party */, MON_DATA_SPECIES);

// Store species in sprite data
gSprites[gBattlerSpriteIds[battler]].data[2] = species; // or use sSpeciesId if struct has it

StartSpriteAffineAnim(&gSprites[gBattlerSpriteIds[battler]], BATTLER_AFFINE_EMERGE);

if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
    gSprites[gBattlerSpriteIds[battler]].callback = SpriteCB_OpponentMonFromBall;
else
    gSprites[gBattlerSpriteIds[battler]].callback = SpriteCB_PlayerMonFromBall;

AnimateSprite(&gSprites[gBattlerSpriteIds[battler]]);
gSprites[gBattlerSpriteIds[battler]].data[1] = 0x1000;
```

**Source:** Emerald `src/pokeball.c` lines 815-824

**Important:** 
- Need to get species from the Pokemon data (already available in the function around line 728)
- Verify how to access species in sprite (may need to use `data[2]` or add `sSpeciesId` field)
- Change affine anim from `1` to `BATTLER_AFFINE_EMERGE`

---

### Phase 6: Verify Summary Screen Implementation

#### Step 6.1: Check `PokemonSummaryDoMonAnimation()` Implementation

**File:** `src/pokemon_3.c` (or wherever it's defined)

**Verify it matches Emerald's implementation:**

**Expected implementation:**
```c
void PokemonSummaryDoMonAnimation(struct Sprite *sprite, u16 species, bool8 oneFrame)
{
    if (!oneFrame && HasTwoFramesAnimation(species))
        StartSpriteAnim(sprite, 1);
    if (sMonAnimationDelayTable[species - 1] != 0)
    {
        // Animation has delay, start delay task
        u8 taskId = CreateTask(Task_PokemonSummaryAnimateAfterDelay, 0);
        STORE_PTR_IN_TASK(sprite, taskId, 0);
        gTasks[taskId].sAnimId = sMonFrontAnimIdsTable[species - 1];
        gTasks[taskId].sAnimDelay = sMonAnimationDelayTable[species - 1];
        SummaryScreen_SetAnimDelayTaskId(taskId);
        SetSpriteCB_MonAnimDummy(sprite);
    }
    else
    {
        // No delay, start animation
        StartMonSummaryAnimation(sprite, sMonFrontAnimIdsTable[species - 1]);
    }
}
```

**If it exists but is different, update it to match.**

**Source:** Emerald `src/pokemon.c` lines 6844-6863

---

### Phase 7: Add Constants and Headers

#### Step 7.1: Add `SKIP_FRONT_ANIM` Constant

**File:** `include/constants/pokemon.h`

**Add after other constants (around line 150):**
```c
#define SKIP_FRONT_ANIM (1 << 7)
```

**Source:** Emerald `include/constants/pokemon.h` line 278

---

#### Step 7.2: Add Function Declarations

**File:** `include/pokemon.h` (or appropriate header)

**Add declarations:**
```c
bool8 HasTwoFramesAnimation(u16 species);
void BattleAnimateFrontSprite(struct Sprite *sprite, u16 species, bool8 noCry, u8 panMode);
void DoMonFrontSpriteAnimation(struct Sprite *sprite, u16 species, bool8 noCry, u8 panModeAnimFlag);
void PokemonSummaryDoMonAnimation(struct Sprite *sprite, u16 species, bool8 oneFrame);
```

**File:** `include/battle_main.h` (or appropriate header)

**Add declarations:**
```c
void SpriteCB_OpponentMonFromBall(struct Sprite *sprite);
void SpriteCB_PlayerMonFromBall(struct Sprite *sprite);
```

---

### Phase 8: Verify Dependencies

#### Step 8.1: Check Required Functions/Constants

**Verify these exist or find alternatives:**

1. ✅ `LaunchAnimationTaskForFrontSprite()` - exists
2. ✅ `StartMonSummaryAnimation()` - exists
3. ✅ `SetSpriteCB_MonAnimDummy()` - exists
4. ✅ `SummaryScreen_SetAnimDelayTaskId()` - exists
5. ✅ `BattleAnimateBackSprite()` - exists
6. ✅ `BATTLER_AFFINE_EMERGE` - exists
7. ✅ `BATTLER_AFFINE_NORMAL` - exists
8. ❓ `PlayCry_Normal()` - need to verify or use `PlayCry_ByMode()`
9. ❓ `SpriteCallbackDummy_2` - need to verify
10. ❓ `TASK_NONE` - need to verify
11. ✅ `StartSpriteAnim()` - exists
12. ✅ `CreateTask()` - exists
13. ✅ `DestroyTask()` - exists
14. ✅ `FindTaskIdByFunc()` - exists (used in StopPokemonAnimationDelayTask)

**If `PlayCry_Normal()` doesn't exist:**
- Check for `PlayCry_ByMode(species, pan, CRY_MODE_NORMAL)` as alternative
- Or check what cry function is used in battle

**If `SpriteCallbackDummy_2` doesn't exist:**
- Check if `SpriteCallbackDummy` is used instead
- Or create it as an empty function

---

## File Modification Summary

### Files to Modify:

1. **`src/pokemon_animation.c`**
   - Update `HasTwoFramesAnimation()` (simple change)

2. **`src/pokemon_3.c`** (or appropriate pokemon file)
   - Add `sMonFrontAnimIdsTable[]` data table
   - Add `sMonAnimationDelayTable[]` data table
   - Add `Task_AnimateAfterDelay()` function
   - Add `Task_PokemonSummaryAnimateAfterDelay()` function
   - Add `BattleAnimateFrontSprite()` function
   - Add `DoMonFrontSpriteAnimation()` function
   - Verify/update `PokemonSummaryDoMonAnimation()` if needed

3. **`src/battle_main.c`**
   - Add `SpriteCB_OpponentMonFromBall()` callback
   - Add `SpriteCB_PlayerMonFromBall()` callback

4. **`src/pokeball.c`**
   - Update pokeball release callback (around line 757) to set new sprite callbacks
   - Store species ID in sprite data
   - Change affine anim to `BATTLER_AFFINE_EMERGE`

5. **`include/constants/pokemon.h`**
   - Add `SKIP_FRONT_ANIM` constant

6. **`include/pokemon.h`** (or appropriate header)
   - Add function declarations

7. **`include/battle_main.h`** (or appropriate header)
   - Add callback declarations

---

## Implementation Order

1. **First:** Update `HasTwoFramesAnimation()` (simple, low risk)
2. **Second:** Add constants and headers (foundation)
3. **Third:** Add data tables (large but straightforward - verify NUM_SPECIES first)
4. **Fourth:** Add delay task functions and macros
5. **Fifth:** Add core animation functions
6. **Sixth:** Add battle callbacks
7. **Seventh:** Update pokeball release
8. **Eighth:** Verify summary screen
9. **Ninth:** Test and debug

---

## Critical Notes

1. **Species ID Storage:** Ruby/Sapphire may use `sprite->data[2]` instead of `sprite->sSpeciesId`. Check pokeball.c to see how species is currently stored.

2. **NUM_SPECIES:** Verify this constant matches between projects. Ruby/Sapphire may have fewer species than Emerald.

3. **Function Names:** Some functions may have different names. Check for:
   - `PlayCry_Normal()` vs `PlayCry_ByMode()`
   - `SpriteCallbackDummy_2` vs alternatives

4. **Data Table Size:** The animation tables are very large (~400 entries each). Ensure proper indexing with `[species - 1]` since SPECIES_NONE is excluded.

5. **Battler Variable:** Ruby/Sapphire uses `sprite->data[6]` for battler, while Emerald may use `sprite->sBattler`. Adapt accordingly.

6. **Affine Animation:** Change from `1` to `BATTLER_AFFINE_EMERGE` constant.

---

## Testing Checklist

After implementation, test:

- ✅ Pokemon animate with 2 frames when sent out in battle (opponent side)
- ✅ Pokemon animate correctly when sent out (player side - back sprite)
- ✅ Pokemon animate with 2 frames in summary screen
- ✅ Animation delays work correctly for species that have them
- ✅ Special cases handled:
  - Castform (4 frames - should not use 2-frame system)
  - Deoxys (2 frames for forms - should not use 2-frame system)
  - Spinda (1 frame - should not animate)
  - Unown (1 frame - should not animate)
- ✅ No crashes or visual glitches
- ✅ Works in single battles
- ✅ Works in double battles
- ✅ Works in link battles (if applicable)

---

## Success Criteria

✅ All Pokemon (except Castform, Deoxys, Spinda, Unown) animate with 2 frames
✅ Animations trigger correctly on battle entry
✅ Animations trigger correctly on summary screen
✅ Animation delays work for species that have them
✅ No regressions in existing functionality
✅ Code compiles without errors
✅ No crashes during gameplay

