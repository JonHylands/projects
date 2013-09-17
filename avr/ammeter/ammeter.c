/****************************************************************************
*
*   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
*   See README and COPYING for more details.
*
****************************************************************************/
/**
*
*   @file   flasher.c
*
*   @brief  Simple LED flasher for the Baby Orangatang board from Pololu
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <math.h>

#include "Delay.h"
#include "Log.h"
#include "Timer.h"
#include "i2cmaster.h"

/* ---- Public Variables ------------------------------------------------- */

/* ---- Private Constants and Types -------------------------------------- */

#define LED_DDR        DDRE
#define LED_PORT       PORTE
#define LED_MASK       ( 1 << 6  )

/* ---- Private Variables ------------------------------------------------ */

/* ---- Functions -------------------------------------------------------- */


/////////////////////////////////////////////////////
//
//	Turn on the LED
//

static inline void turnOnLED (void)
{
	LED_PORT |= LED_MASK;
}

/////////////////////////////////////////////////////
//
//	Turn off the LED
//

static inline void turnOffLED (void)
{
	LED_PORT &= ~LED_MASK;
}



//***************************************************************************
/**
*   Simple LED flasher
*/

int main( void )
{
    // Initialize the two LED pins

    LED_DDR |= LED_MASK;

    while ( 1 )
    {
        turnOnLED();
        ms_spin( 100 );

        turnOffLED();
        ms_spin( 400 );
    }
}
