/*
 * menu.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include "menu.h"

#define DISPLAY_STACKSIZE 2048
Char displayStack[DISPLAY_STACKSIZE];

Display_Handle hDisplay;
//aasigotchi_data *currentGotchi = NULL;

tRectangle testRect = {
		.sXMin = 50,
		.sYMin = 50,
		.sXMax = 57,
		.sYMax = 57
};

const uint8_t imgdata_Gotchi[8] = {
		0b00100100,
		0b00100100,
		0b01111110,
		0b10000001,
		0b10111101,
		0b10000001,
		0b10000001,
		0b01111110
};

const uint8_t imgdata_Sun[8] = {
		0b00100100,
		0b00111100,
		0b11111111,
		0b01111110,
		0b01111110,
		0b11111111,
		0b00111100,
		0b00100100
};

const uint8_t imgdata_PhysicalActivity[8] = {
		0b11111111,
		0b10000001,
		0b10111101,
		0b10100101,
		0b10111101,
		0b10100101,
		0b10000001,
		0b11111111
};

const uint8_t imgdata_FreshAir[8] = {
		0b11111111,
		0b10000001,
		0b10111101,
		0b10100101,
		0b10111101,
		0b10100001,
		0b10000001,
		0b11111111
};


uint32_t imgPalette[] = {
		0xFFFFFF,
		0x000000
};

const tImage img_Gotchi = {
    .BPP = IMAGE_FMT_1BPP_UNCOMP,
    .NumColors = 2,
    .XSize = 1,
    .YSize = 8,
    .pPalette = imgPalette,
    .pPixel = imgdata_Gotchi
};

const tImage img_Sun = {
    .BPP = IMAGE_FMT_1BPP_UNCOMP,
    .NumColors = 2,
    .XSize = 1,
    .YSize = 8,
    .pPalette = imgPalette,
    .pPixel = imgdata_Sun
};

const tImage img_Physical = {
    .BPP = IMAGE_FMT_1BPP_UNCOMP,
    .NumColors = 2,
    .XSize = 1,
    .YSize = 8,
    .pPalette = imgPalette,
    .pPixel = imgdata_PhysicalActivity
};

const tImage img_FreshAir = {
    .BPP = IMAGE_FMT_1BPP_UNCOMP,
    .NumColors = 2,
    .XSize = 1,
    .YSize = 8,
    .pPalette = imgPalette,
    .pPixel = imgdata_FreshAir
};

tImage Menu_GetImageFromBitmap(tImage *img, const uint8_t *bitmap)
{
	img->BPP = IMAGE_FMT_1BPP_UNCOMP;
	img->NumColors = 2;
	img->XSize = 1;
	img->YSize = 8;
	img->pPalette = imgPalette;
	img->pPixel = bitmap;
}

#define MAINMENU_OPTIONS 4
static char mainMenu[MAINMENU_OPTIONS][12] = {
		                   ">Luo uusi",
						   " Hae/vie",
                           " Harrasta",
						   " Help"};

#define SUBMENU1_OPTIONS 5
static char subMenu1[SUBMENU1_OPTIONS][12] = {
		                   ">Paluu",
					       " Aurinko",
						   " Ilma",
                           " Liikunta",
						   " Kaverit"
};

unsigned int selectedItem = 0;
unsigned int selectedMenu = MENU_MAIN;

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id)
{
	System_printf("Button0 pressed.\n");
	System_flush();
	//PIN_setOutputValue(hLed, Board_LED0, !PIN_getOutputValue(Board_LED0));
	switch (selectedMenu) {
	case MENU_MAIN:
		mainMenu[selectedItem][0] = ' ';
		++selectedItem;
		if (selectedItem >= MAINMENU_OPTIONS)
			selectedItem = MM_OPT_CREATENEW;
		mainMenu[selectedItem][0] = '>';
		break;
	case MENU_SUB1:
		subMenu1[selectedItem][0] = ' ';
		++selectedItem;
		if (selectedItem >= SUBMENU1_OPTIONS)
			selectedItem = SM1_OPT_RETURN;
		subMenu1[selectedItem][0] = '>';
		break;
	}

	Event_post(g_hEvent, BUTTON_PRESSED);
}

static void Menu_UpdateState(void)
{
	switch (selectedMenu) {
	case MENU_MAIN:
		switch (selectedItem) {
		case MM_OPT_CREATENEW:
			break;
		case MM_OPT_LOAD:
			break;
		case MM_OPT_ACTIVITIES:
			selectedMenu = MENU_SUB1;
			selectedItem = 0;
			break;
		case MM_OPT_HELP:
			break;
		default:
			break;
		}
		break;
	case MENU_SUB1:
		switch (selectedItem) {
		case SM1_OPT_RETURN:
			System_printf("Paluu.\n");
			selectedMenu = MENU_MAIN;
			selectedItem = 0;
			break;
		case SM1_OPT_SUN:
			System_printf("Auringonottoa.\n");
			break;
		case SM1_OPT_FRESHAIR:
			System_printf("Raikasta ilmaa.\n");
			break;
		case SM1_OPT_PHYS:
			System_printf("Liikuntaa.\n");
			break;
		case SM1_OPT_HELLO:
			System_printf("Morjenstamista.\n");
			break;
		}
		break;
	}
	System_flush();
}

Void Menu_OnButton1(PIN_Handle handle, PIN_Id id)
{
	System_printf("Button1 pressed value: %i.\n", PIN_getOutputValue(Board_BUTTON1));
	System_flush();
	//PIN_setOutputValue(hLed, Board_LED1, !PIN_getOutputValue(Board_LED1));
	Menu_UpdateState();
	Event_post(g_hEvent, BUTTON_PRESSED);
}

Void DrawMainMenu(tContext *pContext)
{
	int i;
	Display_print0(hDisplay, 0, 6, "Menu");

	for (i = 0; i < MAINMENU_OPTIONS; ++i) {
		Display_print0(hDisplay, i+3, 2, mainMenu[i]);
	}
	GrLineDraw(pContext, 0, 58, 96, 58);
	Display_print0(hDisplay, 8, 0, currentGotchi->name);
	tImage test;
	Menu_GetImageFromBitmap(&test, currentGotchi->image);
	GrImageDraw(pContext, &test, 0, 9*8);
}

Void DrawSubMenu1(tContext *pContext)
{
	int i;
	Display_print0(hDisplay, 0, 2, "Harrastus");

	GrImageDraw(pContext, &img_Sun, 8, 48);
	Display_print1(hDisplay, 7, 1, "%i", currentGotchi->a);

	GrImageDraw(pContext, &img_Physical, 8, 64);
	Display_print1(hDisplay, 9, 1, "%i", currentGotchi->l);

	GrImageDraw(pContext, &img_FreshAir, 8, 80);
	Display_print1(hDisplay, 11, 1, "%i", currentGotchi->r);

	for (i = 0; i < SUBMENU1_OPTIONS; ++i) {
		Display_print0(hDisplay, i+1, 2, subMenu1[i]);
	}
}



Void DrawScreen(UArg arg0, UArg arg1)
{
	Display_Params displayParams;
	displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;
	Display_Params_init(&displayParams);

	hDisplay = Display_open(Display_Type_LCD, &displayParams);
	if (hDisplay == NULL) {
		System_abort("Error initializing Display\n");
	}

	Display_clear(hDisplay);

	while (1) {
		tContext *context = DisplayExt_getGrlibContext(hDisplay);
		Display_clear(hDisplay);
		if (context) {
			switch (selectedMenu) {
			case MENU_MAIN:
				DrawMainMenu(context);
				break;
			case MENU_SUB1:
				DrawSubMenu1(context);
				break;
			}
			GrFlush(context);
		}
		Event_pend(g_hEvent, Event_Id_NONE, DATA_CONVERSION_COMPLETE + BUTTON_PRESSED, BIOS_WAIT_FOREVER);
	}
}

Void Display_CreateTask()
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = DISPLAY_STACKSIZE;
	params.stack = &displayStack;
	params.priority = 2;

	task = Task_create(DrawScreen, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create display task!");
	}
}

Void Menu_Start()
{
	Display_CreateTask();
}

