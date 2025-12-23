Underwater_Route125_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Underwater_Route125_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Underwater_Route125_OnTransition
	.byte 0

Underwater_Route125_OnTransition::
.ifdef RUBY
	call AbnormalWeather_Underwater_SetupEscapeWarp
.endif
	end

Underwater_Route125_OnResume::
	setdivewarp MAP_ROUTE125, 255, 8, 6
	end

