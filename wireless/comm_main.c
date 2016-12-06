/*
 * comm_main.c
 *
 *  Created on: Dec 5, 2016
 *      Author: tsei
 */

#include "comm_main.h"

#define COMM_STACKSIZE 4096
Char commStack[COMM_STACKSIZE];

#define PAYLOAD_LENGTH 80
char payload[PAYLOAD_LENGTH];
Semaphore_Handle sem;

int shouldSend = 0;
int lastMsgParsed = 1;

void Comm_CreateNewGotchi(void)
{
	sprintf(payload, "Uusi:%i,%i,%i,%i,%i,%i,%i,%i,%s\n", currentGotchi->image[0],
			currentGotchi->image[1],
			currentGotchi->image[2],
			currentGotchi->image[3],
			currentGotchi->image[4],
			currentGotchi->image[5],
			currentGotchi->image[6],
			currentGotchi->image[7],
			currentGotchi->name);
	//Semaphore_post(sem);
	shouldSend = 1;
}

void Comm_FetchGotchi(void)
{
	sprintf(payload, "Hae:%s\n", currentGotchi->name);
	//Semaphore_post(sem);
	shouldSend = 1;
}

void Comm_ParseReturnMsg(void)
{
	char result[16];
	sscanf(payload, "%s", result);
	if (!strcmp(result, "OK")) {
		System_printf("Viesti ok");
	} else if (!strcmp(result, "Virhe")) {
		System_printf("Viestissä virhe");
	}
	System_flush();
}

Void Comm_Update(UArg arg0, UArg arg1)
{
	uint16_t senderAddr;
	Semaphore_Params semParams;
	Semaphore_Params_init(&semParams);
	sem = Semaphore_create(0, &semParams, NULL);
	if (!sem) {
		System_abort("Failed to create semaphore");
	}

	int32_t result = StartReceive6LoWPAN();
    if(result != true) {
    	System_abort("Wireless receive start failed");
    }

	while (1) {
		//Semaphore_pend(sem, BIOS_WAIT_FOREVER);
		if (GetTXFlag() == false && shouldSend == 1 && lastMsgParsed == 1) {
			Send6LoWPAN(IEEE80154_SINK_ADDR, (uint8_t*)payload, strlen(payload));
			StartReceive6LoWPAN();
			shouldSend = 0;
		} else if (GetRXFlag() == true) {
			if (Receive6LoWPAN(&senderAddr, payload, PAYLOAD_LENGTH) != -1) {
				Comm_ParseReturnMsg();
			}
		}
	}
}

Void Comm_CreateTask(void)
{
	Task_Handle task;
	Task_Params params;

	Task_Params_init(&params);
	params.stackSize = COMM_STACKSIZE;
	params.stack = &commStack;
	params.priority = 1;

	task = Task_create(Comm_Update, &params, NULL);
	if (task == NULL) {
		System_abort("Failed to create comm task!");
	}
}


void Comm_Start(void)
{
	Init6LoWPAN();
	Comm_CreateTask();
}
