MarineCave_End_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, MarineCave_End_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, MarineCave_End_OnTransition
	.byte 0

MarineCave_End_OnResume::
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, MarineCave_End_EventScript_TryRemoveKyogre
	end

MarineCave_End_EventScript_TryRemoveKyogre::
	removeobject 1
	return

MarineCave_End_OnTransition::
	setvar VAR_TEMP_1, 0
	checkflag FLAG_DEFEATED_KYOGRE
	goto_if_eq MarineCave_End_OnTransition_Hide
	clearflag FLAG_HIDE_MARINE_CAVE_KYOGRE
	end

MarineCave_End_OnTransition_Hide::
	setflag FLAG_HIDE_MARINE_CAVE_KYOGRE
	setvar VAR_TEMP_1, 1
	end

MarineCave_End_EventScript_Kyogre::
	checkflag FLAG_DEFEATED_KYOGRE
	goto_if_eq MarineCave_End_EventScript_Kyogre_End
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	applymovement 1, MarineCave_End_Movement_KyogreApproach
	waitmovement 0
	delay 60
	setwildbattle SPECIES_KYOGRE, 70, ITEM_NONE
	waitse
	playmoncry SPECIES_KYOGRE, 2
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special ScrSpecial_StartMarineCaveKyogreBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, 1
	call_if_eq MarineCave_End_EventScript_DefeatedKyogre
	compare VAR_RESULT, 2
	call_if_eq MarineCave_End_EventScript_RanFromKyogre
	releaseall
	end

MarineCave_End_EventScript_Kyogre_End::
	releaseall
	end

MarineCave_End_EventScript_DefeatedKyogre::
	setflag FLAG_DEFEATED_KYOGRE
	goto Common_EventScript_RemoveStaticPokemon
	end

MarineCave_End_EventScript_RanFromKyogre::
	setvar VAR_0x8004, SPECIES_KYOGRE
	goto Common_EventScript_LegendaryFlewAway
	end

MarineCave_End_Movement_KyogreApproach::
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	delay_16
	delay_16
	walk_down_affine
	step_end
