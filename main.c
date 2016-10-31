
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

#include "util/math.h"

/* Board Header files */
#include "Board.h"

#include "wireless/comm_lib.h"
#include "sensors/bmp280.h"
#include "sensors/tmp007.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"

/* Task */
#define STACKSIZE 2048
Char sensorTaskStack[STACKSIZE];
Char commTaskStack[STACKSIZE];
Char displayTaskStack[STACKSIZE];

struct sensor_display_data {
	char TMP007[16];
	char OPT3001[16];
	char BMP280[16];
};

struct sensor_display_data sensorDisplayData;
Event_Handle evt;

/* Display */
Display_Handle hDisplay;

/* Pin Button1 configured as power button */
static PIN_Handle hPowerButton;
static PIN_State sPowerButton;
PIN_Config cPowerButton[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};
PIN_Config cPowerWake[] = {
    Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
    PIN_TERMINATE
};

/* Pin Button0 configured */
static PIN_Handle hButton0;
static PIN_State sButton0;
PIN_Config cButton0[] = {
    Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

/* Leds */
static PIN_Handle hLed;
static PIN_State sLed;
PIN_Config cLed[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/* Handle power button */
Void powerButtonFxn(PIN_Handle handle, PIN_Id pinId)
{
    Display_clear(hDisplay);
    Display_close(hDisplay);
    Task_sleep(100000 / Clock_tickPeriod);

	PIN_close(hPowerButton);

    PINCC26XX_setWakeup(cPowerWake);
	Power_shutdown(NULL,0);
}

Void ledButtonFxn(PIN_Handle handle, PIN_Id pinId)
{
	PIN_setOutputValue(hLed, Board_LED0, !PIN_getOutputValue(Board_LED0));
}

/* Communication Task */
Void CommTask(UArg arg0, UArg arg1)
{
	char		payload[16];
	uint16_t	sender;

    // Radio to receive mode
	int32_t result = StartReceive6LoWPAN();
	if(result != true) {
		System_abort("Wireless receive mode failed");
	}
	sprintf(payload, "Hello world!");
	Send6LoWPAN(IEEE80154_SINK_ADDR, payload, 16);
	StartReceive6LoWPAN();
	// YOUR CODE HERE TO SEND EXAMPLE MESSAGE

    while (1) {
    	if (GetRXFlag() == true) {
            if (Receive6LoWPAN(&sender, payload, 16) != -1) {
            	System_printf("%s\n", payload);
            	System_flush();
            }
            // WE HAVE A MESSAGE
            // YOUR CODE HERE TO RECEIVE MESSAGE
        }

    	// THIS WHILE LOOP DOES NOT USE Task_sleep
    }
}

Void InitCommTask()
{
	Task_Handle task;
	Task_Params params;

	Init6LoWPAN();
	Task_Params_init(&params);
	params.stackSize = STACKSIZE;
	params.stack = &commTaskStack;
	params.priority = 1;

	task = Task_create(CommTask, &params, NULL);
	if (task == NULL) {
		System_abort("Task create failed!");
	}
}

Void SensorTask(UArg arg0, UArg arg1)
{
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    
    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    } else {
        System_printf("I2C Initialized!\n");
    }
    BMP280_Setup(&i2c);
    TMP007_Setup(&i2c);
    OPT3001_Setup(&i2c);

    while (1) {
    	float temp = TMP007_GetTemperature(&i2c);
		float lum = OPT3001_GetLuminosity(&i2c);
		float pressure = BMP280_GetPressure(&i2c);
		sprintf(sensorDisplayData.TMP007, "T:%.2f", temp);
		sprintf(sensorDisplayData.OPT3001, "L:%.2f", lum);
		sprintf(sensorDisplayData.BMP280, "P:%.2f", pressure);
		Event_post(evt, Event_Id_00);
		Task_sleep(500000 / Clock_tickPeriod);
    }
}

Void InitSensorTask()
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = STACKSIZE;
	params.stack = &sensorTaskStack;
	params.priority = 3;

	task = Task_create(SensorTask, &params, NULL);
	if (task == NULL) {
		System_abort("Task create failed!");
	}
}

Void DisplayTask(UArg arg0, UArg arg1)
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
		Event_pend(evt, Event_Id_00, Event_Id_NONE, BIOS_WAIT_FOREVER);
		Display_print0(hDisplay, 0, 0, sensorDisplayData.TMP007);
		Display_print0(hDisplay, 1, 0, sensorDisplayData.OPT3001);
		Display_print0(hDisplay, 2, 0, sensorDisplayData.BMP280);
	}
}

Void InitDisplayTask()
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = STACKSIZE;
	params.stack = &displayTaskStack;
	params.priority = 2;

	task = Task_create(DisplayTask, &params, NULL);
	if (task == NULL) {
		System_abort("Task create failed!");
	}
}

Int main(void)
{
    // Initialize board
    Board_initGeneral();

	/* Buttons */
	hPowerButton = PIN_open(&sPowerButton, cPowerButton);
	if(!hPowerButton) {
		System_abort("Error initializing button shut pins\n");
	}
	if (PIN_registerIntCb(hPowerButton, &powerButtonFxn) != 0) {
		System_abort("Error registering button callback function");
	}

	hButton0 = PIN_open(&sButton0, cButton0);
	if (!hButton0) {
		System_abort("Error initializing button0.\n");
	}
	if (PIN_registerIntCb(hButton0, &ledButtonFxn) != 0) {
		System_abort("Error registering button0 callback.\n");
	}

    /* Leds */
    hLed = PIN_open(&sLed, cLed);
    if(!hLed) {
        System_abort("Error initializing LED pin\n");
    }

    evt = Event_create(NULL, NULL);
    if (evt == NULL) {
    	System_abort("Event create failed.\n");
    }

    /* Task */
    InitSensorTask();
    InitDisplayTask();
    InitCommTask();

    System_printf("Ready!\n");
    System_flush();
    
    /* Start BIOS */
    BIOS_start();

    return (0);
}
