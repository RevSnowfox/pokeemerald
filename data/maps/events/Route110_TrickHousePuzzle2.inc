Route110_TrickHousePuzzle2_MapObjects: @ 853E128
	object_event 1, MAP_OBJ_GFX_SCHOOL_KID_M, 0, 0, 13, 10, 3, 10, 0, 0, 1, 1, Route110_TrickHousePuzzle2_EventScript_26BC75, 0, 0, 0
	object_event 2, MAP_OBJ_GFX_SCHOOL_KID_M, 0, 0, 10, 17, 3, 8, 0, 0, 1, 4, Route110_TrickHousePuzzle2_EventScript_26BC8C, 0, 0, 0
	object_event 3, MAP_OBJ_GFX_GIRL_3, 0, 0, 11, 9, 3, 9, 0, 0, 1, 2, Route110_TrickHousePuzzle2_EventScript_26BCA3, 0, 0, 0
	object_event 4, MAP_OBJ_GFX_ITEM_BALL, 0, 0, 8, 17, 3, 1, 0, 0, 0, 0, Route110_TrickHousePuzzle2_EventScript_291428, 1062, 0, 0
	object_event 5, MAP_OBJ_GFX_ITEM_BALL, 0, 0, 3, 13, 3, 1, 0, 0, 0, 0, Route110_TrickHousePuzzle2_EventScript_29141B, 1061, 0, 0

Route110_TrickHousePuzzle2_MapWarps: @ 853E1A0
	warp_def 0, 21, 3, 2, ROUTE_110_TRICK_HOUSE_ENTRANCE
	warp_def 1, 21, 3, 2, ROUTE_110_TRICK_HOUSE_ENTRANCE
	warp_def 13, 1, 3, 0, ROUTE_110_TRICK_HOUSE_END

Route110_TrickHousePuzzle2_MapCoordEvents: @ 853E1B8
	coord_event 11, 12, 3, 0, VAR_0x4001, 0, 0, Route110_TrickHousePuzzle2_EventScript_26BBDD
	coord_event 0, 4, 0, 0, VAR_0x4002, 0, 0, Route110_TrickHousePuzzle2_EventScript_26BBF0
	coord_event 14, 5, 3, 0, VAR_0x4003, 0, 0, Route110_TrickHousePuzzle2_EventScript_26BC03
	coord_event 7, 11, 3, 0, VAR_0x4004, 0, 0, Route110_TrickHousePuzzle2_EventScript_26BC16

Route110_TrickHousePuzzle2_MapBGEvents: @ 853E1F8
	bg_event 14, 14, 0, 0, 0, Route110_TrickHousePuzzle2_EventScript_26BBC0

Route110_TrickHousePuzzle2_MapEvents:: @ 853E204
	map_events Route110_TrickHousePuzzle2_MapObjects, Route110_TrickHousePuzzle2_MapWarps, Route110_TrickHousePuzzle2_MapCoordEvents, Route110_TrickHousePuzzle2_MapBGEvents

