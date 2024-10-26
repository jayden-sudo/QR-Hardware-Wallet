/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_qr_code.h"
#include "ui/ui_style.h"
#include "esp_log.h"
#include "ui/ui_master_page.h"
#include "string.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "UI_QR_CODE"

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *container = NULL;
static lv_obj_t *event_target = NULL;
static ui_master_page_t *master_page = NULL;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_qr_code_destroy(void *arg);
static void ui_event_handler(lv_event_t *e);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_qr_code_init(char *title, char *text_pre, char *qr_code, char *text_post);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void ui_qr_code_destroy(void *arg)
{
    if (lvgl_port_lock(0))
    {
        if (container != NULL)
        {
            lv_obj_del(container);
            container = NULL;
        }
        if (event_target != NULL)
        {
            lv_obj_del(event_target);
            event_target = NULL;
        }
        lvgl_port_unlock();
    }
    if (master_page != NULL)
    {
        ui_master_page_destroy(master_page);
        free(master_page);
        master_page = NULL;
    }
}
static void ui_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED)
    {
        ui_master_page_set_close_button_visibility(false, master_page);
        lv_async_call(ui_qr_code_destroy, NULL);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_qr_code_init(char *title, char *text_pre, char *qr_code, char *text_post)
{
    if (lvgl_port_lock(0))
    {
        event_target = lv_obj_create(lv_scr_act());
        lv_obj_add_flag(event_target, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_event_cb(event_target, ui_event_handler, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);
        master_page = malloc(sizeof(ui_master_page_t));
        ui_master_page_init(NULL, event_target, false, true, title, master_page);
        lv_obj_t *_container = ui_master_page_get_container(master_page);
        int32_t container_width = 0;
        int32_t container_height = 0;
        ui_master_page_get_container_size(master_page, &container_width, &container_height);
        container = lv_obj_create(_container);
        NO_BODER_PADDING_STYLE(container);
        lv_obj_center(container);
        lv_obj_set_size(container, container_width, container_height);
        lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);

        lv_obj_t *obj = NULL;
        obj = lv_obj_create(container);
        NO_BODER_PADDING_STYLE(obj);
        int obj_width = container_width * 0.95;
        lv_obj_set_size(obj, obj_width, LV_SIZE_CONTENT);
        lv_obj_center(obj);
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *label = NULL;
        if (text_pre != NULL)
        {
            label = lv_label_create(obj);
            lv_label_set_text(label, text_pre);
            lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
            lv_obj_center(label);
        }

        if (qr_code != NULL)
        {
            lv_obj_t *qr = lv_qrcode_create(obj);
            lv_qrcode_set_size(qr, obj_width);
            lv_qrcode_set_dark_color(qr, lv_color_hex(0x000000));
            lv_qrcode_set_light_color(qr, lv_color_hex(0xffffff));
            lv_qrcode_update(qr, qr_code, strlen(qr_code));
            lv_obj_center(qr);
        }

        if (text_post != NULL)
        {
            label = lv_label_create(obj);
            lv_label_set_text(label, text_post);
            lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
            lv_obj_center(label);
        }
        lvgl_port_unlock();
    }
}