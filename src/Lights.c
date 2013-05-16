//!
//! @file 		Lights.c
//! @author 	Geoffrey Hunter <gbmhunter@gmail.com> (www.cladlab.com)
//! @edited 	n/a
//! @date 		09/11/2012
//! @brief 		See Lights.h
//! @details
//!		<b>Last Modified:			</b> 07/11/2012					\n
//!		<b>File Version:			</b> 							\n
//!		<b>Company:					</b> CladLabs					\n
//!		<b>Project:					</b> Free Code Modules			\n
//!		<b>Language:				</b> C							\n
//!		<b>Compiler:				</b> GCC						\n
//! 	<b>uC Model:				</b> PSoC5						\n
//!		<b>Computer Architecture:	</b> ARM						\n
//! 	<b>Operating System:		</b> FreeRTOS v7.2.0			\n
//!		<b>Documentation Format:	</b> Doxygen					\n
//!		<b>License:					</b> GPLv3						\n
//!		
//!		See README.rst, generate Doxygen documentation or see Lights.h for more information on this module.	
//!

//===============================================================================================//
//========================================= INCLUDES ============================================//
//===============================================================================================//

// PSoC
#include <device.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// User
#include "PublicDefinesAndTypeDefs.h"
#include "Config.h"
#include "Lights.h"
#include "UartDebug.h"

//===============================================================================================//
//============================================ GUARDS ===========================================//
//===============================================================================================//

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef configENABLE_TASK_LIGHTS
	#error Please define the switch configENABLE_TASK_LIGHTS
#endif
#ifndef configPRINT_DEBUG_LIGHTS
	#error Please define the switch configPRINT_DEBUG_LIGHTS
#endif

//===============================================================================================//
//=================================== PRIVATE TYPEDEF's =========================================//
//===============================================================================================//

typedef enum
{
	ST_SWITCH_NULL,
	ST_SWITCH_LIGHT_OFF,
	ST_SWITCH_LIGHT_FLASHING_GREEN,
	ST_SWITCH_LIGHT_FLASHING_ORANGE
} switchLightState_t;

typedef struct
{
	switchLightState_t 	switchLightState;
	switchLightState_t 	prevSwitchLightState;
	uint16 				switchLightFlashRateMs;		//!< Flash rate in ms. Set to 0 to keep the led continuously on
	uint16				switchLightFlashTimeMs;		//!< Total length of time of keep light in set state before reverting back. Set to 0 for indefinite.
	bool_t				timedState;
	uint16				stateStartCount;
} lightsParameters_t;

lightsParameters_t _lightsParameters = 
{
	.switchLightState = ST_SWITCH_LIGHT_OFF,
	.prevSwitchLightState = ST_SWITCH_LIGHT_OFF,
	.switchLightFlashRateMs = 0,
	.switchLightFlashTimeMs = 0,
	.timedState = FALSE,
	.stateStartCount = 0
};

lightsParameters_t _prevLightsParameters = 
{
	.switchLightState = ST_SWITCH_LIGHT_OFF,
	.prevSwitchLightState = ST_SWITCH_LIGHT_OFF,
	.switchLightFlashRateMs = 0,
	.switchLightFlashTimeMs = 0,
	.timedState = FALSE,
	.stateStartCount = 0
};

//===============================================================================================//
//==================================== PRIVATE VARIABLES ========================================//
//===============================================================================================//

static xTaskHandle _lightsTaskHandle = 0;			//!< Handle for the lights task
static xQueueHandle _lightsTaskCommandQueue;		//!< Used by the lights task to receive commands from other tasks

//===============================================================================================//
//====================================  FUNCTION PROTOTYPES =====================================//
//===============================================================================================//

// See function definitions for descriptions

void Lights_Task(void *pvParameters);

//===============================================================================================//
//===================================== PUBLIC FUNCTIONS ========================================//
//===============================================================================================//

// See the Doxygen documentation or the function declarations in Lights.h for more information

void Lights_Start(uint32 lightsTaskStackSize, uint8 lightsTaskPriority)
{
	#if(configENABLE_TASK_LIGHTS == 1)
		// Create the main task
		xTaskCreate(	&Lights_Task,
						(signed portCHAR *) "Light Task",
						lightsTaskStackSize,
						NULL,
						lightsTaskPriority,
						&_lightsTaskHandle);
	#endif
	
	// Create incoming command queue, used to send
	// commands to the BLDC task
	_lightsTaskCommandQueue = xQueueCreate(configLIGHTS_COMMAND_QUEUE_LENGTH, sizeof(lightsCommandStruct_t));
	
}


void Lights_SendCommandToTask(lightsCommandWord_t lightCommandWord, uint8 value1, uint8 value2)
{
	lightsCommandStruct_t lightsCommandStruct = 
	{
		.commandWord = lightCommandWord,
		.value1 = value1,
		.value2 = value2
	};
	xQueueSendToBack(_lightsTaskCommandQueue, &lightsCommandStruct , configMAX_QUEUE_WAIT_TIME_MS_BLDC_TASK/portTICK_RATE_MS);
}


//===============================================================================================//
//==================================== PRIVATE FUNCTIONS ========================================//
//===============================================================================================//

//! @brief 		Light task.
//! @param		*pvParameters Void pointer (not used)
//! @note		Not thread-safe. Do not call from any task, this function is a task that
//!				is called by the FreeRTOS kernel
//! @private
void Lights_Task(void *pvParameters)
{
	#if(configPRINT_DEBUG_LIGHTS == 1)
		UartDebug_PutString("LIGHTS: Lights task has started.\r\n");
	#endif

	lightsCommandStruct_t lightsCommandStruct = { 0, 0, 0};		//!< Variable to store received command from queue
	//lightsCommandStruct_t prevLightsCommandStruct = { 0, 0, 0};		//!< Variable to store received command from queue

	uint32_t loopCounter = 0;
	uint32_t lastSwitchFlashCount = 0;
	
	// Infinite task loop
	for(;;)
	{
		// Task waits untill it is time to update light config
		vTaskDelay(configLIGHTS_TASK_UPDATE_RATE_MS/portTICK_RATE_MS);
		
		// Set command to NULL
		memset(&lightsCommandStruct, 0x00, sizeof(lightsCommandStruct));
		
		// Check for a command (don't wait)
		portBASE_TYPE xResult = xQueueReceive(_lightsTaskCommandQueue, &lightsCommandStruct , 0);
		
		if(xResult == pdPASS)
		{
		
			// Check to see if command is only temporary (e.g. a specified time)
			if(lightsCommandStruct.value2 != 0)
			{
				// Copy existing state so it can be replaced at timeout
				memcpy(&_prevLightsParameters, &_lightsParameters, sizeof(_prevLightsParameters));
				_lightsParameters.timedState = TRUE;
				_lightsParameters.switchLightFlashTimeMs = lightsCommandStruct.value2;
				_lightsParameters.stateStartCount = loopCounter;
			}
			else
			{
				_lightsParameters.timedState = FALSE;
			}
			
			// Command was received. Process command.
			
			if(lightsCommandStruct.commandWord == CMD_SWITCH_LIGHT_OFF)
			{
				_lightsParameters.switchLightState = ST_SWITCH_LIGHT_FLASHING_GREEN;
			}
			
			if(lightsCommandStruct.commandWord == CMD_SWITCH_LIGHT_FLASH_GREEN)
			{
				_lightsParameters.switchLightState = ST_SWITCH_LIGHT_FLASHING_GREEN;
			}
			
			if(lightsCommandStruct.commandWord == CMD_SWITCH_LIGHT_FLASH_ORANGE)
			{
				_lightsParameters.switchLightState = ST_SWITCH_LIGHT_FLASHING_ORANGE;
			}
		}
		
		if(_lightsParameters.timedState == TRUE)
		{
			if(loopCounter >= _lightsParameters.stateStartCount + _lightsParameters.switchLightFlashTimeMs/(portTICK_RATE_MS*configLIGHTS_TASK_UPDATE_RATE_MS))
			{
				// State has timed out, revert back to original state
				memcpy(&_lightsParameters, &_prevLightsParameters, sizeof(_lightsParameters));
				_lightsParameters.timedState = FALSE;
				xResult = pdPASS;
			}
		}
		
		switch(_lightsParameters.switchLightState)
		{
			case ST_SWITCH_LIGHT_OFF:
				if(_lightsParameters.prevSwitchLightState != _lightsParameters.switchLightState || xResult == pdPASS)
				{
					PinCpSwitchLed1_Write(0);
					PinCpSwitchLed2_Write(0);
					
					lastSwitchFlashCount = loopCounter;
					_lightsParameters.prevSwitchLightState = _lightsParameters.switchLightState;
				}
				break;
				
			case ST_SWITCH_LIGHT_FLASHING_GREEN:
				if(_lightsParameters.prevSwitchLightState != _lightsParameters.switchLightState || xResult == pdPASS)
				{
					_lightsParameters.switchLightFlashRateMs = lightsCommandStruct.value1;
					
					PinCpSwitchLed1_Write(1);
					PinCpSwitchLed2_Write(0);
					
					lastSwitchFlashCount = loopCounter;
					
					_lightsParameters.prevSwitchLightState = _lightsParameters.switchLightState;
				}
				if(_lightsParameters.switchLightFlashRateMs != 0)
				{
					if(loopCounter >= lastSwitchFlashCount + (_lightsParameters.switchLightFlashRateMs/(portTICK_RATE_MS*configLIGHTS_TASK_UPDATE_RATE_MS)) )
					{
						PinCpSwitchLed1_Write(!PinCpSwitchLed1_Read());
						lastSwitchFlashCount = loopCounter;
					}
				}
				break;
				
			case ST_SWITCH_LIGHT_FLASHING_ORANGE:
				if(_lightsParameters.prevSwitchLightState != _lightsParameters.switchLightState || xResult == pdPASS)
				{
					_lightsParameters.switchLightFlashRateMs = lightsCommandStruct.value1;
					
					PinCpSwitchLed1_Write(0);
					PinCpSwitchLed2_Write(1);
					
					lastSwitchFlashCount = loopCounter;
					
					_lightsParameters.prevSwitchLightState = _lightsParameters.switchLightState;
				}
				
				if(_lightsParameters.switchLightFlashRateMs != 0)
				{
					if(loopCounter >= lastSwitchFlashCount + (_lightsParameters.switchLightFlashRateMs/(portTICK_RATE_MS*configLIGHTS_TASK_UPDATE_RATE_MS)) )
					{
						PinCpSwitchLed2_Write(!PinCpSwitchLed2_Read());
						lastSwitchFlashCount = loopCounter;
					}
				}
				break;
		}
		
		
		// Increment loop counter
		loopCounter++;
	}
}

#ifdef __cplusplus
	} // extern "C" {
#endif

// EOF
