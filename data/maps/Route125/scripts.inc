Route125_MapScripts:: @ 8151F1E
	map_script MAP_SCRIPT_ON_TRANSITION, Route125_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, Route125_OnResume
	map_script MAP_SCRIPT_ON_LOAD, Route125_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route125_OnFrame
	.byte 0

Route125_OnTransition::
.ifdef RUBY
	checkflag FLAG_DEFEATED_KYOGRE
	goto_if_eq Route125_OnTransition_EndWeather
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST
	call_if_eq AbnormalWeather_StartKyogreWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST
	call_if_eq AbnormalWeather_StartKyogreWeather
	end
.else
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetLegendaryWeather
	end
.endif

Route125_OnTransition_EndWeather::
	// Kyogre defeated - end weather and clean up tiles
	setvar VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_NONE
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 0
	setweather WEATHER_SUNNY
	doweather
	// Clean up both possible rough water locations
	call AbnormalWeather_EventScript_CleanupRoute125West
	call AbnormalWeather_EventScript_CleanupRoute125East
	special DrawWholeMapView
	end

Route125_OnResume::
.ifdef RUBY
	// Set dive warp dynamically based on abnormal weather location
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST
	goto_if_eq Route125_OnResume_SetWestDiveWarp
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST
	goto_if_eq Route125_OnResume_SetEastDiveWarp
.endif
	// Default dive warp (normal underwater route)
	setdivewarp MAP_UNDERWATER_ROUTE125, 255, 8, 6
	end

Route125_OnResume_SetWestDiveWarp::
	setdivewarp MAP_UNDERWATER_ROUTE125, 255, 9, 17
	end

Route125_OnResume_SetEastDiveWarp::
	setdivewarp MAP_UNDERWATER_ROUTE125, 255, 54, 19
	end

Route125_OnLoad::
.ifdef RUBY
	// Only place tiles if abnormal weather is active and Kyogre not defeated
	checkflag FLAG_DEFEATED_KYOGRE
	goto_if_eq Route125_OnLoad_End
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute125West
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute125East
	end
.endif

Route125_OnLoad_End::
	end

Route125_OnFrame::
.ifdef RUBY
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0
.else
	.2byte 0
.endif

Route125_EventScript_151F2E:: @ 8151F2E
	trainerbattle_single TRAINER_CODY, Route125_Text_1C0FCF, Route125_Text_1C1002
	msgbox Route125_Text_1C1013, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151F45:: @ 8151F45
	trainerbattle_single TRAINER_STAN, Route125_Text_1C104B, Route125_Text_1C1073
	msgbox Route125_Text_1C1083, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151F5C:: @ 8151F5C
	trainerbattle_single TRAINER_TANYA, Route125_Text_1C10C3, Route125_Text_1C10FB
	msgbox Route125_Text_1C110C, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151F73:: @ 8151F73
	trainerbattle_single TRAINER_SHARON, Route125_Text_1C1136, Route125_Text_1C1175
	msgbox Route125_Text_1C117F, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151F8A:: @ 8151F8A
	trainerbattle_single TRAINER_ERNEST_1, Route125_Text_1C11AF, Route125_Text_1C11ED
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, 1
	goto_if_eq Route125_EventScript_151FB1
	msgbox Route125_Text_1C1208, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151FB1:: @ 8151FB1
	trainerbattle_rematch TRAINER_ERNEST_1, Route125_Text_1C1282, Route125_Text_1C12C5
	msgbox Route125_Text_1C12EE, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151FC8:: @ 8151FC8
	trainerbattle_double TRAINER_KIM_AND_IRIS, Route125_Text_1C13A1, Route125_Text_1C13FF, Route125_Text_1C146E
	msgbox Route125_Text_1C141C, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_151FE3:: @ 8151FE3
	trainerbattle_double TRAINER_KIM_AND_IRIS, Route125_Text_1C14AB, Route125_Text_1C14E7, Route125_Text_1C154F
	msgbox Route125_Text_1C1508, MSGBOX_AUTOCLOSE
	end
