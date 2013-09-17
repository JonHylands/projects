###########################################################################
#
# flasher-32u4 Makefile
#
###########################################################################

MK_OS				= avr
MK_AVR_MCU			= atmega32u4
MK_AVR_FREQ			= 16MHz
MK_AVR_PROGRAMMER     ?= avrdude
MK_avrdude_PORT		?= /dev/ttyACM0
MK_avrdude_PROG    	?= avr109

# Extended fuse byte should be FB for standalone, FA for bootloader-stk500
MK_AVR_FUSES_m32u4   = C3 D0 FC
					  
MK_HEX_TARGET	= ammeter
MK_SRC_FILES	= ammeter.c Delay.c Log.c StrPrintf.o Timer.c i2cmaster.c

include ../../rules/mkRules.mk
