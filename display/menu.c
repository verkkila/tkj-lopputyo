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
Menu *currentMenu = NULL;

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
		0b00000000,
		0b11111000,
		0b00000000,
		0b00111110,
		0b00000000,
		0b00111110,
		0b00000000,
		0b11111000
};

const uint8_t imgdata_FreshAir[8] = {
		0b00000000,
		0b01001001,
		0b01001001,
		0b10010010,
		0b10010010,
		0b01001001,
		0b01001001,
		0b00000000
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

void Menu_GetImageFromBitmap(tImage *img, const uint8_t *bitmap)
{
	img->BPP = IMAGE_FMT_1BPP_UNCOMP;
	img->NumColors = 2;
	img->XSize = 1;
	img->YSize = 8;
	img->pPalette = imgPalette;
	img->pPixel = bitmap;
}

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id)
{
	unsigned char selectedOption = currentMenu->selectedOption;
	if (!isLastOption(currentMenu->options[selectedOption + 1])) {
		currentMenu->selectedOption++;
	} else {
		currentMenu->selectedOption = 0;
	}
	Event_post(g_hEvent, BUTTON_PRESSED);
}

static void Menu_NextState(void)
{
	unsigned char selectedOption = currentMenu->selectedOption;
	if (currentMenu->options[selectedOption].action != NULL) {
		currentMenu->options[selectedOption].action();
	}
	if (currentMenu->options[selectedOption].next != NULL) {
		currentMenu = currentMenu->options[selectedOption].next;
	}
}

Void Menu_OnButton1(PIN_Handle handle, PIN_Id id)
{
	Menu_NextState();
	Event_post(g_hEvent, BUTTON_PRESSED);
}

Void DrawMainMenu(tContext *pContext)
{
	int i;
	unsigned char selectedOptionY = (currentMenu->selectedOption + 3) * 8 + 4;
	Display_print0(hDisplay, 0, 6, "Menu");

	for (i = 0; i < currentMenu->numOptions; ++i) {
		int x = 4;
		if (currentMenu->selectedOption == i)
			x++;
		Display_print0(hDisplay, i+3, x, currentMenu->options[i].text);
	}
	//GrLineDraw(pContext, 20, selectedOptionY, 26, selectedOptionY);

	GrLineDraw(pContext, 0, 58, 96, 58);
	Display_print0(hDisplay, 8, 0, currentGotchi->name);
	tImage test;
	Menu_GetImageFromBitmap(&test, currentGotchi->image);
	GrImageDraw(pContext, &test, 0, 9*8);
}

Void DrawActivitiesMenu(tContext *pContext)
{
	int i;
	unsigned char selectedOptionY = (currentMenu->selectedOption + 2) * 8 + 4;
	Display_print0(hDisplay, 0, 2, "Harrastus");

	Display_print1(hDisplay, 8, 3, "%i", currentGotchi->a);
	GrImageDraw(pContext, &img_Sun, 8, 8*8);

	Display_print1(hDisplay, 9, 3, "%i", currentGotchi->r);
	GrImageDraw(pContext, &img_FreshAir, 8, 9*8);

	Display_print1(hDisplay, 10, 4, "%i", currentGotchi->l);
	GrImageDraw(pContext, &img_Physical, 4, 10*8);
	tImage test;
	Menu_GetImageFromBitmap(&test, currentGotchi->image);
	GrImageDraw(pContext, &test, 12, 10*8);

	for (i = 0; i < currentMenu->numOptions; ++i) {
		int x = 3;
		if (currentMenu->selectedOption == i)
			x++;
		Display_print0(hDisplay, i+2, x, currentMenu->options[i].text);
	}
	//GrLineDraw(pContext, 10, selectedOptionY, 16, selectedOptionY);
}

Void DrawSunMenu(tContext *pContext)
{
	int i;
	//unsigned char selectedOptionY = (currentMenu->selectedOption + 6) * 8 + 4;
	for (i = 0; i < currentMenu->numOptions; ++i) {
		Display_print0(hDisplay, i, 1, currentMenu->options[i].text);
	}
	Display_print1(hDisplay, 6, 7, "%i", currentGotchi->a);
	//GrLineDraw(pContext, 0, 4, 6, 4);
	GrCircleDraw(pContext, 48, 48, 20);
	for (i = 0; i < 8; ++i) {
		int length = 10;
		if (i % 2 == 0)
			length += 5;
		int xClose = cos(i * 0.785398) * 20;
		int yClose = sin(i * 0.785398) * 20;

		int xFar = cos(i * 0.785398) * (20 + length);
		int yFar = sin(i * 0.785398) * (20 + length);
		GrLineDraw(pContext, xClose+48, yClose+48, xFar+48, yFar+48);
	}
}

void dummyFxn1(void)
{
	System_printf("Dummyfxn1 called.\n");
	System_flush();
}

void dummyFxn2(void)
{
	System_printf("Dummyfxn2 called.\n");
	System_flush();
}

void dummyFxn3(void)
{
	System_printf("Dummyfxn3 called.\n");
	System_flush();
}

void dummyFxn4(void)
{
	System_printf("Dummyfxn4 called.\n");
	System_flush();
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
	currentMenu = GetFirstMenu();

	while (1) {
		tContext *context = DisplayExt_getGrlibContext(hDisplay);
		Display_clear(hDisplay);
		if (context) {
			if (currentMenu != NULL) {
				currentMenu->drawFxn(context);
			}
			GrFlush(context);
		}
		Event_pend(g_hEvent, Event_Id_NONE, BUTTON_PRESSED + DATA_CONVERSION_COMPLETE, BIOS_WAIT_FOREVER);
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

