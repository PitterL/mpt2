/*+
 * pinconf.c
 *
 * Created: 12/11/2019 1:27:27 PM
 *  Author: A41450
 */ 

#include <stdint.h>
#include <pin_manager.h>

#include "arch/pinconf.h"

#ifdef USE_MPTT_WRAPPER
void touch_ptc_pin_config(void)
{

}
#endif

uint8_t avdd_test(void)
{
    /* TBD */
    return 0;
}

uint8_t pinfault_test(uint8_t delay,uint8_t thld, uint8_t *test_pin, uint8_t *test_val)
{
    /* TBD */
    return 0;
}

void gpio_assert_chg(void)
{
    CHG_SetLow();
    CHG_SetDigitalOutput();
}

void gpio_release_chg(void)
{
    CHG_SetPullup();
    CHG_SetDigitalInput();
}