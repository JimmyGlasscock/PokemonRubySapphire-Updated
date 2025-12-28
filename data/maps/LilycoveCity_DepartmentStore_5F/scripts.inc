LilycoveCity_DepartmentStore_5F_MapScripts:: @ 815A060
	.byte 0

LilycoveCity_DepartmentStore_5F_EventScript_15A061:: @ 815A061
	lock
	faceplayer
	message Text_HowMayIServeYou
	waitmessage
	pokemartdecoration LilycoveCity_DepartmentStore_5F_Dolls
	msgbox Text_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Dolls:: @ 815A078
	.2byte DECOR_PICHU_DOLL
	.2byte DECOR_PIKACHU_DOLL
	.2byte DECOR_MARILL_DOLL
	.2byte DECOR_TOGEPI_DOLL
	.2byte DECOR_CYNDAQUIL_DOLL
	.2byte DECOR_CHIKORITA_DOLL
	.2byte DECOR_TOTODILE_DOLL
	.2byte DECOR_JIGGLYPUFF_DOLL
	.2byte DECOR_MEOWTH_DOLL
	.2byte DECOR_CLEFAIRY_DOLL
	.2byte DECOR_DITTO_DOLL
	.2byte DECOR_SMOOCHUM_DOLL
	.2byte DECOR_TREECKO_DOLL
	.2byte DECOR_TORCHIC_DOLL
	.2byte DECOR_MUDKIP_DOLL
	.2byte DECOR_DUSKULL_DOLL
	.2byte DECOR_WYNAUT_DOLL
	.2byte DECOR_BALTOY_DOLL
	.2byte DECOR_KECLEON_DOLL
	.2byte DECOR_AZURILL_DOLL
	.2byte DECOR_SKITTY_DOLL
	.2byte DECOR_SWABLU_DOLL
	.2byte DECOR_GULPIN_DOLL
	.2byte DECOR_LOTAD_DOLL
	.2byte DECOR_SEEDOT_DOLL
	.2byte DECOR_SNORLAX_DOLL
	.2byte DECOR_RHYDON_DOLL
	.2byte DECOR_LAPRAS_DOLL
	.2byte DECOR_VENUSAUR_DOLL
	.2byte DECOR_CHARIZARD_DOLL
	.2byte DECOR_BLASTOISE_DOLL
	.2byte DECOR_WAILMER_DOLL
	.2byte DECOR_REGIROCK_DOLL
	.2byte DECOR_REGICE_DOLL
	.2byte DECOR_REGISTEEL_DOLL
	.2byte DECOR_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A094:: @ 815A094
	lock
	faceplayer
	message Text_HowMayIServeYou
	waitmessage
	pokemartdecoration LilycoveCity_DepartmentStore_5F_Posters
	msgbox Text_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Posters:: @ 815A0AC
	.2byte DECOR_BALL_POSTER
	.2byte DECOR_GREEN_POSTER
	.2byte DECOR_RED_POSTER
	.2byte DECOR_BLUE_POSTER
	.2byte DECOR_CUTE_POSTER
	.2byte DECOR_PIKA_POSTER
	.2byte DECOR_LONG_POSTER
	.2byte DECOR_SEA_POSTER
	.2byte DECOR_SKY_POSTER
	.2byte DECOR_KISS_POSTER
	.2byte DECOR_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A0C2:: @ 815A0C2
	lock
	faceplayer
	message Text_HowMayIServeYou
	waitmessage
	pokemartdecoration2 LilycoveCity_DepartmentStore_5F_Items3
	msgbox Text_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Items3:: @ 815A0DC
	.2byte ITEM_CARBOS
	.2byte ITEM_CALCIUM
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_PP_UP
	.2byte ITEM_ZINC
	.2byte ITEM_PP_MAX
	.2byte 72
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A0F2:: @ 815A0F2
	lock
	faceplayer
	message Text_HowMayIServeYou
	waitmessage
	pokemartdecoration LilycoveCity_DepartmentStore_5F_Mats
	msgbox Text_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_Mats:: @ 815A10C
	.2byte DECOR_GLITTER_MAT
	.2byte DECOR_JUMP_MAT
	.2byte DECOR_SPIN_MAT
	.2byte DECOR_C_LOW_NOTE_MAT
	.2byte DECOR_D_NOTE_MAT
	.2byte DECOR_E_NOTE_MAT
	.2byte DECOR_F_NOTE_MAT
	.2byte DECOR_G_NOTE_MAT
	.2byte DECOR_A_NOTE_MAT
	.2byte DECOR_B_NOTE_MAT
	.2byte DECOR_C_HIGH_NOTE_MAT
	.2byte DECOR_SURF_MAT
	.2byte DECOR_THUNDER_MAT
	.2byte DECOR_FIRE_BLAST_MAT
	.2byte DECOR_POWDER_SNOW_MAT
	.2byte DECOR_ATTRACT_MAT
	.2byte DECOR_FISSURE_MAT
	.2byte DECOR_SPIKES_MAT
	.2byte DECOR_NONE
	release
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A124:: @ 815A124
	msgbox LilycoveCity_DepartmentStore_5F_Text_18C4F9, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A12D:: @ 815A12D
	msgbox LilycoveCity_DepartmentStore_5F_Text_18C59F, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A136:: @ 815A136
	msgbox LilycoveCity_DepartmentStore_5F_Text_18C556, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_5F_EventScript_15A149:: @ 815A149
	lock
	faceplayer
	message Text_HowMayIServeYou
	waitmessage
	pokemartdecoration LilycoveCity_DepartmentStore_5F_OtherDecorations
	msgbox Text_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_5F_OtherDecorations:: @ 815A162
	.2byte DECOR_SMALL_DESK
	.2byte DECOR_POKEMON_DESK
	.2byte DECOR_HEAVY_DESK
	.2byte DECOR_RAGGED_DESK
	.2byte DECOR_COMFORT_DESK
	.2byte DECOR_PRETTY_DESK
	.2byte DECOR_BRICK_DESK
	.2byte DECOR_CAMP_DESK
	.2byte DECOR_HARD_DESK
	.2byte DECOR_SMALL_CHAIR
	.2byte DECOR_POKEMON_CHAIR
	.2byte DECOR_HEAVY_CHAIR
	.2byte DECOR_PRETTY_CHAIR
	.2byte DECOR_COMFORT_CHAIR
	.2byte DECOR_RAGGED_CHAIR
	.2byte DECOR_BRICK_CHAIR
	.2byte DECOR_CAMP_CHAIR
	.2byte DECOR_HARD_CHAIR
	.2byte DECOR_RED_PLANT
	.2byte DECOR_TROPICAL_PLANT
	.2byte DECOR_PRETTY_FLOWERS
	.2byte DECOR_COLORFUL_PLANT
	.2byte DECOR_BIG_PLANT
	.2byte DECOR_GORGEOUS_PLANT
	.2byte DECOR_RED_BRICK
	.2byte DECOR_YELLOW_BRICK
	.2byte DECOR_BLUE_BRICK
	.2byte DECOR_RED_BALLOON
	.2byte DECOR_BLUE_BALLOON
	.2byte DECOR_YELLOW_BALLOON
	.2byte DECOR_RED_TENT
	.2byte DECOR_BLUE_TENT
	.2byte DECOR_SOLID_BOARD
	.2byte DECOR_SLIDE
	.2byte DECOR_FENCE_LENGTH
	.2byte DECOR_FENCE_WIDTH
	.2byte DECOR_TIRE
	.2byte DECOR_STAND
	.2byte DECOR_MUD_BALL
	.2byte DECOR_BREAKABLE_DOOR
	.2byte DECOR_SAND_ORNAMENT
	.2byte DECOR_SILVER_SHIELD
	.2byte DECOR_GOLD_SHIELD
	.2byte DECOR_GLASS_ORNAMENT
	.2byte DECOR_TV
	.2byte DECOR_ROUND_TV
	.2byte DECOR_CUTE_TV
	.2byte DECOR_NONE
	release
	end
