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
#include "adc.h"
#include "i2cmaster.h"

/* ---- Public Variables ------------------------------------------------- */

/* ---- Private Constants and Types -------------------------------------- */

#define LED_DDR        DDRE
#define LED_PORT       PORTE
#define LED_MASK       ( 1 << 6  )

#define MOSI_DDR	DDRB
#define MOSI_PORT   PORTB
#define MOSI_MASK   ( 1 << 2 )

#define MISO_DDR	DDRB
#define MISO_PORT   PORTB
#define MISO_PIN	PINB
#define MISO_MASK   ( 1 << 3 )

#define SCK_DDR	 DDRB
#define SCK_PORT	PORTB
#define SCK_MASK	( 1 << 1 )

#define SS_DDR	 DDRB
#define SS_PORT	PORTB
#define SS_MASK	( 1 << 0 )

#define SPI_CLOCK_TIME 10

#define SetDirectionIn(x)   x ## _DDR &= ~ x ## _MASK; x ## _PORT &= ~ x ##_MASK
#define SetDirectionOut(x)  x ## _DDR |=   x ## _MASK


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

/**
*   Functions for manipulating SCK, MOSI and MISO
*/

static inline void SPI_ClockLow( void )
{
	SCK_PORT &= ~SCK_MASK;
}

static inline void SPI_ClockHigh( void )
{
	SCK_PORT |= SCK_MASK;
}

static inline void SPI_SlaveSelectLow( void )
{
	SS_PORT &= ~SS_MASK;
}

static inline void SPI_SlaveSelectHigh( void )
{
	SS_PORT |= SS_MASK;
}

static inline void SPI_DataOut( uint16_t bit )
{
	if ( bit )
	{
		MOSI_PORT |= MOSI_MASK;
	}
	else
	{
		MOSI_PORT &= ~MOSI_MASK;
	}
}

static inline uint8_t SPI_DataIn( void )
{
	return ( MISO_PIN & MISO_MASK ) != 0;  
}


//***************************************************************************
/**
*   Initialize the SPI for Master mode
*/

static void SPI_MasterInit( void )
{
	SetDirectionOut(MOSI);
	SetDirectionIn(MISO);
	SetDirectionOut(SCK);
	SetDirectionOut(SS);

	// SCK idles low
	SPI_ClockLow();
	SPI_SlaveSelectHigh();

} // SPI_MasterInit

//***************************************************************************
/**
*   Do a conversion according to the argument, then read and return the value
*/

static uint16_t SPI_DoConversion ( void )
{
	uint8_t i;
	uint16_t value = 0;

	SPI_ClockHigh();
	SPI_SlaveSelectLow();
	for ( i = 0; i < 4; i++ )
	{
		// spin the clock 4 times to ignore the leading zero bits
		us_spin (SPI_CLOCK_TIME);
		SPI_ClockLow();
		us_spin (SPI_CLOCK_TIME);
		SPI_ClockHigh();
	}

	// retrieve 16 bits
	for (i = 0; i < 16; i++)
	{
		us_spin (SPI_CLOCK_TIME);
		SPI_ClockLow();
		us_spin (SPI_CLOCK_TIME);
		value <<= 1;
		value |= ( SPI_DataIn() & 0x01 );
		SPI_ClockHigh();
	}
	
	SPI_SlaveSelectHigh();
	
	return value;
	
} // SPI_DoConversion


//***************************************************************************
//
//	Ammeter Testing
//

int main( void )
{
	tick_t targetCount;

	SPI_MasterInit();

    // Initialize the two LED pins

    LED_DDR |= LED_MASK;
	InitTimer ();
	InitUART ();
	ADC_Init (ADC_PRESCALAR_AUTO);

	fdevopen (UART1_PutCharStdio, UART1_GetCharStdio);

	printf ("\nLinear LTC1999 Current Monitor Test\n\n");
	
	int done = 0;
    while ( !done )
    {
 		targetCount = gTickCount + 10;
 		while (gTickCount < targetCount);

		uint16_t value = SPI_DoConversion();
// 		printf ("%d\n", value);
		uint16_t current = value / 20;
		uint16_t voltageValue = ADC_Read (1);
		int16_t voltage = voltageValue * 5;
		printf ("(%u) %u mA - %d mV\n", value, current, voltage);
	}
    ms_spin(1000);
}
