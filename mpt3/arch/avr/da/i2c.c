/**
 * \file
 *
 * \brief I2C interface wrapper.
 *
 (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
#include <common/include/types.h>
#include "arch/bus.h"
#include "arch/i2c.h"
#include "arch/pinconf.h"
#include "arch/avr/da/drivers/i2c_slave.h"

#ifdef USE_I2C_SLOT
// Below interface is I2C slave view
#define I2C_init I2C_0_init
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

#define I2C_SLAVE_SLOT_ADDR 0x4a

extern ssint handle_bus_event(u8 state, u8 *val);
void bus_address_handler(void)
{
  u8 addr8;

  addr8 = I2C_read();
  if ((addr8 >> 1) == I2C_SLAVE_SLOT_ADDR)
  {
    if (addr8 & 0x1)
    {
      bus_state_change(BUS_READ);
    }
    else
    {
      bus_state_change(BUS_WRITE);
    }
    I2C_send_ack(); // or send_nack() if we don't want to ack the address
  }
  else
  {
    bus_state_change(BUS_STOP);
    I2C_send_nack();
  }
}

void bus_read_handler(void)
{
  u8 val = 0xA5; // Initial value for debug only
  ssint result;

  result = handle_bus_event(BUS_READ, &val);
  if (result < 0) {
    // Something is error
  }

  // Must respond a data to host
  I2C_write(val);
}

void bus_write_handler(void)
{
  u8 val;
  ssint result;

  val = I2C_read();
  result = handle_bus_event(BUS_WRITE, &val);
  if (result >= 0)
  { // handle the data or discard(zero)
    I2C_send_ack();
  }
  else
  {
    I2C_send_nack();
  }
}

void bus_stop_handler(void)
{
  handle_bus_event(BUS_STOP, NULL);
}

void bus_error_handler(void)
{
  handle_bus_event(BUS_ERROR, NULL);
}

void i2c_init(void)
{
  // I2C 
  I2C_init(I2C_SLAVE_SLOT_ADDR);
  // Note: the read and write is looked up by the bus view
  I2C_set_read_callback(bus_read_handler);
  I2C_set_write_callback(bus_write_handler);
  I2C_set_address_callback(bus_address_handler);
  I2C_set_stop_callback(bus_stop_handler);
  I2C_set_collision_callback(bus_error_handler);
  I2C_set_bus_error_callback(bus_error_handler);
}

void i2c_start(void)
{
  I2C_open();
}

/**
 * \brief Initialize I2C Data Bus for communication
 */

void bus_init(void)
{ 
  // Bus pinmux
  gpio_bus_pin_config();

  // I2C init
  i2c_init();
 
  // bus_state_change(BUS_STOP);
}

void bus_start(void)
{
  i2c_start();
}
