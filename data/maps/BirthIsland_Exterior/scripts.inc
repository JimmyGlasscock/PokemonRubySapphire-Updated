BirthIsland_Exterior_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, BirthIsland_Exterior_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, BirthIsland_Exterior_OnResume
	.byte 0

BirthIsland_Exterior_OnTransition:
	setflag FLAG_MAP_SCRIPT_CHECKED_DEOXYS
	setvar VAR_DEOXYS_ROCK_STEP_COUNT, 0
	setvar VAR_DEOXYS_ROCK_LEVEL, 0
	call_if_set FLAG_BATTLED_DEOXYS, BirthIsland_Exterior_EventScript_HideDeoxysAndPuzzle
	call_if_unset FLAG_BATTLED_DEOXYS, BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle
	end

BirthIsland_Exterior_EventScript_HideDeoxysAndPuzzle::
	setflag FLAG_HIDE_DEOXYS
	setflag FLAG_HIDE_BIRTH_ISLAND_DEOXYS_TRIANGLE
	return

BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle::
	goto_if_set FLAG_DEFEATED_DEOXYS, BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle_End
	clearflag FLAG_HIDE_BIRTH_ISLAND_DEOXYS_TRIANGLE
	clearflag FLAG_DEOXYS_ROCK_COMPLETE
	setflag FLAG_HIDE_DEOXYS
BirthIsland_Exterior_EventScript_TryShowDeoxysPuzzle_End:
	return

BirthIsland_Exterior_OnResume:
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, BirthIsland_Exterior_EventScript_TryRemoveDeoxys
	end

BirthIsland_Exterior_EventScript_TryRemoveDeoxys::
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne BirthIsland_Exterior_EventScript_TryRemoveDeoxys_End
	removeobject 1
BirthIsland_Exterior_EventScript_TryRemoveDeoxys_End:
	return

BirthIsland_Exterior_EventScript_Triangle::
	lock
	faceplayer
	special DoDeoxysRockInteraction
	waitstate
	compare VAR_RESULT, DEOXYS_ROCK_FAILED
	goto_if_eq BirthIsland_Exterior_EventScript_Failed
	compare VAR_RESULT, DEOXYS_ROCK_PROGRESSED
	goto_if_eq BirthIsland_Exterior_EventScript_Progressed
	compare VAR_RESULT, DEOXYS_ROCK_SOLVED
	goto_if_eq BirthIsland_Exterior_EventScript_Deoxys
	compare VAR_RESULT, DEOXYS_ROCK_COMPLETE
	goto_if_eq BirthIsland_Exterior_EventScript_Complete
	release
	end

@ The actual rock triangle movement is handled by DoDeoxysRockInteraction.
@ Unless the player has solved the puzzle and needs to encounter Deoxys,
@ there's nothing else the script needs to do.
BirthIsland_Exterior_EventScript_Failed::
	release
	end

BirthIsland_Exterior_EventScript_Progressed::
	release
	end

BirthIsland_Exterior_EventScript_Complete::
	release
	end

BirthIsland_Exterior_EventScript_Deoxys::
	waitse
	setfieldeffectargument 0, 0
	setfieldeffectargument 1, 11
	setfieldeffectargument 2, 26
	dofieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	waitfieldeffect FLDEFF_DESTROY_DEOXYS_ROCK
	addobject 1
	applymovement 1, BirthIsland_Exterior_Movement_DeoxysApproach
	waitmovement 0
	waitse
	playmoncry SPECIES_DEOXYS, 2
	delay 40
	waitmoncry
	setvar VAR_LAST_TALKED, 1
	setwildbattle SPECIES_DEOXYS, 30, ITEM_NONE
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartSouthernIslandBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq BirthIsland_Exterior_EventScript_DefeatedDeoxys
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq BirthIsland_Exterior_EventScript_RanFromDeoxys
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq BirthIsland_Exterior_EventScript_RanFromDeoxys
	setflag FLAG_BATTLED_DEOXYS
	release
	end

BirthIsland_Exterior_EventScript_DefeatedDeoxys::
	setflag FLAG_DEFEATED_DEOXYS
	setvar VAR_0x8004, SPECIES_DEOXYS
	goto Common_EventScript_LegendaryFlewAway
	end

BirthIsland_Exterior_EventScript_RanFromDeoxys::
	setvar VAR_0x8004, SPECIES_DEOXYS
	goto Common_EventScript_LegendaryFlewAway
	end

BirthIsland_Exterior_Movement_DeoxysApproach:
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	step_end
