#include "dcswcI2C.h"


typedef struct {
	int8 revision;
	int8 modbus_address;
	int8 modbus_mode;

	int8 serial_prefix;
	int16 serial_number;

	int8 uart_sc_sbd;
} struct_config;


typedef struct {
	int16 sequence_number;
	int16 uptime_minutes;
	int16 interval_milliseconds;

	int8 factory_unlocked;

	int16 adc[16];
	
	int8 restart_now;
	int8 adc_sample_ch;
} struct_current;

typedef struct {
	int8 led_on_green;
	int8 led_on_red;
} struct_time_keep;


/* global structures */
struct_config config={0};
struct_current current={0};
struct_time_keep timers={0};

#include "i2c_access_dcswcI2C.c"
#include "uart_sc16is740_dcswcI2C.c"
#include "mcp3208_dcswcI2C.c"
#include "interrupt_dcswcI2C.c"
#include "param_dcswcI2C.c"


#include "filter_dcswcI2C.c"


void init() {
	setup_oscillator(OSC_8MHZ || OSC_INTRC); 
//	setup_adc(ADC_CLOCK_INTERNAL);
	setup_adc_ports(NO_ANALOGS);
	setup_wdt(WDT_ON);

	/* 
	Manually set ANCON0 to 0xff and ANCON1 to 0x1f for all digital
	Otherwise set high bit of ANCON1 for VbGen enable, then remaining bits are AN12 ... AN8
	ANCON1 AN7 ... AN0
	set bit to make input digital
	*/
	/* AN7 AN6 AN5 AN4 AN3 AN2 AN1 AN0 */
	ANCON0=0xff;
	/* VbGen x x 12 11 10 9 8 */
	ANCON1=0x1f;

	setup_ccp1(CCP_OFF);
	setup_ccp2(CCP_OFF);
	setup_ccp3(CCP_OFF);
	setup_ccp4(CCP_OFF);
	setup_ccp5(CCP_OFF);

	output_low(RS485_DE); /* shut off RS-485 transmitter */

	/* data structure initialization */
	timers.led_on_green=0;
	timers.led_on_red=0;

	current.sequence_number=0;
	current.uptime_minutes=0;
	current.interval_milliseconds=0;
	current.factory_unlocked=0;
	current.restart_now=0;


	/* interrupts */
	/* timer0 - Modbus slave timeout timer */
	/* configured in modbus_slave_sdc.c */

	/* timer1 - unused */

	/* timer2 - unused */

	/* timer3 - general housekeeping Prescaler=1:1; TMR1 Preset=45536; Freq=100.00Hz; Period=10.00 ms */
	setup_timer_3(T3_INTERNAL | T3_DIV_BY_1);
	set_timer3(45536);
	enable_interrupts(INT_TIMER3);

	/* timer4 - unused */

	/* FTDI serial cable receive character */
	enable_interrupts(INT_RDA2);

	/* initialize MCP3208 external ADCs */
	mcp3208_init();


	/* initialize SCI UART @ 19200 */
	uart_init(6); /* 2=>57600 (tested, works) 6=>19200 */


	delay_ms(14);

}

int8 get_ack_status(int8 address) {
	int8 status;

	i2c_start();
	status = i2c_write(address);  // Status = 0 if got an ACK
	i2c_stop();

	if ( 0 == status )
		return TRUE;

   return FALSE;
}

void main(void) {
	int8 i,j;
	int16 l;

	init();
	read_param_file();

	if ( config.modbus_address > 127 ) {
		write_default_param_file();
	}


	/* prime filters */
	for ( i=0 ; i<20 ; i++ ) {
		for ( j=0 ; j<16 ; j++ ) {
			current.adc[j]=mean_filter_n(j,mcp3208_read(j));
		}
	}




#if 1
	fprintf(STREAM_WORLD,"# dcswcI2C (%c%lu) start up (ds30 bootloader support) (worldData stream) (modbus address=%u) %s\r\n",
		config.serial_prefix,
		config.serial_number,
		config.modbus_address,
		__DATE__
	);
#endif	


#if 1
	/* scan I2C bus */
	for (i=0x10 ; i<0xF0 ; i+=2) {
		if ( get_ack_status(i) ) {
			fprintf(STREAM_WORLD,"# testing address 0x%02x ...",i);
			fprintf(STREAM_WORLD," got ack!\r\n");
    	} else {
//			fprintf(STREAM_FTDI," nothing\r\n");
		}
   	}
#endif


	config.uart_sc_sbd=get_ack_status(0x9a);

	timers.led_on_red=timers.led_on_green=0;

#if 0
	/* set ports to their startup state */
	for ( i=0 ; i<5 ; i++ ) {
		current.p_on[i]=config.ch[i].startup;
	}
#endif

	i=0;
	for ( ; ; ) {
		restart_wdt();

			if ( config.uart_sc_sbd && uart_kbhit() ) {
				fprintf(STREAM_WORLD,"# RockBLOCK says: '%c'\r\n",uart_getc());	
			}
			

#if 1
		if ( current.restart_now ) {
			timers.led_on_green=200;

			output_high(CTRL_0);

			fprintf(STREAM_WORLD,"# you said '%c'\r\n",current.restart_now);
			if ( config.uart_sc_sbd ) {
				uart_putc(current.restart_now);
			}

			current.restart_now=0;


#if 0
			/* read a block of bytes from device */
			i2c_start();
			delay_us(15);
			i2c_write(0x36);
			i2c_write(0); /* register address 0 */
			i2c_start();
			delay_us(15);
			i2c_write(0x36 | 1); /* read */
			
			for ( i=0 ; i<16 ; i++ ) {
				j=i2c_read(1);

				fprintf(STREAM_WORLD,"# byte addr[0x%02x]=0x%02x (%u)\r\n",
					i,
					j,
					j
				);
			}
			i2c_read(0);
#endif

#if 1
			/* 16 bit registers, but byte addressed */
			for ( i=0 ; i<18 ; i ++ ) {
				l=i2c_register_read16(0x36,i);
				fprintf(STREAM_WORLD,"# reg addr[0x%02x]=0x%04lx (%lu)\r\n",i,l,l);
			}
#endif

#if 0
			fprintf(STREAM_WORLD,"# writing 0x%04lx to register 1\r\n",n);
			i2c_register_write16(0x36,1,n);
			n+=200;
#endif
			output_low(CTRL_0);

		}
#endif



		
	}
}
