/*
 * pinconf.h
 *
 * Created: 12/11/2019 1:31:49 PM
 *  Author: A41450
 */ 


#ifndef ARCH_I2C_H_
#define ARCH_I2C_H_

#define BUS_SLOT

#ifdef BUS_SLOT
#define I2C_open I2C_0_open
#define I2C_read I2C_0_read
#define I2C_write I2C_0_write
#define I2C_send_ack I2C_0_send_ack
#define I2C_send_nack I2C_0_send_nack
#define I2C_set_read_callback I2C_0_set_read_callback
#define I2C_set_write_callback I2C_0_set_write_callback
#define I2C_set_address_callback I2C_0_set_address_callback
#define I2C_set_stop_callback I2C_0_set_stop_callback
#define I2C_set_collision_callback I2C_0_set_collision_callback
#define I2C_set_bus_error_callback I2C_0_set_bus_error_callback
#endif

void bus_address_handler(void);
void bus_read_handler(void);
void bus_write_handler(void);
void bus_stop_handler(void);
void bus_error_handler(void);
void bus_init(void);
void bus_start(void);

#endif /* ARCH_I2C_H_ */