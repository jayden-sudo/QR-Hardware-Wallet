/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_connect_qrcode.h"
#include "ui/ui_style.h"
#include "esp_log.h"
#include "ui/ui_master_page.h"
#include "alloc_utils.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "UI_CONNECT_QRCODE"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct __attribute__((aligned(4)))
{
    ctrl_home_network_data_t *network_data;
    ctrl_home_connect_qr_type qr_type;
    ctrl_home_3rd_wallet_info_t *wallet_info_3rd;

} ui_connect_qrcode_t;

/**********************
 *      MACROS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static int32_t container_width = 0;
static int32_t container_height = 0;
static alloc_utils_memory_struct *alloc_utils_memory_struct_pointer;
static lv_obj_t *choose_wallet_container = NULL;
static lv_obj_t *qrcode_container = NULL;
static lv_obj_t *event_target = NULL;
static ui_master_page_t *master_page = NULL;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_connect_qr_destroy();
static void ui_event_handler(lv_event_t *e);
static void show_qrcode(ui_connect_qrcode_t *ui_connect_qrcode_data);
static void hide_qrcode();

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_connect_qrcode(ctrl_home_network_data_t *network_data);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void show_qrcode(ui_connect_qrcode_t *ui_connect_qrcode_data)
{
    hide_qrcode();
    if (lvgl_port_lock(0))
    {

        /*

        UI:
        ┌───────────────────┐
        │ ┌───────────────┐ │
        │ │    qr code    │ │
        │ └───────────────┘ │
        │  icon WalletName  │
        │  Address          │
        └───────────────────┘

         */

        qrcode_container = lv_obj_create(ui_master_page_get_container(master_page));
        NO_BODER_PADDING_STYLE(qrcode_container);
        lv_obj_set_size(qrcode_container, container_width, container_height);
        lv_obj_set_flex_flow(qrcode_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollbar_mode(qrcode_container, LV_SCROLLBAR_MODE_OFF);

        int qrcode_width = container_width > container_height ? container_height : container_width;
        lv_obj_t *obj = NULL;

        obj = lv_obj_create(qrcode_container);
        NO_BODER_PADDING_STYLE(obj);
        lv_obj_set_size(obj, container_width, LV_SIZE_CONTENT);

        /* logo */
        lv_obj_t *wallet_logo = lv_img_create(obj);
        lv_img_set_src(wallet_logo, ui_connect_qrcode_data->wallet_info_3rd->icon);
        lv_obj_set_pos(wallet_logo, 10, 10);

        /* wallet name */
        lv_obj_t *wallet_name = lv_label_create(obj);
        lv_label_set_text(wallet_name, ui_connect_qrcode_data->wallet_info_3rd->name);
        lv_obj_set_pos(wallet_name, 50, 12);

        /* qr code */
        {
            lv_obj_t *qr = lv_qrcode_create(qrcode_container);
            lv_qrcode_set_size(qr, qrcode_width * 0.95);
            lv_qrcode_set_dark_color(qr, lv_color_hex(0x000000));
            lv_qrcode_set_light_color(qr, lv_color_hex(0xffffff));
            lv_obj_set_style_margin_left(qr, (qrcode_width * (1 - 0.95)) / 2, 0);

            /*Set data*/
            char *qr_code = ctrl_home_get_connect_qrcode(ui_connect_qrcode_data->network_data, ui_connect_qrcode_data->qr_type);
            lv_qrcode_update(qr, qr_code, strlen(qr_code));
            free(qr_code);
            lv_obj_center(qr);
        }

        /* address */
        lv_obj_t *wallet_address = lv_label_create(qrcode_container);
        lv_obj_set_size(wallet_address, container_width - 20, LV_SIZE_CONTENT);
        lv_obj_set_pos(wallet_address, 10, 0);
        lv_label_set_long_mode(wallet_address, LV_LABEL_LONG_WRAP);
        lv_label_set_text(wallet_address, ui_connect_qrcode_data->network_data->address);

        lvgl_port_unlock();
    }
    ui_master_page_set_back_button_visibility(true, master_page);
    ui_master_page_set_title(ui_connect_qrcode_data->wallet_info_3rd->name, master_page);
}
static void hide_qrcode()
{
    if (qrcode_container != NULL)
    {
        if (lvgl_port_lock(0))
        {
            lv_obj_del(qrcode_container);
            qrcode_container = NULL;
            lvgl_port_unlock();
        }
    }
    ui_master_page_set_title("Connect via QR Code", master_page);
}
static void ui_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        ui_connect_qrcode_t *ui_connect_qrcode_data = (ui_connect_qrcode_t *)lv_event_get_user_data(e);
        if (ui_connect_qrcode_data == NULL)
        {
            return;
        }
        show_qrcode(ui_connect_qrcode_data);
    }
    else if (code == UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED)
    {
        ui_master_page_set_back_button_visibility(false, master_page);
        hide_qrcode();
    }
    else if (code == UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED)
    {
        ui_master_page_set_close_button_visibility(false, master_page);
        lv_async_call(ui_connect_qr_destroy, NULL);
    }
}
static void ui_connect_qr_destroy()
{
    if (lvgl_port_lock(0))
    {
        if (qrcode_container != NULL)
        {
            lv_obj_del(qrcode_container);
            qrcode_container = NULL;
        }
        if (choose_wallet_container != NULL)
        {
            lv_obj_del(choose_wallet_container);
            choose_wallet_container = NULL;
        }
        if (event_target != NULL)
        {
            lv_obj_del(event_target);
            event_target = NULL;
        }
        lvgl_port_unlock();
    }

    ui_master_page_free(master_page);

    if (alloc_utils_memory_struct_pointer != NULL)
    {
        ALLOC_UTILS_FREE_MEMORY(alloc_utils_memory_struct_pointer);
    }
    master_page = NULL;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_connect_qrcode(ctrl_home_network_data_t *network_data)
{
    if (network_data == NULL)
    {
        ESP_LOGE(TAG, "network_data is NULL");
        return;
    }
    ALLOC_UTILS_INIT_MEMORY_STRUCT(alloc_utils_memory_struct_pointer);

    if (lvgl_port_lock(0))
    {
        event_target = lv_obj_create(lv_scr_act());
        lv_obj_add_flag(event_target, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_event_cb(event_target, ui_event_handler, UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED, NULL);
        lv_obj_add_event_cb(event_target, ui_event_handler, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);
        ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, master_page, sizeof(ui_master_page_t));
        ui_master_page_create(NULL, event_target, false, true, "Connect via QR Code", master_page);
        lv_obj_t *container = ui_master_page_get_container(master_page);
        ui_master_page_get_container_size(master_page, &container_width, &container_height);

        choose_wallet_container = lv_obj_create(container);
        NO_BODER_PADDING_STYLE(choose_wallet_container);
        lv_obj_set_size(choose_wallet_container, container_width, container_height);
        lv_obj_set_scrollbar_mode(choose_wallet_container, LV_SCROLLBAR_MODE_OFF);

        lv_obj_t *cont_col = lv_obj_create(choose_wallet_container);
        NO_BODER_PADDING_STYLE(cont_col);
        lv_obj_set_size(cont_col, container_width * 0.6, LV_SIZE_CONTENT);
        lv_obj_center(cont_col);
        lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *obj = NULL;
        lv_obj_t *label = NULL;
        ctrl_home_3rd_wallet_info_t *wallet_info_3rd = network_data->compatible_wallet_group->wallet_info_3rd;
        while (wallet_info_3rd)
        {
            {
                obj = lv_button_create(cont_col);
                lv_obj_set_style_margin_bottom(obj, 20, 0);
                lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                lv_obj_t *wallet_icon = lv_img_create(obj);
                lv_img_set_src(wallet_icon, wallet_info_3rd->icon);

                label = lv_label_create(obj);
                NO_BODER_PADDING_STYLE(label);
                lv_label_set_text(label, wallet_info_3rd->name);
                lv_obj_center(label);
                ui_connect_qrcode_t *ui_connect_qrcode_data = NULL;
                ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, ui_connect_qrcode_data, sizeof(ui_connect_qrcode_t));
                ui_connect_qrcode_data->network_data = network_data;
                ui_connect_qrcode_data->qr_type = network_data->compatible_wallet_group->qr_type;
                ui_connect_qrcode_data->wallet_info_3rd = wallet_info_3rd;
                lv_obj_add_event_cb(obj, ui_event_handler, LV_EVENT_CLICKED, ui_connect_qrcode_data);
            }
            wallet_info_3rd = wallet_info_3rd->next;
        }
        lvgl_port_unlock();
    }
}