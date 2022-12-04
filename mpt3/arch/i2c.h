/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_I2C_H_
#define ARCH_I2C_H_

void bus_address_handler(void);
void bus_read_handler(void);
void bus_write_handler(void);
void bus_stop_handler(void);
void bus_error_handler(void);
void bus_init(void);
void bus_start(void);

#endif /* ARCH_I2C_H_ */