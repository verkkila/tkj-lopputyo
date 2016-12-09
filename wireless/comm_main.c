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
static char payload[PAYLOAD_LENGTH];
static Semaphore_Handle commUpdate;

static int shouldSend = 0;
static int lastMsgParsed = 1;

static uint16_t destinationAddress = IEEE80154_SINK_ADDR;

ReturnMsgParserFxn msgParser = NULL;

Bool CheckMsgOK(void)
{
	if (!strncmp(payload, "OK", 2)) {
		return true;
	}
	return false;
}

int GetErrorCode(void)
{
	int err = 0;
	sscanf(payload, "Virhe:%i:", &err);
	return err;
}

void HandleError(int errcode)
{
	switch (errcode) {
	case 1:
		System_printf("Error: Unknown command.\n");
		break;
	case 2:
		System_printf("Error: Invalid parameters.\n");
		break;
	case 3:
		System_printf("Error: Donkey exists.\n");
		break;
	case 4:
		System_printf("Error: Donkey not found on server. Try creating it?\n");
		break;
	default:
		System_printf("Bad error code.\n");
		break;
	}
	System_flush();
}

int ParseCreateMsg(void)
{
	if (CheckMsgOK()) {
		return 0;
	} else {
		return GetErrorCode();
	}
}

int ParseFetchMsg(void)
{
	if (CheckMsgOK()) {
		int scannedParams = sscanf(payload, "OK:%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%i,%i,%i,%i,%s", &currentGotchi.image[0],
																	 &currentGotchi.image[1],
																	 &currentGotchi.image[2],
																	 &currentGotchi.image[3],
																	 &currentGotchi.image[4],
																	 &currentGotchi.image[5],
																	 &currentGotchi.image[6],
																	 &currentGotchi.image[7],
																	 &currentGotchi.l,
																	 &currentGotchi.a,
																	 &currentGotchi.r,
																	 &currentGotchi.s,
																	 currentGotchi.name);
		if (scannedParams != 13) {
			System_printf("Did not receive all parameters.\n");
			System_flush();
		}
		currentGotchi.active = true;
		return 0;
	} else {
		return GetErrorCode();
	}
}

int ParseReturnMsg(void)
{
	if (CheckMsgOK()) {
		ResetGotchi();
		return 0;
	} else {
		return GetErrorCode();
	}
}

int Comm_TrackReceivedGreetings(void)
{
	if (strncmp(payload, "Terve:", 6))
		return 1;
	else {
		currentGotchi.s++;
		return 0;
	}
}

void Comm_StopTrackingGreetings(void)
{
	msgParser = NULL;
}

void TestAllReturnMessages(void)
{
	sprintf(payload, "OK:1,2,3,4,5,6,7,8,40,30,20,10,asdasd");
	//ParseCreateMsg();
	//ParseFetchMsg();
	ParseReturnMsg();
	/*
	sprintf(payload, "Virhe:1:malformed - unknown command");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	sprintf(payload, "Virhe:2:malformed - got x parameters - need y");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	sprintf(payload, "Virhe:2:malformed - missing colon after command");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	sprintf(payload, "Virhe:2:malformed - invalid parameters");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	sprintf(payload, "Virhe:3:donkey exists");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	sprintf(payload, "Virhe:4:donkey not found");
	HandleError(ParseCreateMsg());
	HandleError(ParseFetchMsg());
	HandleError(ParseReturnMsg());
	*/
}

void Comm_CreateNewGotchi(void)
{
	destinationAddress = IEEE80154_SINK_ADDR;
	sprintf(payload, "Uusi:%i,%i,%i,%i,%i,%i,%i,%i,%s\n", currentGotchi.image[0],
			currentGotchi.image[1],
			currentGotchi.image[2],
			currentGotchi.image[3],
			currentGotchi.image[4],
			currentGotchi.image[5],
			currentGotchi.image[6],
			currentGotchi.image[7],
			currentGotchi.name);
	Semaphore_post(commUpdate);
	shouldSend = 1;
	msgParser = ParseCreateMsg;
}

void Comm_FetchGotchi(void)
{
	sprintf(payload, "Leiki:testi\n");
	Semaphore_post(commUpdate);
	shouldSend = 1;
	msgParser = ParseFetchMsg;
}

void Comm_ReturnGotchi(void)
{
	sprintf(payload, "Nuku:%i,%i,%i,%i,%s\n", currentGotchi.l,
											  currentGotchi.a,
											  currentGotchi.r,
											  currentGotchi.s,
											  currentGotchi.name);
	Semaphore_post(commUpdate);
	shouldSend = 1;
	msgParser = ParseReturnMsg;
}

void Comm_FetchOrReturnGotchi(void)
{
	destinationAddress = IEEE80154_SINK_ADDR;
	if (currentGotchi.active) {
		Comm_ReturnGotchi();
	} else {
		Comm_FetchGotchi();
	}
}

Bool ReceivedGreeting(void)
{
	if (!strncmp(payload, "Terve:", 6))
		return false;
	return !strncmp(payload, "Terve", 5);
}

void Comm_SendGreeting(void)
{
	destinationAddress = IEEE80154_SINK_ADDR;
	shouldSend = 1;
	Semaphore_post(commUpdate);
	sprintf(payload, "Terve\n");
	msgParser = Comm_TrackReceivedGreetings;
}

Void Comm_Update(UArg arg0, UArg arg1)
{
	uint16_t senderAddr;

	Semaphore_Params commUpdateParams;
	Semaphore_Params_init(&commUpdateParams);
	commUpdate = Semaphore_create(0, &commUpdateParams, NULL);
	if (!commUpdate) {
		System_abort("Failed to create semaphore");
	}

	Init6LoWPAN();

	int32_t result = StartReceive6LoWPAN();
    if(result != true) {
    	System_abort("Wireless receive start failed");
    }

	while (1) {
		if (GetTXFlag() == false && shouldSend == 1 && lastMsgParsed == 1) {
			Send6LoWPAN(0xBEEA, (uint8_t*)payload, strlen(payload));
			StartReceive6LoWPAN();
			System_printf(payload);
			System_flush();
			shouldSend = 0;
			lastMsgParsed = 0;
		}
		if (GetRXFlag() == true) {
			Receive6LoWPAN(&senderAddr, payload, PAYLOAD_LENGTH);
			if (!ReceivedGreeting()) {
				int returnResult = 0;
				if (msgParser != NULL)
					returnResult = msgParser();
				if (returnResult == 0) {

				} else {
					HandleError(returnResult);
				}
			} else if (currentGotchi.active) {
				sprintf(payload, "Terve:%s\n", currentGotchi.name);
				destinationAddress = BROADCAST_ALL;
				shouldSend = 1;
			}
			lastMsgParsed = 1;
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
