/*
 * comm_main.c
 *
 *  Created on: Oct 31, 2016
 *      Author: Valtteri
 */

#include "comm_main.h"

#define COMM_STACKSIZE 2048
Char commStack[COMM_STACKSIZE];

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

Void Comm_CreateTask()
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = COMM_STACKSIZE;
	params.stack = &commStack;
	params.priority = 1;

	task = Task_create(CommTask, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create comm task!");
	}
}

Void Comm_Start()
{
	Comm_CreateTask();
}
