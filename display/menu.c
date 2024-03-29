/*
 * menu.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include "menu.h"

#define AON_BATMON_BASE 0x40095000
#define AON_BATMON_O_BAT 0x28

#define MAX_BAT_VOLTAGE 3.3f

#define DISPLAY_STACKSIZE 2048
Char displayStack[DISPLAY_STACKSIZE];

Display_Handle hDisplay;
Menu *currentMenu = NULL;
Clock_Handle redrawClock = NULL;
Clock_Handle decayClock = NULL;
Clock_Handle buttonClock = NULL;
int buttonHasReset = 1;

static int batteryPercentage = 0;

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

const uint8_t imgdata_Social[8] = {
		0b00000000,
		0b00000000,
		0b00100100,
		0b00100100,
		0b00000000,
		0b10000001,
		0b01111110,
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

const tImage img_Social = {
	.BPP = IMAGE_FMT_1BPP_UNCOMP,
	.NumColors = 2,
	.XSize = 1,
	.YSize = 8,
	.pPalette = imgPalette,
	.pPixel = imgdata_Social
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

Void Menu_RedrawTick(UArg arg)
{
	Event_post(globalEvents, UPDATE_SCREEN);
}

Void Menu_DecayAttributes(UArg arg)
{
	if (currentGotchi.active) {
		if (currentGotchi.a > 0) --currentGotchi.a;
		if (currentGotchi.r > 0) --currentGotchi.r;
		if (currentGotchi.l > 0) --currentGotchi.l;
		if (currentGotchi.s > 0) --currentGotchi.s;
	}
}

Void Menu_StartDecayingAttributes(void)
{
	Clock_start(decayClock);
}

Void Menu_StopDecayingAttributes(void)
{
	Clock_stop(decayClock);
}

Void Menu_StartRedrawing(void)
{
	Clock_start(redrawClock);
}

Void Menu_StopRedrawing(void)
{
	Clock_stop(redrawClock);
}

Void ResetButtons(UArg arg)
{
	buttonHasReset = 1;
	Clock_stop(buttonClock);
}

Void Menu_OnButton0(PIN_Handle handle, PIN_Id id)
{
	Clock_start(buttonClock);
	if (buttonHasReset) {
		unsigned char selectedOption = currentMenu->selectedOption;
		if (!isLastOption(currentMenu->options[selectedOption + 1])) {
			currentMenu->selectedOption++;
		} else {
			currentMenu->selectedOption = 0;
		}
		Event_post(globalEvents, BUTTON_PRESSED);
		buttonHasReset = 0;
	}
}

static void Menu_NextState(void)
{
	Clock_start(buttonClock);
	if (buttonHasReset){
		unsigned char selectedOption = currentMenu->selectedOption;
		int fn = 0;
		while (currentMenu->options[selectedOption].actions[fn] != NULL) {
			currentMenu->options[selectedOption].actions[fn]();
			++fn;
		}
		if (currentMenu->options[selectedOption].next != NULL) {
			currentMenu = currentMenu->options[selectedOption].next;
		}
		buttonHasReset = 0;
	}
}

Void Menu_OnButton1(PIN_Handle handle, PIN_Id id)
{
	Menu_NextState();
	Event_post(globalEvents, BUTTON_PRESSED);
}

Void DrawMainMenu(tContext *pContext)
{
	int i;
	Display_print1(hDisplay, 0, 6, "Menu  %i%%", batteryPercentage);

	for (i = 0; i < currentMenu->numOptions; ++i) {
		int x = 4;
		if (currentMenu->selectedOption == i)
			x++;
		Display_print0(hDisplay, i+3, x, currentMenu->options[i].text);
	}
	if (currentGotchi.active) {
		GrLineDraw(pContext, 0, 58, 96, 58);
		Display_print0(hDisplay, 8, 0, currentGotchi.name);
		tImage test;
		Menu_GetImageFromBitmap(&test, currentGotchi.image);
		GrImageDraw(pContext, &test, 0, 9*8);
	} else {
		GrLineDraw(pContext, 0, 58, 96, 58);
		Display_print0(hDisplay, 8, 0, "Ei aasia");
		GrLineDraw(pContext, 0, 9*8, 8, 9*8+8);
		GrLineDraw(pContext, 0, 9*8+8, 8, 9*8);
	}
}

Void DrawActivitiesMenu(tContext *pContext)
{
	int i;
	Display_print0(hDisplay, 0, 2, "Harrastus");

	Display_print1(hDisplay, 8, 4, "%i", currentGotchi.a);
	GrImageDraw(pContext, &img_Sun, 16, 8*8);

	Display_print1(hDisplay, 9, 4, "%i", currentGotchi.r);
	GrImageDraw(pContext, &img_FreshAir, 16, 9*8);

	Display_print1(hDisplay, 10, 4, "%i", currentGotchi.l);
	GrImageDraw(pContext, &img_Physical, 8, 10*8);
	tImage test;
	Menu_GetImageFromBitmap(&test, currentGotchi.image);
	GrImageDraw(pContext, &test, 16, 10*8);

	Display_print1(hDisplay, 11, 4, "%i", currentGotchi.s);
	GrImageDraw(pContext, &img_Social, 16, 11*8);

	for (i = 0; i < currentMenu->numOptions; ++i) {
		int x = 3;
		if (currentMenu->selectedOption == i)
			x++;
		Display_print0(hDisplay, i+2, x, currentMenu->options[i].text);
	}
}

Void DrawSunMenu(tContext *pContext)
{
	int i;
	static int length = 10;
	for (i = 0; i < currentMenu->numOptions; ++i) {
		Display_print0(hDisplay, i, 1, currentMenu->options[i].text);
	}
	Display_print1(hDisplay, 6, 7, "%i", currentGotchi.a);
	GrCircleDraw(pContext, 48, 48, 20);
	for (i = 0; i < 8; ++i) {
		int lengthReal = length;
		if (i % 2 == 0)
			lengthReal += 5;
		int xClose = cos(i * 0.785398) * 20;
		int yClose = sin(i * 0.785398) * 20;

		int xFar = cos(i * 0.785398) * (20 + lengthReal);
		int yFar = sin(i * 0.785398) * (20 + lengthReal);
		GrLineDraw(pContext, xClose+48, yClose+48, xFar+48, yFar+48);
	}
	length += 5;
	if (length > 15)
		length = 10;
}

Void DrawAirMenu(tContext *pContext)
{
	int i;
	static float offset = 0;
	for (i = 0; i < currentMenu->numOptions; ++i) {
		Display_print0(hDisplay, i, 1, currentMenu->options[i].text);
	}
	Display_print1(hDisplay, 1, 7, "%i", currentGotchi.r);

	GrCircleDraw(pContext, 36, 48, 5);
	for (i = 0; i < 3; ++i) {
		vec2f wing, wingSideLeft, wingSideRight;
		wing.x = cos((i * 2.0943) + offset);
		wing.y = sin((i * 2.0943) + offset);
		wingSideLeft.x = wing.y * 3;
		wingSideLeft.y = wing.x * -3;
		wingSideRight = vec2f_Mult(&wingSideLeft, -1.0f);
		vec2f vClose = vec2f_Mult(&wing, 5);
		vec2f vFar = vec2f_Mult(&wing, 30);
		vec2f halfWing = vec2f_Mult(&wing, 10);
		wingSideLeft.x += halfWing.x;
		wingSideLeft.y += halfWing.y;
		wingSideRight.x += halfWing.x;
		wingSideRight.y += halfWing.y;
		GrLineDraw(pContext, vClose.x+36, vClose.y+48, wingSideLeft.x+36, wingSideLeft.y+48);
		GrLineDraw(pContext, vClose.x+36, vClose.y+48, wingSideRight.x+36, wingSideRight.y+48);
		GrLineDraw(pContext, wingSideLeft.x+36, wingSideLeft.y+48, vFar.x+36, vFar.y+48);
		GrLineDraw(pContext, wingSideRight.x+36, wingSideRight.y+48, vFar.x+36, vFar.y+48);
	}
	offset += 0.20943;
	if (offset >= 2.0943)
		offset = 0;
}

Void DrawPhysMenu(tContext *pContext)
{
	int i;
	static int xPos = 8, xVel = 5;
	for (i = 0; i < currentMenu->numOptions; ++i) {
		Display_print0(hDisplay, i, 1, currentMenu->options[i].text);
	}
	Display_print1(hDisplay, 6, 3, "%i", currentGotchi.l);

	static int offset = 0;
	GrLineDraw(pContext, 0, 75, 96, 75);
	GrCircleDraw(pContext, xPos, 50-sin(-0.7854+(offset*0.20943))*10, 16);
	offset += 3;
	xPos += xVel;
	if (xPos > 80) {
		xVel = -5;
	} else if (xPos < 16) {
		xVel = 5;
	}
}

Void DrawSocialMenu(tContext *pContext)
{
	int i;
	for (i = 0; i < currentMenu->numOptions; ++i) {
		int x = 4;
		if (currentMenu->selectedOption == i)
			x++;
		Display_print0(hDisplay, i, x, currentMenu->options[i].text);
	}
	Display_print1(hDisplay, 6, 7, "%i", currentGotchi.s);

	GrCircleDraw(pContext, 48, 48, 30);
	GrLineDraw(pContext, 40, 38, 40, 43);
	GrLineDraw(pContext, 56, 38, 56, 43);


	static int offset  = -10;
	int xLeft = 33;
	int yLeft = 58 + offset;
	int xLeftMid = 38;
	int yLeftMid = 58;
	int xRightMid = 58;
	int yRightMid = 58;
	int xRight = 63;
	int yRight = 58 + offset;

	GrLineDraw(pContext, xLeft, yLeft, xLeftMid, yLeftMid);
	GrLineDraw(pContext, xLeftMid, yLeftMid, xRightMid, yRightMid);
	GrLineDraw(pContext, xRightMid, yRightMid, xRight, yRight);
	offset += 10;
	if (offset > 10)
		offset = -10;
}

Void CheckBattery()
{
	int i;
	uint32_t batteryReg = HWREG(AON_BATMON_BASE + AON_BATMON_O_BAT);
	float battery = (batteryReg & 0x00000700) >> 8;
	uint32_t frac = batteryReg & 0x000000FF;
	for (i = 7; i >= 0; --i) {
		battery += (frac >> i) & 1 ? pow2(-8 + i) : 0;
	}
	batteryPercentage = floor((battery / MAX_BAT_VOLTAGE) * 100);
}

Void DrawScreen(UArg arg0, UArg arg1)
{
	Clock_Params clockParams;
	Clock_Params decayClockParams;
	Clock_Params buttonClockParams;
	Display_Params displayParams;
	displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;
	Display_Params_init(&displayParams);

	hDisplay = Display_open(Display_Type_LCD, &displayParams);
	if (hDisplay == NULL) {
		System_abort("Error initializing Display\n");
	}

	Clock_Params_init(&clockParams);
	clockParams.period = 1000 * 1000 / Clock_tickPeriod;
	redrawClock = Clock_create(&Menu_RedrawTick, 0, &clockParams, NULL);
	if (redrawClock == NULL) {
		System_abort("...");
	}

	Clock_Params_init(&decayClockParams);
	decayClockParams.period = 60000 * 1000 / Clock_tickPeriod;
	decayClock = Clock_create(Menu_DecayAttributes, 60000, &decayClockParams, NULL);
	if (decayClock == NULL) {
		System_abort("...");
	}

	Clock_Params_init(&buttonClockParams);
	buttonClockParams.period = 400 * 1000 / Clock_tickPeriod;
	buttonClock = Clock_create(ResetButtons, 0, &buttonClockParams, NULL);
	if (buttonClock == NULL) {
		System_abort("...");
	}

	Clock_start(decayClock);

	Display_clear(hDisplay);
	currentMenu = GetFirstMenu();

	while (1) {
		CheckBattery();
		tContext *context = DisplayExt_getGrlibContext(hDisplay);
		if (context) {
			if (currentMenu != NULL) {
				currentMenu->drawFxn(context);
			}
			GrFlush(context);
		}
		Event_pend(globalEvents, Event_Id_NONE, BUTTON_PRESSED | DATA_CONVERSION_COMPLETE | UPDATE_SCREEN, BIOS_WAIT_FOREVER);
		Display_clear(hDisplay);
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
