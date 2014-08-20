#define NO_VAR 0
#define NO_CHILDREN NULL
#define NO_FUN NULL

struct brushWrapper* CREATE_BRUSH_WRAPPER( SDL_Keycode key, brushFun *brush, int brushVariant, struct SDLGUI_List *children) {
	struct brushWrapper *result = (struct brushWrapper*)malloc( sizeof( struct brushWrapper));
	result->key = key;
	result->brush = brush;
	result->brushVariant = brushVariant;
	result->children = children;
	return result;
}

#define CREATE_LIST_BUTTON( i, text, data) SDLGUI_Create_Text( 5, 5 + 35*i, 160, 30, &brushListItem_clicked, text, (int[4]){255,255,255,255}, (int[4]){0,0,0,255}, 6, 9, 1, data)
void initGui() {
	SDLGUI_Init( renderer, textures->font);
	bodyContainer = SDLGUI_Create_Panel( 0, 0, GUI_LEFTPANEL_WIDTH, 960, (int[4]){170,180,190,255}, (int[4]){100,100,100,255}, 4);
	SDLGUI_Add_Element( bodyContainer);
	
	struct SDLGUI_List *bodyItems = SDLGUI_Get_Panel_Elements( bodyContainer);
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 50, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonSave_clicked, "(s)ave", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));
	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 90, GUI_LEFTPANEL_WIDTH - 2*10, 30, &buttonQuit_clicked, "(q)uit", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	SDLGUI_List_Add( bodyItems, SDLGUI_Create_Text( 10, 200, GUI_LEFTPANEL_WIDTH - 2*10, 30, &brushBack_clicked, "(Tab) Back", (int[4]){0,0,0,0}, (int[4]){0,0,0,255}, 12, 20, 1, NULL));

	brushContainer = SDLGUI_Create_Panel( 10, 240, GUI_LEFTPANEL_WIDTH - 2*10, 300, (int[4]){0,0,0,255}, (int[4]){255,255,255,255}, 1);
	SDLGUI_List_Add( bodyItems, brushContainer);

	/* Table readable with tab-width 4 */
	brushList = SDLGUI_List_Create_From_Array(
		(struct SDLGUI_Element*[]){
			CREATE_LIST_BUTTON( 0, "1. rotate", CREATE_BRUSH_WRAPPER( SDLK_1, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) up"	, 				CREATE_BRUSH_WRAPPER( SDLK_1, &setDirection, dir_up, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) right", 			CREATE_BRUSH_WRAPPER( SDLK_2, &setDirection, dir_right, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) down", 				CREATE_BRUSH_WRAPPER( SDLK_3, &setDirection, dir_down,		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) left", 				CREATE_BRUSH_WRAPPER( SDLK_4, &setDirection, dir_left,		NO_CHILDREN)),
				}, 4
			))),
			CREATE_LIST_BUTTON( 1, "2. terrain", CREATE_BRUSH_WRAPPER( SDLK_2, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) ground", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawTerrain, terrain_gnd, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) wall", 	 			CREATE_BRUSH_WRAPPER( SDLK_2, &drawTerrain, terrain_wall, 	NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 2, "3. erase", CREATE_BRUSH_WRAPPER(/*key*/SDLK_3, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) object", 			CREATE_BRUSH_WRAPPER( SDLK_1, &eraseObject, NO_VAR, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) ai",	 			CREATE_BRUSH_WRAPPER( SDLK_2, &eraseAI, 	NO_VAR, 		NO_CHILDREN)),
				}, 2
			))),
			CREATE_LIST_BUTTON( 3, "4. object", CREATE_BRUSH_WRAPPER( SDLK_4, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) player", 			CREATE_BRUSH_WRAPPER( SDLK_1, &drawObject, go_player, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_2, &drawObject, go_leftTurner, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) apple", 			CREATE_BRUSH_WRAPPER( SDLK_3, &drawObject, go_apple, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) flower", 			CREATE_BRUSH_WRAPPER( SDLK_4, &drawObject, go_flower, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 4, "(5) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_5, &drawObject, go_creeperPlant, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 5, "(6) peekaboo monster", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawObject, go_peekaboo, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 6, "(7) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_7, &drawObject, ai_weepingAngel, NO_CHILDREN)),
				}, 7
			))),
			CREATE_LIST_BUTTON( 4, "5. ai", CREATE_BRUSH_WRAPPER( SDLK_5, NO_FUN, NO_VAR, /*children*/ SDLGUI_List_Create_From_Array( (struct SDLGUI_Element*[]){ 
					CREATE_LIST_BUTTON( 0, "(1) left turner", 		CREATE_BRUSH_WRAPPER( SDLK_1, &drawAI, ai_leftTurner, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 1, "(2) hungry left turner",CREATE_BRUSH_WRAPPER( SDLK_2, &drawAI, ai_hungryLeftTurner, NO_CHILDREN)),
					CREATE_LIST_BUTTON( 2, "(3) simple flower", 	CREATE_BRUSH_WRAPPER( SDLK_3, &drawAI, ai_simpleFlower, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 3, "(4) creeper plant", 	CREATE_BRUSH_WRAPPER( SDLK_4, &drawAI, ai_creeperPlant, 	NO_CHILDREN)),
					CREATE_LIST_BUTTON( 4, "(5) peek-a-boo chaser", CREATE_BRUSH_WRAPPER( SDLK_5, &drawAI, ai_peekaboo, 		NO_CHILDREN)),
					CREATE_LIST_BUTTON( 5, "(6) weeping angel", 	CREATE_BRUSH_WRAPPER( SDLK_6, &drawAI, ai_weepingAngel, 	NO_CHILDREN)),
				}, 6
			))),
		}, 5
	);
	SDLGUI_Set_Panel_Elements( brushContainer, brushList, true);
	
}
#undef CREATE_LIST_BUTTON


#undef NO_VAR
#undef NO_CHILDREN
#undef NO_FUN
