Route115_MapScripts:: @ 815092D
	map_script MAP_SCRIPT_ON_TRANSITION, Route115_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route115_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route115_OnFrame
	.byte 0

Route115_OnTransition::
	checkflag FLAG_DEFEATED_GROUDON
	goto_if_eq Route115_OnTransition_EndWeather
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_115_WEST
	call_if_eq AbnormalWeather_StartGroudonWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_115_EAST
	call_if_eq AbnormalWeather_StartGroudonWeather
	end

Route115_OnTransition_EndWeather::
	// Groudon defeated - end weather and clean up tiles
	setvar VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_NONE
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 0
	setweather WEATHER_SUNNY
	doweather
	// Clean up both possible cave entrance locations
	call AbnormalWeather_EventScript_CleanupRoute115West
	call AbnormalWeather_EventScript_CleanupRoute115East
	special DrawWholeMapView
	end

Route115_OnLoad::
	// Only place tiles if abnormal weather is active and Groudon not defeated
	checkflag FLAG_DEFEATED_GROUDON
	goto_if_eq Route115_OnLoad_End
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_115_WEST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute115West
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_115_EAST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute115East
	end

Route115_OnLoad_End::
	end

Route115_OnFrame::
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route115_EventScript_15092E:: @ 815092E
	msgbox Route115_Text_170273, MSGBOX_NPC
	end

Route115_EventScript_150937:: @ 8150937
	msgbox Route115_Text_1702E7, MSGBOX_SIGN
	end

Route115_EventScript_150940:: @ 8150940
	msgbox Route115_Text_170301, MSGBOX_SIGN
	end

Route115_EventScript_150949:: @ 8150949
	trainerbattle_single TRAINER_TIMOTHY_1, Route115_Text_1BC9AC, Route115_Text_1BC9E6
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_150970
	msgbox Route115_Text_1BCA0E, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_150970:: @ 8150970
	trainerbattle_rematch TRAINER_TIMOTHY_1, Route115_Text_1BCA6B, Route115_Text_1BCAB3
	msgbox Route115_Text_1BCAC6, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_150987:: @ 8150987
	trainerbattle_single TRAINER_KOICHI, Route115_Text_1BCB0D, Route115_Text_1BCB2E
	msgbox Route115_Text_1BCB40, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_15099E:: @ 815099E
	trainerbattle_single TRAINER_NOB_1, Route115_Text_1BCB8A, Route115_Text_1BCBC1
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_1509C5
	msgbox Route115_Text_1BCBDE, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_1509C5:: @ 81509C5
	trainerbattle_rematch TRAINER_NOB_1, Route115_Text_1BCC4C, Route115_Text_1BCCA2
	msgbox Route115_Text_1BCCBA, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_1509DC:: @ 81509DC
	trainerbattle_single TRAINER_CYNDY_1, Route115_Text_1BCCF4, Route115_Text_1BCD32
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route115_EventScript_150A03
	msgbox Route115_Text_1BCD4C, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_150A03:: @ 8150A03
	trainerbattle_rematch TRAINER_CYNDY_1, Route115_Text_1BCDB0, Route115_Text_1BCDD0
	msgbox Route115_Text_1BCDEF, MSGBOX_AUTOCLOSE
	end

Route115_EventScript_150A1A:: @ 8150A1A
.ifdef SAPPHIRE
	trainerbattle_single TRAINER_514, UnknownString_81BCE47, UnknownString_81BCE7D
	msgbox UnknownString_81BCEA4, MSGBOX_AUTOCLOSE
.else
	trainerbattle_single TRAINER_514, Route115_Text_1BCEE0, Route115_Text_1BCF16
	msgbox Route115_Text_1BCF3D, MSGBOX_AUTOCLOSE
.endc
	end
