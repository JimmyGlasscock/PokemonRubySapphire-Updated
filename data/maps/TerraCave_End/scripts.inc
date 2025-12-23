TerraCave_End_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, TerraCave_End_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, TerraCave_End_OnTransition
	.byte 0

TerraCave_End_OnResume::
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, TerraCave_End_EventScript_TryRemoveGroudon
	end

TerraCave_End_EventScript_TryRemoveGroudon::
	removeobject 1
	return

TerraCave_End_OnTransition::
	setvar VAR_TEMP_0, 0
	checkflag FLAG_DEFEATED_GROUDON
	goto_if_eq TerraCave_End_OnTransition_Hide
	clearflag FLAG_HIDE_TERRA_CAVE_GROUDON
	end

TerraCave_End_OnTransition_Hide::
	setflag FLAG_HIDE_TERRA_CAVE_GROUDON
	setvar VAR_TEMP_0, 1
	end

TerraCave_End_EventScript_Groudon::
	checkflag FLAG_DEFEATED_GROUDON
	goto_if_eq TerraCave_End_EventScript_Groudon_End
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 1, TerraCave_End_Movement_GroudonApproach
	waitmovement 0
	delay 60
	setwildbattle SPECIES_GROUDON, 70, ITEM_NONE
	waitse
	playmoncry SPECIES_GROUDON, 2
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartTerraCaveGroudonBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, 1
	call_if_eq TerraCave_End_EventScript_DefeatedGroudon
	compare VAR_RESULT, 7
	call_if_eq TerraCave_End_EventScript_DefeatedGroudon
	compare VAR_RESULT, 2
	call_if_eq TerraCave_End_EventScript_RanFromGroudon
	releaseall
	end

TerraCave_End_EventScript_Groudon_End::
	releaseall
	end

TerraCave_End_EventScript_DefeatedGroudon::
	setflag FLAG_DEFEATED_GROUDON
	// End abnormal weather immediately, but keep cave tiles until map exit
	setvar VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_NONE
	removeobject VAR_LAST_TALKED
	return

TerraCave_End_EventScript_RanFromGroudon::
	setvar VAR_0x8004, SPECIES_GROUDON
	goto Common_EventScript_LegendaryFlewAway
	end

TerraCave_End_Movement_GroudonApproach::
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	delay_16
	delay_16
	walk_down_affine
	step_end
