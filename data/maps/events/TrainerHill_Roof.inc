TrainerHill_Roof_MapObjects: @ 853D9C4
	object_event 1, MAP_OBJ_GFX_GENTLEMAN, 0, 0, 12, 7, 3, 8, 17, 0, 0, 0, TrainerHill_Roof_EventScript_268FB2, 0, 0, 0

TrainerHill_Roof_MapWarps: @ 853D9DC
	warp_def 9, 5, 3, 1, TRAINER_HILL_4F
	warp_def 15, 5, 0, 1, TRAINER_HILL_ELEVATOR

TrainerHill_Roof_MapEvents:: @ 853D9EC
	map_events TrainerHill_Roof_MapObjects, TrainerHill_Roof_MapWarps, 0x0, 0x0

