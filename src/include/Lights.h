//!
//! @file 		Lights.h
//! @author 	Geoffrey Hunter <gbmhunter@gmail.com> (www.cladlab.com)
//! @edited 	n/a
//! @date 		09/09/2012
//! @brief 		FreeRTOS task for controlling lights (e.g. LEDS).
//! @details
//!		<b>Last Modified:			</b> 2013/05/16					\n
//!		<b>File Version:			</b> v1.0.3.0					\n
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
	
		

//===============================================================================================//
//============================================ GUARDS ===========================================//
//===============================================================================================//

#ifndef LIGHTS_H
#define LIGHTS_H

#ifdef __cplusplus
	extern "C" {
#endif

//===============================================================================================//
//=================================== PUBLIC TYPEDEFS ===========================================//
//===============================================================================================//

//! Enumerates the different colours a bidirectional LED can be set to
typedef enum
{
	SWITCH_OFF,
	SWITCH_GREEN,
	SWITCH_ORANGE
} switchlightColour_t;

//! Enumerates the commands that can be sent to Lights_Task() using Lights_SendCommandToTask()
typedef enum
{
	CMD_LIGHTS_NULL,
	CMD_LIGHTS_ON,
	CMD_SWITCH_LIGHT_FLASH_GREEN,
	CMD_SWITCH_LIGHT_FLASH_ORANGE,
	CMD_SWITCH_LIGHT_OFF
} lightsCommandWord_t;

//! The command structure consists of a #commandWord_t and two values (which may or may not be
//! used depending on the command
typedef struct
{
	lightsCommandWord_t commandWord;
	uint8_t value1;
	uint8_t value2;
} lightsCommandStruct_t;

//===============================================================================================//
//=================================== PUBLIC FUNCTION PROTOTYPES ================================//
//===============================================================================================//

//! @brief		Start-up function. Call from main() before starting scheduler
//! @note		Not thread-safe. Do not call from any task!
//! @sa			main()
//! @public
void Lights_Start(uint32 mainBldcStackSize, uint8 mainBldcPriority);

//! @brief		Wrapper function used by external tasks to send a command to the lights task
//! @details	Commands are enumerated as #lightsCommandWord_t
//! @sa			Lights_Task()
//! @public
void Lights_SendCommandToTask(lightsCommandWord_t lightCommandWord, uint8 value1, uint8 value2);

//===============================================================================================//
//================================== PUBLIC VARIABLES/STRUCTURES ================================//
//===============================================================================================//

// none

#ifdef __cplusplus
	} // extern "C" {
#endif

#endif // #ifndef LIGHTS_H

// EOF
