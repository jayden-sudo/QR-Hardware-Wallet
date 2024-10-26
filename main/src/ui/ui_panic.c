/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_panic.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_lvgl_port.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "ui_panic"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void panic_event_cb(lv_event_t *e);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_panic(const char *message, panic_action_t action);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void panic_event_cb(lv_event_t *e)
{
    esp_restart();
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_panic(const char *message, panic_action_t action)
{
    ESP_LOGE(TAG, "%s", message);
    lv_obj_t *mbox1 = lv_msgbox_create(NULL);
    lv_obj_set_size(mbox1, lv_obj_get_content_width(lv_scr_act()), LV_SIZE_CONTENT);
    lv_msgbox_add_title(mbox1, "Panic - can't recover");
    lv_msgbox_add_text(mbox1, message);

    lv_obj_t *btn;
    btn = lv_msgbox_add_footer_button(mbox1, "Reboot!");
    lv_obj_add_event_cb(btn, panic_event_cb, LV_EVENT_CLICKED, NULL);
}