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

/*
0000 0000
0010 0100
0011 1100
0100 0010
0100 0010
0100 0010
0011 1100
*/

const uint8_t imgData[8] = {
   0x00, 0x24, 0x3B, 0x42, 0x42, 0x42, 0x3B, 0x00
};

const uint8_t imgData2[8] = {
		0b00000000,
		0b00100100,
		0b00111100,
		0b01000010,
		0b01000010,
		0b01000010,
		0b00111100
};

uint32_t imgPalette[] = {
		0xFFFFFF,
		0x000000
};

const tImage image = {
    .BPP = IMAGE_FMT_1BPP_UNCOMP,
    .NumColors = 2,
    .XSize = 1,
    .YSize = 8,
    .pPalette = imgPalette,
    .pPixel = imgData2
};

Void DrawMainMenu(tContext *pContext)
{
	Display_print0(hDisplay, 0, 2, "Menu");
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
		Event_pend(g_hEvent, DATA_READ_COMPLETE, Event_Id_NONE, BIOS_WAIT_FOREVER);
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

