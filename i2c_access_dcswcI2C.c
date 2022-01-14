int16 i2c_register_read16(int8 i2c_address, int8 regaddr) {
	int8 msb, lsb;

	/* start I2C transaction and write the register we want to read to the slave */
	i2c_start();
	delay_us(15);
	i2c_write(i2c_address);
	i2c_write(regaddr);

	/* restart I2C and read two bytes from the slave */
	i2c_start();
	delay_us(15);
	i2c_write(i2c_address | 1);  // read cycle
	msb=i2c_read(1);
	lsb=i2c_read(0);                                
	i2c_stop();

	return make16(msb,lsb);
}

void i2c_register_write16(int8 i2c_address, int8 regaddr, int16 value) {
	i2c_start();
	delay_us(15);                                                 
	i2c_write(i2c_address); // write cycle                       
	i2c_write(regaddr);  // write cycle         
	i2c_write(make8(value,1));
	i2c_write(make8(value,0));
	i2c_stop();
}

void i2c_register_write32(int8 i2c_address, int8 regaddr, int32 value) {
	i2c_start();
	delay_us(15);                                                 
	i2c_write(i2c_address); // write cycle                       
	i2c_write(regaddr);  // write cycle     
	i2c_write(make8(value,3));
	i2c_write(make8(value,2));    
	i2c_write(make8(value,1));
	i2c_write(make8(value,0));
	i2c_stop();
}