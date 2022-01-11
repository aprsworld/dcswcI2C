#include <18F25K80.h>
#device ADC=12
#device *=16


#if 1
/* no boot loader */
#fuses INTRC_IO
#fuses NOFCMEN
#fuses NOIESO
#fuses PUT
#fuses BORV30
#fuses WDT32768
#fuses NOMCLR
#fuses STVREN
#fuses SOSC_DIG
#fuses NOXINST
#fuses NODEBUG

#else

/* DS30 boot loader version 1.5.1 - engine 2.2.2 */
/* leave last nine pages alone for boot loader. first two words do the jump to the boot loader */
/* max mem address - 0x243, max mem address - 0x240 */
#build(reset=0x7dbc:0x7dbf)
/* max mem address - 0x23f, max mem address - see memory organization in datasheet */
//#org 0x7dc0,0x7fff {}
#org 0x7dc0,0x7fff {}

#endif


#include <stdlib.h>
#use delay(clock=8000000, restart_wdt)

/* 
Parameters are stored in EEPROM
*/
#define PARAM_CRC_ADDRESS  0x000
#define PARAM_ADDRESS      PARAM_CRC_ADDRESS+2


/* UART1 - RS-485 connection modbus */
#use rs232(UART1,stream=MODBUS_SERIAL,baud=9600,xmit=PIN_C6,rcv=PIN_C7,errors)	


#byte TXSTA=GETENV("SFR:txsta1")
#bit  TRMT=TXSTA.1
#byte ANCON0=GETENV("SFR:ancon0")
#byte ANCON1=GETENV("SFR:ancon1")

#byte PORTB=GETENV("SFR:portb")
#byte INTCON2=GETENV("SFR:intcon2")
#bit RBPU=INTCON2.7

/* UART2 - FTDI cable */
#use rs232(UART2,stream=STREAM_WORLD, baud=57600,errors)	

/* I2C on hardware */
#use i2c(master, sda=PIN_C4, scl=PIN_C3, FAST)


#use standard_io(A)
#use standard_io(B)
#use standard_io(C)


#define LED_GREEN       PIN_B5

#define SER_TO_NET      PIN_B6
#define SER_FROM_NET    PIN_B7
#define RS485_DE        PIN_C5


#define CTRL_0          PIN_A0
#define CTRL_1          PIN_A1
#define CTRL_2          PIN_A2
#define CTRL_3          PIN_A3
#define CTRL_4          PIN_A5

#define STAT_0          PIN_A7
#define STAT_1          PIN_A6
#define STAT_2          PIN_C0
#define STAT_3          PIN_C1
#define STAT_4          PIN_C2

#define I2C_SCL         PIN_C3
#define I2C_SDA         PIN_C4

#define CS_ADC0         PIN_B0
#define SPI_DIN         PIN_B1
#define SPI_DOUT        PIN_B2
#define SPI_CLK         PIN_B3
#define CS_ADC1         PIN_B4

/* U4 - first ADC */
#define ADC_CH_VDIV_0   0
#define ADC_CH_IMON_0   1
#define ADC_CH_VDIV_1   2
#define ADC_CH_IMON_1   3
#define ADC_CH_VDIV_2   4
#define ADC_CH_IMON_2   5
#define ADC_CH_VDIV_3   6
#define ADC_CH_IMON_3   7

/* U6 - second ADC */
#define ADC_CH_VDIV_4   8
#define ADC_CH_IMON_4   9
#define ADC_CH_VDIV_5   10
#define ADC_CH_TP_1     11
#define ADC_CH_VDIV_6   12
#define ADC_CH_TP_2     13
#define ADC_CH_VDIV_7   14
#define ADC_CH_TP_3     15




/* Modbus defines */
#define MODBUS_MODE_RTU     0
#define MODBUS_MODE_TCP_RTU 1

#define SERIAL_PREFIX_DEFAULT 'Z'
#define SERIAL_NUMBER_DEFAULT 2221


#define DCSW_FPF2700_ACS714     0
#define DCSW_LATCHING_CONTACTOR 1
#define DCSW_SMALL48            2