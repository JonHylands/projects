/**********************************************************/

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
#include "UART.h"
#include "i2cmaster.h"

/* ---- Public Variables ------------------------------------------------- */

/* ---- Private Constants and Types -------------------------------------- */

#define LED_DDR        DDRE
#define LED_PORT       PORTE
#define LED_MASK       ( 1 << 6  )

#define MONITOR_ADDRESS_WRITE 0x64
#define MONITOR_ADDRESS_READ 0xC8

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

uint16_t readCurrentCounterValue(void) {

	i2c_start (0xC8);
	i2c_write(0x02);
	i2c_start (0xC9);
	uint16_t high = (uint16_t)i2c_readAck();
	uint16_t low = (uint16_t)i2c_readNak();
	i2c_stop();
	uint16_t charge = (high * 256) + low;
	return charge;
}


//***************************************************************************
//
//	Ammeter Testing
//

int main( void )
{
	tick_t previousCount;
	
	volatile uint16_t currentCounterValue;
	volatile uint16_t lastCounterValue;
	volatile tick_t lastIncrementTimer;
	volatile tick_t currentTimer;

    // Initialize the two LED pins

    LED_DDR |= LED_MASK;
	InitTimer ();
	InitUART ();
	i2c_init ();

	fdevopen (UART1_PutCharStdio, UART1_GetCharStdio);

	printf ("\nLinear LTC2942 I2C Current Monitor Test\n\n");
	
	// set the prescalar to 1 (2^0)
	i2c_start (0xC8);
	i2c_write(0x01);
	i2c_write(0x00);
	i2c_stop();
	
	currentCounterValue = readCurrentCounterValue();
	lastCounterValue = currentCounterValue;
	lastIncrementTimer = gTickCount;

	int done = 0;
    while ( !done )
    {
		previousCount = gTickCount;
		while (gTickCount == previousCount);
		currentTimer = gTickCount;
		currentCounterValue = readCurrentCounterValue();
		if (currentCounterValue > lastCounterValue) {
			tick_t timeSinceLastIncrement = currentTimer - lastIncrementTimer;
			uint16_t counterIncrease = currentCounterValue - lastCounterValue;
			double increaseOverTime = (double)counterIncrease / (double)timeSinceLastIncrement;
			uint16_t current = (uint16_t)floor(increaseOverTime * 100.0);
// 			printf("%u - %u / %u\n", currentTimer, counterIncrease, timeSinceLastIncrement);
 			printf("%u - %u mA\n", currentTimer, current);
			lastIncrementTimer = currentTimer;
			lastCounterValue = currentCounterValue;
		}
	}
    ms_spin(1000);
}
