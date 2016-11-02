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

Void DrawMainMenu(tContext *pContext)
{

	Display_print0(hDisplay, 0, 6, "Menu");

	GrImageDraw(pContext, &img_Sun, 8, 64);

	GrImageDraw(pContext, &img_Physical, 8, 72);

	GrImageDraw(pContext, &img_FreshAir, 8, 80);

	/*
	Display_print0(hDisplay, 0, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 1, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 2, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 3, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 4, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 5, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 6, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 7, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 8, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 9, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 10, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 11, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 12, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 13, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 14, 0, "0123456789ABCDEF");
	Display_print0(hDisplay, 15, 0, "0123456789ABCDEF");
	*/
	//GrRectFill(pContext, &testRect);
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
		if (context) {
			DrawMainMenu(context);
			GrFlush(context);
		}
		Event_pend(g_hEvent, DATA_CONVERSION_COMPLETE, Event_Id_NONE, BIOS_WAIT_FOREVER);
		//Display_print0(hDisplay, 0, 0, sensorDisplayData.TMP007);
		//Display_print0(hDisplay, 1, 0, sensorDisplayData.OPT3001);
		//Display_print0(hDisplay, 2, 0, sensorDisplayData.BMP280);
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

