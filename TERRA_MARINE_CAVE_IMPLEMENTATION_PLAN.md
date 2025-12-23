# Terra Cave and Marine Cave Implementation Plan

## Overview
This document outlines the step-by-step plan to implement the complete Terra Cave and Marine Cave functionality from Pokemon Emerald into Pokemon Ruby/Sapphire, including abnormal weather system, dynamic locations, and all related mechanics.

## Current Status Assessment

### ✅ Already Implemented
- Weather constants (`include/constants/weather.h`) - Complete
- Flags for caves (`include/constants/flags.h`) - Complete
- Variables (`include/constants/vars.h`) - Complete
- Map files exist:
  - `TerraCave_Entrance`
  - `TerraCave_End`
  - `MarineCave_Entrance`
  - `MarineCave_End`
  - `Underwater_MarineCave`
- Map scripts exist for all caves
- Region map sections defined
- Maps registered in `map_groups.json`
- Basic abnormal weather system (`src/abnormal_weather.c`)

### ❌ Needs Implementation/Verification
- Complete `AbnormalWeatherHasExpired` function (Emerald version with cave protection)
- Region map integration functions (`GetTerraOrMarineCaveMapSecId`, `GetMarineCaveCoords`)
- Route warps to Terra Cave entrance
- Underwater route warps to Marine Cave
- Dynamic warp system for Terra Cave exit
- Dive warp system for Marine Cave
- Layout files verification
- Sprite verification (Groudon/Kyogre front sprites)

---

## Implementation Steps

### Phase 1: Code Implementation

#### Step 1.1: Update `AbnormalWeatherHasExpired` in `src/field_specials.c`
**File:** `src/field_specials.c`

**Action:** Add the complete Emerald version of `AbnormalWeatherHasExpired` that:
- Checks if player is inside cave maps (prevents expiration)
- Properly handles step counter
- Returns TRUE when weather expires on target route

**Test:** Compile and verify no errors

---

#### Step 1.2: Add Region Map Integration Functions
**File:** `src/region_map.c`

**Actions:**
1. Add `GetTerraOrMarineCaveMapSecId()` function
2. Add `GetMarineCaveCoords()` function
3. Add arrays:
   - `sTerraOrMarineCaveMapSecIds[16]`
   - `sMarineCaveLocationCoords[8]`
4. Update region map display logic to use these functions

**Test:** Compile and verify no errors

---

#### Step 1.3: Verify `CreateAbnormalWeatherEvent` Implementation
**File:** `src/field_specials.c` (or check if in `src/abnormal_weather.c`)

**Action:** Ensure the function matches Emerald behavior:
- If Kyogre defeated → only Terra Cave locations
- If Groudon defeated → only Marine Cave locations
- Otherwise → 50/50 random choice

**Test:** Compile and verify

---

### Phase 2: Map Connections

#### Step 2.1: Verify Terra Cave Route Warps
**Files to check:**
- `data/maps/Route113/map.json`
- `data/maps/Route114/map.json`
- `data/maps/Route115/map.json`
- `data/maps/Route116/map.json`
- `data/maps/Route118/map.json`

**Action:** Verify each route has warp events that connect to `MAP_TERRA_CAVE_ENTRANCE` with dynamic warp system

**Test:** In-game, check if warps appear on routes during abnormal weather

---

#### Step 2.2: Verify Marine Cave Underwater Warps
**Files to check:**
- `data/maps/Underwater_Route105/map.json`
- `data/maps/Underwater_Route125/map.json`
- `data/maps/Underwater_Route127/map.json`
- `data/maps/Underwater_Route129/map.json`

**Action:** Verify each underwater route has warp events connecting to `MAP_UNDERWATER_MARINE_CAVE`

**Test:** In-game, dive on routes and check for cave entrance

---

#### Step 2.3: Verify Dynamic Warp System
**File:** `src/field_control_avatar.c` or `src/overworld.c`

**Action:** Ensure dynamic warp system properly handles `MAP_DYNAMIC` warps from Terra Cave entrance

**Test:** Enter Terra Cave and exit - should return to originating route

---

#### Step 2.4: Verify Dive Warp System
**Files:**
- `data/maps/MarineCave_Entrance/scripts.inc`
- `data/maps/Underwater_MarineCave/scripts.inc`

**Action:** Verify `setdivewarp` commands are correct

**Test:** Enter Marine Cave from underwater, emerge, then dive back

---

### Phase 3: Layouts and Graphics

#### Step 3.1: Verify Layout Files Exist
**Files to check:**
- `data/layouts/TerraCave_Entrance/`
- `data/layouts/TerraCave_End/`
- `data/layouts/MarineCave_Entrance/`
- `data/layouts/MarineCave_End/`
- `data/layouts/Underwater_MarineCave/`

**Action:** Verify all layout directories exist with:
- `border.bin`
- `map.bin`
- Layout entries in `data/layouts/layouts.json`

**Test:** Compile and check for missing layout errors

---

#### Step 3.2: Verify Sprite Graphics
**Files to check:**
- Groudon front sprite (`OBJ_EVENT_GFX_GROUDON_FRONT`)
- Kyogre front sprite (`OBJ_EVENT_GFX_KYOGRE_FRONT`)

**Action:** Verify sprites are defined in graphics system

**Test:** In-game, check if legendaries appear correctly

---

### Phase 4: Script Integration

#### Step 4.1: Verify Event Scripts Include
**File:** `data/event_scripts.s`

**Action:** Verify all cave scripts are included:
- `data/maps/TerraCave_Entrance/scripts.inc`
- `data/maps/TerraCave_End/scripts.inc`
- `data/maps/MarineCave_Entrance/scripts.inc`
- `data/maps/MarineCave_End/scripts.inc`
- `data/maps/Underwater_MarineCave/scripts.inc`

**Test:** Compile and verify no missing script errors

---

#### Step 4.2: Verify Abnormal Weather Scripts
**File:** `data/scripts/abnormal_weather.inc`

**Action:** Verify script handles both Terra and Marine Cave weather types

**Test:** Compile and verify

---

### Phase 5: Testing

#### Step 5.1: Test Abnormal Weather Creation
**Test Procedure:**
1. Complete game (defeat Elite Four)
2. Talk to Weather Institute scientist
3. Verify abnormal weather is created
4. Check `VAR_ABNORMAL_WEATHER_LOCATION` has valid value (1-16)

**Expected:** Weather location is set randomly

---

#### Step 5.2: Test Terra Cave Access
**Test Procedure:**
1. With Terra Cave weather active
2. Go to Route 114, 115, 116, or 118
3. Look for cave entrance warp
4. Enter cave
5. Navigate to end
6. Verify Groudon appears

**Expected:** Can access cave and see Groudon

---

#### Step 5.3: Test Marine Cave Access
**Test Procedure:**
1. With Marine Cave weather active
2. Dive on Route 105, 125, 127, or 129
3. Find underwater cave entrance
4. Enter and emerge
5. Navigate to end
6. Verify Kyogre appears

**Expected:** Can access cave and see Kyogre

---

#### Step 5.4: Test Dynamic Warp (Terra Cave Exit)
**Test Procedure:**
1. Enter Terra Cave from Route 114
2. Exit cave
3. Verify you return to Route 114 (not different route)

**Expected:** Returns to originating route

---

#### Step 5.5: Test Dive Warp (Marine Cave)
**Test Procedure:**
1. Enter Marine Cave from underwater
2. Emerge at entrance
3. Dive again
4. Verify you return to underwater location

**Expected:** Dive warp works both ways

---

#### Step 5.6: Test Legendary Encounters
**Test Procedure:**
1. Approach Groudon/Kyogre
2. Trigger battle
3. Test all outcomes:
   - Catch
   - Defeat
   - Run away

**Expected:** 
- Caught/defeated: Pokemon removed, flag set, weather ends
- Run away: Pokemon flies away, can return later

---

#### Step 5.7: Test Weather Expiration
**Test Procedure:**
1. Create abnormal weather
2. Walk 1000+ steps away from target route
3. Verify weather expires
4. Verify new location is chosen

**Expected:** Weather expires and moves to new location

---

#### Step 5.8: Test Weather Protection in Caves
**Test Procedure:**
1. Create abnormal weather
2. Enter cave
3. Walk 1000+ steps inside cave
4. Exit cave
5. Verify weather still active

**Expected:** Weather doesn't expire while in caves

---

#### Step 5.9: Test Region Map Display
**Test Procedure:**
1. Open region map
2. With abnormal weather active
3. Verify cave location shows on map
4. Verify cursor position is correct

**Expected:** Cave location visible on region map

---

#### Step 5.10: Test After Defeating Legendary
**Test Procedure:**
1. Defeat Groudon
2. Verify only Marine Cave locations possible
3. Defeat Kyogre
4. Verify no more abnormal weather

**Expected:** Logic works correctly after defeats

---

## File Checklist

### Source Files to Modify/Create
- [ ] `src/field_specials.c` - Add `AbnormalWeatherHasExpired` (Emerald version)
- [ ] `src/region_map.c` - Add cave map section functions
- [ ] `src/abnormal_weather.c` - Verify/update if needed

### Map Files to Verify
- [ ] `data/maps/TerraCave_Entrance/map.json`
- [ ] `data/maps/TerraCave_Entrance/scripts.inc`
- [ ] `data/maps/TerraCave_End/map.json`
- [ ] `data/maps/TerraCave_End/scripts.inc`
- [ ] `data/maps/MarineCave_Entrance/map.json`
- [ ] `data/maps/MarineCave_Entrance/scripts.inc`
- [ ] `data/maps/MarineCave_End/map.json`
- [ ] `data/maps/MarineCave_End/scripts.inc`
- [ ] `data/maps/Underwater_MarineCave/map.json`
- [ ] `data/maps/Underwater_MarineCave/scripts.inc`

### Route Warp Files to Verify
- [ ] `data/maps/Route113/map.json`
- [ ] `data/maps/Route114/map.json`
- [ ] `data/maps/Route115/map.json`
- [ ] `data/maps/Route116/map.json`
- [ ] `data/maps/Route118/map.json`
- [ ] `data/maps/Underwater_Route105/map.json`
- [ ] `data/maps/Underwater_Route125/map.json`
- [ ] `data/maps/Underwater_Route127/map.json`
- [ ] `data/maps/Underwater_Route129/map.json`

### Layout Files to Verify
- [ ] `data/layouts/TerraCave_Entrance/`
- [ ] `data/layouts/TerraCave_End/`
- [ ] `data/layouts/MarineCave_Entrance/`
- [ ] `data/layouts/MarineCave_End/`
- [ ] `data/layouts/Underwater_MarineCave/`
- [ ] `data/layouts/layouts.json` - Verify entries exist

### Other Files to Verify
- [ ] `data/maps/map_groups.json` - Verify cave maps in groups
- [ ] `data/event_scripts.s` - Verify script includes
- [ ] `data/scripts/abnormal_weather.inc` - Verify script logic
- [ ] `include/constants/flags.h` - Verify all flags exist
- [ ] `include/constants/vars.h` - Verify all variables exist
- [ ] `include/constants/weather.h` - Verify constants exist
- [ ] `include/constants/region_map_sections.h` - Verify map sections exist

---

## Implementation Order

1. **Code Implementation** (Steps 1.1-1.3)
   - Add missing functions
   - Fix compilation errors
   - Basic functionality

2. **Map Connections** (Steps 2.1-2.4)
   - Verify warps work
   - Test dynamic warps
   - Test dive warps

3. **Graphics & Layouts** (Steps 3.1-3.2)
   - Verify layouts exist
   - Verify sprites work

4. **Script Integration** (Steps 4.1-4.2)
   - Verify scripts included
   - Test script execution

5. **Comprehensive Testing** (Steps 5.1-5.10)
   - Test all functionality
   - Fix any bugs
   - Verify edge cases

---

## Notes

- The Ruby/Sapphire project already has `src/abnormal_weather.c` with a version-specific implementation. We need to ensure the Emerald-style functions in `field_specials.c` are also present and working.
- Dynamic warps use `MAP_DYNAMIC` which requires the dynamic warp system to be properly set up.
- Dive warps require the dive warp system to be functional.
- All testing should be done on a save file that has completed the main story (Elite Four defeated).

---

## Success Criteria

✅ All code compiles without errors
✅ All maps load correctly
✅ Terra Cave accessible from routes during weather
✅ Marine Cave accessible from underwater routes during weather
✅ Dynamic warps work correctly
✅ Dive warps work correctly
✅ Groudon and Kyogre appear and can be battled
✅ Weather system works correctly
✅ Region map shows cave locations
✅ All edge cases handled (defeated legendaries, weather expiration, etc.)

