/*********************
 *      INCLUDES
 *********************/
#include "controller/ctrl_init.h"
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "kv_fs.h"
#include <string.h>
#include "wallet.h"
#include "controller/ctrl_wizard.h"
#include "controller/ctrl_home.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "ctrl_init";

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ctrl_init(void *parameters);

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ctrl_init(void *parameters)
{
    while (1)
    {
        char *privateKeyStr = NULL;
        ctrl_wizard(&privateKeyStr);
        while (privateKeyStr == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        ctrl_wizard_free();
        /* show main page */
        int home_flag = 0;
        ctrl_home_init(privateKeyStr, &home_flag);

        // debug
        // TaskHandle_t lvgl_task_handle = __test_get_lvgl_task_handle();
        // UBaseType_t stackHighWaterMark = 0;
        // while (home_flag == 0)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(10));

        //     UBaseType_t _stackHighWaterMark = uxTaskGetStackHighWaterMark(lvgl_task_handle);
        //     if (_stackHighWaterMark != stackHighWaterMark)
        //     {
        //         stackHighWaterMark = _stackHighWaterMark;
        //         ESP_LOGI("MEM USAGE", "Task LVGL remaining stack size: %u bytes", stackHighWaterMark * sizeof(StackType_t));
        //     }
        // }

        while (home_flag == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        ctrl_home_free();
        free(privateKeyStr);
        privateKeyStr = NULL;
    }

    vTaskDelete(NULL);
}