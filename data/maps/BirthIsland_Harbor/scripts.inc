BirthIsland_Harbor_MapScripts::
	.byte 0

BirthIsland_Harbor_EventScript_Sailor::
	lock
	faceplayer
	msgbox BirthIsland_Harbor_Text_SailorReturn, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BirthIsland_Harbor_EventScript_AsYouLike
	msgbox SouthernIsland_Exterior_Text_1C5281, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	hideobjectat 1, MAP_BIRTH_ISLAND_HARBOR
	setvar VAR_0x8004, 2
	call Common_EventScript_FerryDepartIsland
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BirthIsland_Harbor_EventScript_AsYouLike::
	msgbox SouthernIsland_Exterior_Text_1C52A6, MSGBOX_DEFAULT
	release
	end

