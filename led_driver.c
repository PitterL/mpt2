/*
 * led_driver.c
 *
 * Created: 18-09-2017 11:44:47
 *  Author: I41681
 */

#include "led_driver.h"

#if ENABLE_LED == 1u

extern qtm_surface_cs_control_t qtm_surface_cs_control1;

uint8_t i2c_write_buf[3];

void I2C_write(uint8_t slvae_address, uint8_t *buffer, uint8_t len)
{
	I2C_open(slvae_address);
	I2C_set_buffer(buffer, len);
	I2C_master_write();
	while (I2C_BUSY == I2C_close())
		;
}

void init_led_driver(void)
{
	i2c_write_buf[0] = 0x00; // reg direction
	i2c_write_buf[1] = 0x00; // output
	I2C_write(SLAVE_ADDR, i2c_write_buf, 2);

	i2c_write_buf[0] = 0x14; // reg address
	i2c_write_buf[1] = 0x0;  // all low 0X6F
	I2C_write(SLAVE_ADDR, i2c_write_buf, 2);

	i2c_write_buf[0] = 0x01; // reg direction
	i2c_write_buf[1] = 0x00; // output
	I2C_write(SLAVE_ADDR, i2c_write_buf, 2);

	i2c_write_buf[0] = 0x15; // reg address
	i2c_write_buf[1] = 0x0;  // all low 0X1F
	I2C_write(SLAVE_ADDR, i2c_write_buf, 2);
}

void led_gpio_update(uint8_t data, uint8_t ver_or_hor)
{
	if (ver_or_hor == LED_HOR) {
		i2c_write_buf[0] = 0x14; // reg address
		i2c_write_buf[1] = data; // all low
		I2C_write(SLAVE_ADDR, i2c_write_buf, 2);
	} else {
		i2c_write_buf[0] = 0x15; // reg address
		i2c_write_buf[1] = data; // all low
		I2C_write(SLAVE_ADDR, i2c_write_buf, 2);
	}
}

void led_decode_position(void)
{
	uint8_t h_pos = 0, v_pos = 0;
	led_reset();

	if (qtm_surface_cs_control1.qtm_surface_contact_data->qt_surface_status) {
		h_pos = (qtm_surface_cs_control1.qtm_surface_contact_data->h_position / 43);
		h_pos = (1 << (5 - h_pos));
		led_gpio_update(h_pos, LED_HOR);

		v_pos = (qtm_surface_cs_control1.qtm_surface_contact_data->v_position / 52);
		v_pos = (1 << (4 - v_pos));
		led_gpio_update(v_pos, LED_VER);
	}
}

void led_reset(void)
{
	led_gpio_update(0, LED_HOR);
	led_gpio_update(0, LED_VER);
}

#endif
