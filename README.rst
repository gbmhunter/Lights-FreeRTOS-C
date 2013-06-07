==============================================================
Light Controller Library
==============================================================

- Author: gbmhunter <gbmhunter@gmail.com> (http://www.cladlab.com)
- Created: 2012/03/19
- Last Modified: 2013/06/08
- Library Version: v1.0.3.1
- Company: CladLabs
- Project: Free Code Libraries	.
- Language: C
- Compiler: GCC	
- uC Model: n/a
- Computer Architecture: n/a
- Operating System: FreeRTOS
- Documentation Format: Doxygen
- License: GPLv3

Description
===========

A light controller library designed to control LED's and other lights on an embedded platform running FreeRTOS.

An typical command to the lights task looks like:

::

	Lights::SendCommandToTask(Lights::CMD_SWITCH_LIGHT_FLASH_GREEN, 200, 0);
	
This will cause the led called SWITCH_LIGHT to FLASH_GREEN at a period of 200ms, continuing on forever until the state is changed 
by another call (time of 0 means do not stop).

Internal Dependencies
=====================

These are all included in the repository.

- Lights.c, .h						-> Lights FreeRTOS task and control functions

External Dependencies
=====================
- "UartDebug" 	-> Used for printing debug messages.
- "FreeRTOS.h" 	-> FreeRTOS API (Lights.c implements a FreeRTOS task).
- "task.h"		-> FreeRTOS API (Lights.c implements a FreeRTOS task).
- "queue.h"		-> FreeRTOS API (Lights.c implements a FreeRTOS task).
- "semphr.h"	-> FreeRTOS API (Lights.c implements a FreeRTOS task).
- "PublicDefinesAndTypeDefs.h"	-> Defines a boolean type (bool_t).
- "Config.h"	-> Contains configuration settings,

BUGS/LIMITATIONS/TODO
=====================

See Issues.

Usage
=====

Edit Lights.h and configure the light commands to suit your particular application.
Add functionality for any new commands in Light.c.

**Main.c**

::

	#include "Lights.h"
	
	main()
	{
		// Initialise lights task
		Lights_Start();
		
		// Start FreeRTOS scheduler
		vTaskStartScheduler();
	}
	
	AnotherTask()
	{
		// Starts green light flashing, then after a second turns it off, waits another second,
		// and then repeats.
		while(1)
		{
			Lights_SendCommandToTask(CMD_SWITCH_LIGHT_FLASH_GREEN, 200, 0);
			DelayMs(1000);
			Lights_SendCommandToTask(CMD_SWITCH_LIGHT_OFF, 0, 0);
			DelayMs(1000);
		}
		
	}

	
Changelog
=========

======== ========== ===================================================================================================
Version  Date       Comment
======== ========== ===================================================================================================
v1.0.3.1 2013/06/08 README Changelog now in reverse chronological order and in table format.
v1.0.3.0 2013/05/15 Added README.rst. Moved appropriate info into this file. Added usage info. Deleted unneeded "#include UartComms.h".
v1.0.2.0 2012/11/06	Moved documentation into .h file. Added C++ header guard.	Added detail comments to some functions. Removed '_' prefix from header guard ID (restricted namespace).
v1.0.1.0 2012/10/08	Added header guard, modified a few other comments.
v1.0.0.0 2012/09/26	First versioned commit.
======== ========== ===================================================================================================