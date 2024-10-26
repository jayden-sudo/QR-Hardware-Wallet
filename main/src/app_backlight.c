/*********************
 *      INCLUDES
 *********************/
#include "app_backlight.h"
#include "peripherals.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/
bool app_backlight_support(void);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
bool app_backlight_support(void)
{
    return CONFIG_LCD_GPIO_BL_PWM != 0;
}