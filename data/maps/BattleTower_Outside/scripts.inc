BattleTower_Outside_MapScripts:: @ 8160156
	map_script MAP_SCRIPT_ON_TRANSITION, BattleTower_Outside_MapScript1_16015C
	.byte 0

BattleTower_Outside_MapScript1_16015C:: @ 816015C
	setflag FLAG_LANDMARK_BATTLE_TOWER
	setvar VAR_BRAVO_TRAINER_BATTLE_TOWER_ON, 0
	setflag FLAG_HIDE_REPORTER_BATTLE_TOWER
	end

BattleTower_Outside_EventScript_160168:: @ 8160168
	lock
	faceplayer
	msgbox BattleTower_Outside_Text_199D06, MSGBOX_DEFAULT
	checkitem ITEM_SS_TICKET, 1
	compare VAR_RESULT, 0
	goto_if_eq BattleTower_Outside_EventScript_1601C6
	message BattleTower_Outside_Text_199D9D
	waitmessage
	goto BattleTower_Outside_EventScript_16018E
	end

BattleTower_Outside_EventScript_16018E:: @ 816018E
	multichoicedefault 21, 6, 53, 2, 0
	switch VAR_RESULT
	case 0, BattleTower_Outside_EventScript_1601D0
	case 1, BattleTower_Outside_EventScript_1601FB
	case 2, BattleTower_Outside_EventScript_160250
	case 127, BattleTower_Outside_EventScript_160250
	end

BattleTower_Outside_EventScript_1601C6:: @ 81601C6
	msgbox BattleTower_Outside_Text_199D40, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_1601D0:: @ 81601D0
	msgbox BattleTower_Outside_Text_199DF2, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_160226
	msgbox BattleTower_Outside_Text_199E4B, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_160232
	warp MAP_SLATEPORT_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_1601FB:: @ 81601FB
	msgbox BattleTower_Outside_Text_199E0E, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_160226
	msgbox BattleTower_Outside_Text_199E4B, MSGBOX_DEFAULT
	call BattleTower_Outside_EventScript_160232
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BattleTower_Outside_EventScript_160226:: @ 8160226
	message BattleTower_Outside_Text_199E7A
	waitmessage
	goto BattleTower_Outside_EventScript_16018E
	end

BattleTower_Outside_EventScript_160232:: @ 8160232
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	hideobjectat 5, MAP_BATTLE_TOWER_OUTSIDE
	setvar VAR_0x8004, 4
	call Common_EventScript_FerryDepartIsland
	return

BattleTower_Outside_EventScript_160250:: @ 8160250
	msgbox BattleTower_Outside_Text_199E29, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_16025A:: @ 816025A
	msgbox BattleTower_Outside_Text_199C6F, MSGBOX_SIGN
	end

BattleTower_Outside_EventScript_160263:: @ 8160263
	msgbox BattleTower_Outside_Text_199CA4, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_16026C:: @ 816026C
	msgbox BattleTower_Outside_Text_199E9C, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_160275:: @ 8160275
	msgbox BattleTower_Outside_Text_199ED4, MSGBOX_NPC
	end

BattleTower_Outside_EventScript_Scientist:: @ 816027D
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HOENN_STARTERS_BATTLE_TOWER, BattleTower_Outside_EventScript_Scientist_AlreadyReceived
	msgbox BattleTower_Outside_Text_ScientistGreeting, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BattleTower_Outside_EventScript_Scientist_Declined
	msgbox BattleTower_Outside_Text_ScientistGivingStarters, MSGBOX_DEFAULT
	compare VAR_STARTER_MON, 0
	goto_if_eq BattleTower_Outside_EventScript_Scientist_GiveTorchicMudkip
	compare VAR_STARTER_MON, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_GiveTreeckoMudkip
	compare VAR_STARTER_MON, 2
	goto_if_eq BattleTower_Outside_EventScript_Scientist_GiveTreeckoTorchic
	release
	end

BattleTower_Outside_EventScript_Scientist_AlreadyReceived:: @ 81602B5
	msgbox BattleTower_Outside_Text_ScientistAlreadyReceived, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_Scientist_Declined:: @ 81602BF
	msgbox BattleTower_Outside_Text_ScientistDeclined, MSGBOX_DEFAULT
	release
	end

BattleTower_Outside_EventScript_Scientist_GiveTorchicMudkip:: @ 81602C3
	playfanfare MUS_OBTAIN_ITEM
	bufferspeciesname 0, SPECIES_TORCHIC
	message BattleTower_Outside_Text_ScientistReceivedFirst
	waitfanfare
	waitmessage
	givemon SPECIES_TORCHIC, 10, ITEM_NONE
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_TorchicToPC
	goto BattleTower_Outside_EventScript_Scientist_GiveMudkip
	end

BattleTower_Outside_EventScript_Scientist_TorchicToPC:: @ 81602E5
	bufferspeciesname 0, SPECIES_TORCHIC
	msgbox BattleTower_Outside_Text_ScientistPartyFull, MSGBOX_DEFAULT
	goto BattleTower_Outside_EventScript_Scientist_GiveMudkip
	end

BattleTower_Outside_EventScript_Scientist_GiveMudkip:: @ 81602F5
	playfanfare MUS_OBTAIN_ITEM
	bufferspeciesname 0, SPECIES_MUDKIP
	message BattleTower_Outside_Text_ScientistReceivedSecond
	waitfanfare
	waitmessage
	givemon SPECIES_MUDKIP, 10, ITEM_NONE
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_MudkipToPC
	goto BattleTower_Outside_EventScript_Scientist_Complete
	end

BattleTower_Outside_EventScript_Scientist_MudkipToPC:: @ 8160317
	bufferspeciesname 0, SPECIES_MUDKIP
	msgbox BattleTower_Outside_Text_ScientistPartyFull, MSGBOX_DEFAULT
	goto BattleTower_Outside_EventScript_Scientist_Complete
	end

BattleTower_Outside_EventScript_Scientist_GiveTreeckoMudkip:: @ 8160327
	playfanfare MUS_OBTAIN_ITEM
	bufferspeciesname 0, SPECIES_TREECKO
	message BattleTower_Outside_Text_ScientistReceivedFirst
	waitfanfare
	waitmessage
	givemon SPECIES_TREECKO, 10, ITEM_NONE
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_TreeckoToPC
	goto BattleTower_Outside_EventScript_Scientist_GiveMudkip
	end

BattleTower_Outside_EventScript_Scientist_TreeckoToPC:: @ 8160349
	bufferspeciesname 0, SPECIES_TREECKO
	msgbox BattleTower_Outside_Text_ScientistPartyFull, MSGBOX_DEFAULT
	goto BattleTower_Outside_EventScript_Scientist_GiveMudkip
	end

BattleTower_Outside_EventScript_Scientist_GiveTreeckoTorchic:: @ 8160359
	playfanfare MUS_OBTAIN_ITEM
	bufferspeciesname 0, SPECIES_TREECKO
	message BattleTower_Outside_Text_ScientistReceivedFirst
	waitfanfare
	waitmessage
	givemon SPECIES_TREECKO, 10, ITEM_NONE
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_TreeckoToPC2
	goto BattleTower_Outside_EventScript_Scientist_GiveTorchic
	end

BattleTower_Outside_EventScript_Scientist_TreeckoToPC2:: @ 816037B
	bufferspeciesname 0, SPECIES_TREECKO
	msgbox BattleTower_Outside_Text_ScientistPartyFull, MSGBOX_DEFAULT
	goto BattleTower_Outside_EventScript_Scientist_GiveTorchic
	end

BattleTower_Outside_EventScript_Scientist_GiveTorchic:: @ 816038B
	playfanfare MUS_OBTAIN_ITEM
	bufferspeciesname 0, SPECIES_TORCHIC
	message BattleTower_Outside_Text_ScientistReceivedSecond
	waitfanfare
	waitmessage
	givemon SPECIES_TORCHIC, 10, ITEM_NONE
	compare VAR_RESULT, 1
	goto_if_eq BattleTower_Outside_EventScript_Scientist_TorchicToPC2
	goto BattleTower_Outside_EventScript_Scientist_Complete
	end

BattleTower_Outside_EventScript_Scientist_TorchicToPC2:: @ 81603AD
	bufferspeciesname 0, SPECIES_TORCHIC
	msgbox BattleTower_Outside_Text_ScientistPartyFull, MSGBOX_DEFAULT
	goto BattleTower_Outside_EventScript_Scientist_Complete
	end

BattleTower_Outside_EventScript_Scientist_Complete:: @ 81603BD
	setflag FLAG_RECEIVED_HOENN_STARTERS_BATTLE_TOWER
	msgbox BattleTower_Outside_Text_ScientistComplete, MSGBOX_DEFAULT
	release
	end
