
#include <sensors/sensor_main.h>
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>

/* Board Header files */
#include "Board.h"

#include "sensors/sensor_main.h"
#include "wireless/comm_lib.h"
#include "wireless/comm_main.h"
#include "display/menu.h"

#include "aasigotchi.h"

aasigotchi_data currentGotchi = {
	.name = "testi",
	.image = {
			0b00100100,
			0b00100100,
			0b01111110,
			0b10000001,
			0b10111101,
			0b10000001,
			0b10000001,
			0b01111110
	},
	.l = 0,
	.a = 0,
	.r = 0,
	.s = 0,
	.active = true
};

/*Globals*/
Event_Handle globalEvents = NULL;

/* Display */
Display_Handle hDisplay;

static PIN_Handle hButton0;
static PIN_State sButton0;
PIN_Config cButton0[] = {
    Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

static PIN_Handle hButton1;
static PIN_State sButton1;
PIN_Config cButton1[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

PIN_Config cPowerWake[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
    PIN_TERMINATE
};

/* Leds */
PIN_Handle hLed;
static PIN_State sLed;
PIN_Config cLed[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void ResetGotchi()
{
	memset(&currentGotchi, 0, sizeof(aasigotchi_data));
	currentGotchi.active = false;
}

void SetPowerOff(void)
{
    Display_clear(hDisplay);
    Display_close(hDisplay);
    Task_sleep(100 * 1000 / Clock_tickPeriod);

	PIN_close(hButton0);

    PINCC26XX_setWakeup(cPowerWake);

	Power_shutdown(NULL, 0);
}

Int main(void)
{
    // Initialize board
    Board_initGeneral();

	/* Buttons */
	hButton0 = PIN_open(&sButton0, cButton0);
	if (!hButton0) {
		System_abort("Error initializing button0.\n");
	}
	if (PIN_registerIntCb(hButton0, &Menu_OnButton0) != 0) {
		System_abort("Error registering button0 callback.\n");
	}

	hButton1 = PIN_open(&sButton1, cButton1);
	if(!hButton1) {
		System_abort("Error initializing button1.\n");
	}
	if (PIN_registerIntCb(hButton1, &Menu_OnButton1) != 0) {
		System_abort("Error registering button1 callback.");
	}

    /* Leds */
    hLed = PIN_open(&sLed, cLed);
    if(!hLed) {
        System_abort("Error initializing LED pin\n");
    }

    globalEvents = Event_create(NULL, NULL);
    if (globalEvents == NULL) {
    	System_abort("Event create failed.\n");
    }

    /* Task */

    Display_CreateTask();
    Comm_CreateTask();
    Sensors_CreateTask();

    System_printf("Ready!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
