BirthIsland_Exterior_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, BirthIsland_Exterior_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, BirthIsland_Exterior_OnTransition
	.byte 0

BirthIsland_Exterior_OnResume::
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, BirthIsland_Exterior_EventScript_TryRemoveDeoxys
	end

BirthIsland_Exterior_EventScript_TryRemoveDeoxys::
	removeobject 1
	return

BirthIsland_Exterior_OnTransition::
	setvar VAR_TEMP_0, 0
	checkflag FLAG_DEFEATED_DEOXYS
	goto_if_eq BirthIsland_Exterior_OnTransition_Hide
	clearflag FLAG_HIDE_DEOXYS
	end

BirthIsland_Exterior_OnTransition_Hide::
	setflag FLAG_HIDE_DEOXYS
	setvar VAR_TEMP_0, 1
	end

BirthIsland_Exterior_EventScript_Deoxys::
	checkflag FLAG_DEFEATED_DEOXYS
	goto_if_eq BirthIsland_Exterior_EventScript_Deoxys_End
	lockall
	faceplayer
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	playmoncry SPECIES_DEOXYS, 2
	delay 40
	waitmoncry
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	setvar VAR_LAST_TALKED, 1
	setwildbattle SPECIES_DEOXYS, 30, ITEM_NONE
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartSouthernIslandBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setflag FLAG_DEFEATED_DEOXYS
	removeobject VAR_LAST_TALKED
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	call_if_eq BirthIsland_Exterior_EventScript_DefeatedDeoxys
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	call_if_eq BirthIsland_Exterior_EventScript_DefeatedDeoxys
	compare VAR_RESULT, B_OUTCOME_RAN
	call_if_eq BirthIsland_Exterior_EventScript_RanFromDeoxys
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	call_if_eq BirthIsland_Exterior_EventScript_RanFromDeoxys
	releaseall
	end

BirthIsland_Exterior_EventScript_Deoxys_End::
	releaseall
	end

BirthIsland_Exterior_EventScript_DefeatedDeoxys::
	return

BirthIsland_Exterior_EventScript_RanFromDeoxys::
	setvar VAR_0x8004, SPECIES_DEOXYS
	goto Common_EventScript_LegendaryFlewAway
	end
