#inline
char xor_crc(char oldcrc, char data) {
	return oldcrc ^ data;
}

char EEPROMDataRead( int16 address, int8 *data, int16 count ) {
	char crc=0;

	while ( count-- != 0 ) {
		*data = read_eeprom( address++ );
		crc = xor_crc(crc,*data);
		data++;
	}
	return crc;
}

char EEPROMDataWrite( int16 address, int8 *data, int16 count ) {
	char crc=0;

	while ( count-- != 0 ) {
		/* restart_wdt() */
		crc = xor_crc(crc,*data);
		write_eeprom( address++, *data++ );
	}

	return crc;
}

void write_param_file() {
	int8 crc;

	/* write the config structure */
	crc = EEPROMDataWrite(PARAM_ADDRESS,(void *)&config,sizeof(config));
	/* write the CRC was calculated on the structure */
	write_eeprom(PARAM_CRC_ADDRESS,crc);

}

void write_default_param_file() {
	int8 i;

	/* green LED for 1.5 seconds */
	timers.led_on_green=150;

	config.revision='a';

	config.modbus_address=37;
	config.modbus_mode=MODBUS_MODE_RTU;

	config.serial_prefix=SERIAL_PREFIX_DEFAULT;
	config.serial_number=SERIAL_NUMBER_DEFAULT;

	/* initialize the channel configurations */
	for ( i=0 ; i<5 ; i++ ) {
		config.ch[i].startup=0;

		config.ch[i].adc_channel=i*2;
		config.ch[i].off_below_adc=500;
		config.ch[i].off_below_delay=500;
		config.ch[i].on_above_adc=700;
		config.ch[i].on_above_delay=1000;

		config.ch[i].override_timeout=300;

		config.ch[i].switch_type=DCSW_FPF2700_ACS714;
		config.ch[i].i_mon_offset=0;
		config.ch[i].invert=0;
	}

#if 0
	/* spanish fork */
	/*
		ch0 (A) - 12 volt bus
			off at 12.0 (adc=819 input=0) volts 60 second delay

		ch1 (B) - 24 volt bus
			off at 24.0 (adc=1638 input=1) volts 60 second delay

		ch2 (C) - Wind MPPT to 24 volt controller (unused)
			off at 24.0 (adc=1638 input=1) volts 60 second delay
		
		ch3 (D) - 48 volt LVD bus
			off at 48.0 (adc=3277 input=2) volts 60 second delay

		ch4 (E) - wind MPPT to 48 volt controller (unused)
			off at 48.0 (adc=3277 input=3) volts 60 second delay
	*/

	config.serial_prefix='A';
	config.serial_number=3387;
	config.modbus_address=50;

	config.ch[0].startup=0;
	config.ch[0].adc_channel=0;
	config.ch[0].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[0].off_below_adc=819;
	config.ch[0].off_below_delay=6000;
	config.ch[0].on_above_adc=839;
	config.ch[0].on_above_delay=6000;

	config.ch[1].startup=0;
	config.ch[1].adc_channel=2;
	config.ch[1].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[1].off_below_adc=1638;
	config.ch[1].off_below_delay=6000;
	config.ch[1].on_above_adc=1658;
	config.ch[1].on_above_delay=6000;

	config.ch[2].startup=0;
	config.ch[2].adc_channel=2;
	config.ch[2].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[2].off_below_adc=1638;
	config.ch[2].off_below_delay=6000;
	config.ch[2].on_above_adc=1658;
	config.ch[2].on_above_delay=6000;

	config.ch[3].startup=0;
	config.ch[3].adc_channel=4;
	config.ch[3].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[3].off_below_adc=3277;
	config.ch[3].off_below_delay=6000;
	config.ch[3].on_above_adc=3297;
	config.ch[3].on_above_delay=6000;

	config.ch[4].startup=0;
	config.ch[4].adc_channel=4;
	config.ch[4].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[4].off_below_adc=3277;
	config.ch[4].off_below_delay=6000;
	config.ch[4].on_above_adc=3297;
	config.ch[4].on_above_delay=6000;

#endif

#if 0
	/* porcupine dome LVD */
	/*
	ch4 (top) - classic for kestrel
		off at 47.0 (adc=3209) volts with 60 second delay
	ch3 - DAS panel
		off at 47.5 (adc=3243) volts with 60 second delay
	ch2 - CAM panel
		off at 48.5 (adc=3311) volts with 60 second delay
	ch1 - inverter (satellite)
		off at 50.0 (adc=3413) volts with 300 second delay
	ch0 - heater for anemometer
		off at 54.0 (adc=3685) volts with 10 second delay
	*/

	config.serial_prefix='A';
	config.serial_number=3420;
	config.modbus_address=50;

	config.ch[4].startup=0;
	config.ch[4].adc_channel=0;
	config.ch[4].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[4].off_below_adc=3209;
	config.ch[4].off_below_delay=6000;
	config.ch[4].on_above_adc=3229;
	config.ch[4].on_above_delay=6000;

	config.ch[3].startup=0;
	config.ch[3].adc_channel=0;
	config.ch[3].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[3].off_below_adc=3243;
	config.ch[3].off_below_delay=6000;
	config.ch[3].on_above_adc=3263;
	config.ch[3].on_above_delay=6000;

	config.ch[2].startup=0;
	config.ch[2].adc_channel=0;
	config.ch[2].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[2].off_below_adc=3311;
	config.ch[2].off_below_delay=6000;
	config.ch[2].on_above_adc=3331;
	config.ch[2].on_above_delay=6000;

	config.ch[1].startup=0;
	config.ch[1].adc_channel=0;
	config.ch[1].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[1].off_below_adc=3413;
	config.ch[1].off_below_delay=30000;
	config.ch[1].on_above_adc=3433;
	config.ch[1].on_above_delay=30000;

	config.ch[0].startup=0;
	config.ch[0].adc_channel=0;
	config.ch[0].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[0].off_below_adc=3685;
	config.ch[0].off_below_delay=1000;
	config.ch[0].on_above_adc=3705;
	config.ch[0].on_above_delay=1000;

#endif


#if 0
	/* porcupine dome turbine stop controller */
	config.modbus_address=51;

	/* 3 phase SSR for ? */
	config.ch[0].startup=0;
	config.ch[0].adc_channel=16;
	config.ch[0].switch_type=DCSW_FPF2700_ACS714;
	config.ch[0].invert=1;

	/* 3 phase SSR for ? */
	config.ch[1].startup=0;
	config.ch[1].adc_channel=16;
	config.ch[1].switch_type=DCSW_FPF2700_ACS714;
	config.ch[1].invert=1;

	/* unused */

	/* 2 x latching contactor (to disconnect / stop) for ? */
	config.ch[3].startup=0;
	config.ch[3].adc_channel=16;
	config.ch[3].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[3].invert=0;

	/* 2 x latching contactor (to disconnect / stop) for ? */
	config.ch[4].startup=0;
	config.ch[4].adc_channel=16;
	config.ch[4].switch_type=DCSW_LATCHING_CONTACTOR;
	config.ch[4].invert=0;
#endif

	for ( i=0 ; i<16 ; i++ ) {
		config.adc_offset[i]=0;
	}



	/* write them so next time we use from EEPROM */
	write_param_file();

}


void read_param_file() {
	int8 crc;

	crc = EEPROMDataRead(PARAM_ADDRESS, (void *)&config, sizeof(config)); 
		
	if ( crc != read_eeprom(PARAM_CRC_ADDRESS) || config.revision<'a' || config.revision>'z' ) {
		write_default_param_file();
	}
}


