#ifndef CTRL_INIT_H
#define CTRL_INIT_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

/*********************
 *      DEFINES
 *********************/
#define EVENT_LOCK_SCREEN (1 << 0)

/**********************
 *      VARIABLES
 **********************/
extern EventGroupHandle_t event_group_global;

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ctrl_init(void *parameters);

#ifdef __cplusplus
}
#endif

#endif /* CTRL_INIT_H */