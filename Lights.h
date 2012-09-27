//!
//! @file 		Lights.h
//! @author 	Geoffrey Hunter (gbmhunter@gmail.com)
//! @edited 	n/a
//! @date 		09/09/2012
//! @brief 		Header file for Lights.c.
//! @details
//!		<b>Last Modified:			</b> 27/09/2012					\n
//!		<b>Version:					</b> v1.0.0						\n
//!		<b>Company:					</b> CladLabs					\n
//!		<b>Project:					</b> Example Code				\n
//!		<b>Computer Architecture:	</b> ARM						\n
//!		<b>Compiler:				</b> GCC						\n
//! 	<b>uC Model:				</b> PSoC5						\n
//! 	<b>Operating System:		</b> FreeRTOS v7.2.0			\n
//!		<b>Documentation Format:	</b> Doxygen					\n
//!		

//===============================================================================================//
//=================================== PUBLIC TYPEDEFS ===========================================//
//===============================================================================================//

typedef enum
{
	SWITCH_OFF,
	SWITCH_GREEN,
	SWITCH_ORANGE
} switchlightColour_t;

typedef enum
{
	CMD_LIGHTS_NULL,
	CMD_LIGHTS_ON,
	CMD_SWITCH_LIGHT_FLASH_GREEN,
	CMD_SWITCH_LIGHT_FLASH_ORANGE,
	CMD_SWITCH_LIGHT_OFF
} lightsCommandWord_t;

typedef struct
{
	lightsCommandWord_t commandWord;
	uint8_t value1;
	uint8_t value2;
} lightsCommandStruct_t;

//===============================================================================================//
//=================================== PUBLIC FUNCTION PROTOTYPES ================================//
//===============================================================================================//

// See the function definitions in Bldc.c for more information

void Lights_Start(uint32 mainBldcStackSize, uint8 mainBldcPriority);
void Lights_SendCommandToTask(lightsCommandWord_t lightCommandWord, uint8 value1, uint8 value2);

//===============================================================================================//
//================================== PUBLIC VARIABLES/STRUCTURES ================================//
//===============================================================================================//



// EOF