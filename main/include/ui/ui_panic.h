#ifndef UI_PANIC_H
#define UI_PANIC_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    PANIC_REBOOT = 0
} panic_action_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_panic(const char *message, panic_action_t action);

#ifdef __cplusplus
}
#endif

#endif /* UI_TOAST_H */