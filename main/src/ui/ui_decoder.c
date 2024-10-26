/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_decoder.h"
#include "ui/ui_style.h"
#include "esp_log.h"
#include "ui/ui_master_page.h"
#include "alloc_utils.h"
#include "ui/ui_events.h"
#include "wallet_db.h"
#include "ui/ui_panic.h"
#include "ui/ui_pin.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "UI_DECODER"
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static alloc_utils_memory_struct *alloc_utils_memory_struct_pointer;
static Wallet *wallet;
static qrcode_protocol_bc_ur_data_t *qrcode_protocol_bc_ur_data;
static lv_obj_t *event_target = NULL;
static ui_master_page_t *master_page = NULL;
static lv_obj_t *container = NULL;
static int32_t container_width = 0;
static int32_t container_height = 0;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_event_handler(lv_event_t *e);
static void ui_decoder_destroy(void *arg);
static char *verify_pin(char *pin_str);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_decoder_init(Wallet *_wallet, qrcode_protocol_bc_ur_data_t *_qrcode_protocol_bc_ur_data, lv_obj_t *event_target);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void ui_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // sign transaction

        wallet_data_version_1_t walletData;
        if (wallet_db_load_wallet_data(&walletData) == false)
        {
            ui_panic("Can't load wallet data", PANIC_REBOOT);
            return;
        }
        if (walletData.signPinRequired)
        {
            ui_pin_verify(container, container_width, container_height, NULL, verify_pin);
        }
        else
        {
            lv_async_call(ui_decoder_destroy, NULL);
            lv_obj_send_event(event_target, UI_EVENT_DECODER_CONFIRM, NULL);
        }
    }
    else if (code == UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED)
    {
        ui_master_page_set_close_button_visibility(false, master_page);
        lv_async_call(ui_decoder_destroy, NULL);
        lv_obj_send_event(event_target, UI_EVENT_DECODER_CANCEL, NULL);
    }
}
static char *verify_pin(char *pin_str)
{
    char *ret = wallet_db_verify_pin(pin_str);
    if (ret == NULL)
    {
        free(wallet_db_pop_private_key());
        lv_async_call(ui_decoder_destroy, NULL);
        lv_obj_send_event(event_target, UI_EVENT_DECODER_CONFIRM, NULL);
    }
    return ret;
}
static void ui_decoder_destroy(void *arg)
{
    wallet = NULL;
    qrcode_protocol_bc_ur_data = NULL;

    ui_pin_destroy();

    if (lvgl_port_lock(0))
    {
        if (container != NULL)
        {
            lv_obj_del(container);
            container = NULL;
        }
        lvgl_port_unlock();
    }

    ui_master_page_destroy(master_page);

    if (alloc_utils_memory_struct_pointer != NULL)
    {
        ALLOC_UTILS_FREE_MEMORY(alloc_utils_memory_struct_pointer);
    }
    master_page = NULL;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_decoder_init(Wallet *_wallet, qrcode_protocol_bc_ur_data_t *_qrcode_protocol_bc_ur_data, lv_obj_t *_event_target)
{
    ui_init_events();
    event_target = _event_target;

    wallet = _wallet;
    qrcode_protocol_bc_ur_data = _qrcode_protocol_bc_ur_data;

    ALLOC_UTILS_INIT_MEMORY_STRUCT(alloc_utils_memory_struct_pointer);

    if (lvgl_port_lock(0))
    {
        lv_obj_add_event_cb(event_target, ui_event_handler, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);
        ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, master_page, sizeof(ui_master_page_t));
        ui_master_page_init(NULL, event_target, false, true, "Transaction", master_page);
        lv_obj_t *_container = ui_master_page_get_container(master_page);
        ui_master_page_get_container_size(master_page, &container_width, &container_height);

        container = lv_obj_create(_container);
        NO_BODER_PADDING_STYLE(container);
        lv_obj_set_size(container, container_width, container_height);
        lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
        int32_t width_col = container_width * 0.9;
        lv_obj_t *cont_col = lv_obj_create(container);
        NO_BODER_PADDING_STYLE(cont_col);
        lv_obj_set_size(cont_col, width_col, LV_SIZE_CONTENT);
        lv_obj_center(cont_col);
        lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *obj = NULL;
        lv_obj_t *label = NULL;

        int footer_height = 60;

        label = lv_label_create(cont_col);
        NO_BODER_PADDING_STYLE(label);
        lv_label_set_text(label, "Transaction decode not implemented yet");
        lv_obj_set_size(label, LV_PCT(100), container_height - footer_height);

        obj = lv_button_create(cont_col);
        lv_obj_set_size(obj, LV_PCT(100), footer_height * 0.8);
        label = lv_label_create(obj);
        lv_label_set_text(label, "Sign");
        lv_obj_center(label);
        lv_obj_add_event_cb(obj, ui_event_handler, LV_EVENT_CLICKED, NULL);
    }
    lvgl_port_unlock();
}