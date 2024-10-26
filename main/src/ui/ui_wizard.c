/*********************
 *      INCLUDES
 *********************/
#include "esp_log.h"
#include "alloc_utils.h"
#include "ui/ui_wizard.h"
#include "ui/ui_master_page.h"
#include "ui/ui_mnemonic.h"
#include "ui/ui_pin.h"
#include "controller/ctrl_wizard.h"
#include "ui/ui_panic.h"
#include "wallet_db.h"
#include "ui/ui_toast.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "UI_WIZARD"
#define TAB_INDEX_CHOOSE_LANGUAGE 0
#define TAB_INDEX_READ_WARNING_MESSAGE 1
#define TAB_INDEX_CHOOSE_MNEMONIC_TYPE 2
#define TAB_INDEX_ENTER_MNEMONIC 3
#define TAB_INDEX_ENTER_PIN 4
#define TAB_INDEX_DONE 5

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    CHOOSE_LANGUAGE = 0,
    READ_WARNING_MESSAGE = 1,
    CHOOSE_MNEMONIC_TYPE = 2,
    DONE = 3,
} tab_action_t;

typedef enum
{
    MNEMONIC_TYPE_12 = 12,
    MNEMONIC_TYPE_24 = 24,
} mnemonic_type_t;

typedef struct
{
    tab_action_t tab_action;
    void *parameter;
} tab_data_t;

/**********************
 *      MACROS
 **********************/
#define NO_PADDING(obj)                           \
    do                                            \
    {                                             \
        lv_obj_set_style_margin_all(obj, 0, 0);   \
        lv_obj_set_style_pad_all(obj, 0, 0);      \
        lv_obj_set_style_radius(obj, 0, 0);       \
        lv_obj_set_style_border_width(obj, 0, 0); \
    } while (0);

/**********************
 *  STATIC VARIABLES
 **********************/
static char *tab_title_list[] = {"Language", "Security", "Mnemonic Type", "Enter Mnemonic", "Enter Passcode", ""};
static lv_obj_t *screen = NULL;
static lv_obj_t *tv = NULL;
static uint32_t tab_index = 0;
static lv_obj_t *tab_language = NULL;
static lv_obj_t *tab_warning_message = NULL;
static lv_obj_t *tab_choose_mnemonic_type = NULL;
static lv_obj_t *tab_enter_mnemonic = NULL;
static lv_obj_t *tab_enter_pin = NULL;
static lv_obj_t *tab_done = NULL;
static alloc_utils_memory_struct *alloc_utils_memory_struct_pointer = NULL;
static ui_master_page_t *ui_master_page = NULL;
static lv_obj_t *container = NULL;
static int32_t container_width = 0;
static int32_t container_height = 0;
static int mnemonic_type = 0;
static char *phrase_cache = NULL;
static char *pin_cache = NULL;
static char *root_private_key = NULL;
static lv_obj_t *btn_done = NULL;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ui_event_handler(lv_event_t *e);
static void on_tab_change(size_t prev_tab_index, size_t next_tab_index);
static void init_tab_language(void);
static void init_tab_warning_message(void);
static void init_tab_choose_mnemonic_type(void);
static void init_tab_done(void);
static void tabview_next_tab(void);
static void tabview_prev_tab(void);
static void task_store_wallet_data(void *parameters);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_wizard_init(void);
void ui_wizard_destroy(void);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void tabview_next_tab(void)
{
    int next_tab_index = tab_index + 1;
    if (next_tab_index > TAB_INDEX_DONE)
    {
        ESP_LOGE(TAG, "next_tab_index is out of range");
        return;
    }
    ui_master_page_set_title(tab_title_list[next_tab_index], ui_master_page);
    ui_master_page_set_back_button_visibility(next_tab_index > 0 && next_tab_index != TAB_INDEX_DONE, ui_master_page);
    if (lvgl_port_lock(0))
    {
        lv_tabview_set_active(tv, next_tab_index, LV_ANIM_ON);
        lvgl_port_unlock();
    }
    on_tab_change(tab_index, next_tab_index);
    tab_index = next_tab_index;
}
static void tabview_prev_tab(void)
{
    int prev_tab_index = tab_index - 1;
    if (prev_tab_index < 0)
    {
        ESP_LOGE(TAG, "prev_tab_index is out of range");
        return;
    }
    ui_master_page_set_title(tab_title_list[prev_tab_index], ui_master_page);
    ui_master_page_set_back_button_visibility(prev_tab_index > 0, ui_master_page);
    if (lvgl_port_lock(0))
    {
        lv_tabview_set_active(tv, prev_tab_index, LV_ANIM_ON);
        lvgl_port_unlock();
    }
    on_tab_change(tab_index, prev_tab_index);
    tab_index = prev_tab_index;
}
static void ui_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        tab_data_t *tab_data = lv_event_get_user_data(e);
        if (tab_data == NULL)
        {
            return;
        }
        if (tab_data->tab_action == CHOOSE_LANGUAGE)
        {
            tabview_next_tab();
        }
        else if (tab_data->tab_action == READ_WARNING_MESSAGE)
        {
            tabview_next_tab();
        }
        else if (tab_data->tab_action == CHOOSE_MNEMONIC_TYPE)
        {
            int *_mnemonic_type = (int *)tab_data->parameter;
            mnemonic_type = (*_mnemonic_type);
            tabview_next_tab();
        }
        else if (tab_data->tab_action == DONE)
        {
            ctrl_wizard_set_private_key(root_private_key);
        }
    }
    else if (code == UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED)
    {
        tabview_prev_tab();
    }
    else if (code == UI_EVENT_PHRASE_CONFIRM)
    {
        char *_phrase = lv_event_get_param(e);
        // free(phrase);
        // *phraseStr = _phrase;
        phrase_cache = _phrase;
        tabview_next_tab();
    }
    else if (code == UI_EVENT_PIN_CONFIRM)
    {
        char *_pin = lv_event_get_param(e);
        // free(_pin);
        //*pinStr = _pin;
        pin_cache = _pin;
        tabview_next_tab();
    }
}
static void on_tab_change(size_t prev_tab_index, size_t next_tab_index)
{
    if (prev_tab_index == TAB_INDEX_ENTER_MNEMONIC)
    {
        // free mnemonic input UI
        ui_mnemonic_destroy();
    }
    else if (prev_tab_index == TAB_INDEX_ENTER_PIN)
    {
        // free pin input UI
        ui_pin_destroy();
    }

    if (next_tab_index == TAB_INDEX_ENTER_MNEMONIC)
    {
        // init mnemonic input UI
        ui_mnemonic_init(tab_enter_mnemonic, container_width, container_height, screen, mnemonic_type);
    }
    else if (next_tab_index == TAB_INDEX_ENTER_PIN)
    {
        // init pin input UI
        ui_pin_set(tab_enter_pin, container_width, container_height, screen);
    }
    else if (next_tab_index == TAB_INDEX_DONE)
    {
        // save to flash
        xTaskCreate(task_store_wallet_data, "task_store_wallet_data", 5 * 1024, NULL, 5, NULL);
    }
}
static void init_tab_language(void)
{
    /* tab_language */
    lv_obj_t *div = lv_obj_create(tab_language);
    lv_obj_set_size(div, container_width, container_height);
    NO_PADDING(div);
    lv_obj_t *cont_col = lv_obj_create(div);
    NO_PADDING(cont_col);
    lv_obj_set_style_pad_bottom(cont_col, 20, 0);
    lv_obj_set_style_pad_top(cont_col, 20, 0);

    lv_obj_set_size(cont_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(cont_col, container, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    {
        lv_obj_t *obj = lv_button_create(cont_col);
        lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, "English");
        lv_obj_center(label);
        tab_data_t *tab_data = NULL;
        ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, tab_data, sizeof(tab_data_t));
        tab_data->tab_action = CHOOSE_LANGUAGE;
        tab_data->parameter = "English";

        lv_obj_add_event_cb(obj, ui_event_handler, LV_EVENT_CLICKED, tab_data);
    }
    lv_obj_center(cont_col);
}
static void init_tab_warning_message(void)
{
    /* tab_warning_message */

    lv_obj_t *cont = lv_obj_create(tab_warning_message);
    lv_obj_set_size(cont, container_width, container_height);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    NO_PADDING(cont);
    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_style_margin_all(label, 10, 0);
    lv_obj_set_size(label, container_width * 0.9, LV_SIZE_CONTENT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "  This wallet only supports importing seed phrases because the hardware lacks an audited true random number generator. Please generate your seed phrase using another trusted software or hardware wallet and then import it here.\n\n  If you forget your passcode, you will lose access to the wallet.\n\n  If the hardware is lost, transfer your assets immediately. The private keys stored on this device could be accessed by others, especially if the bootloader is not encrypted!\n\n  When charging or connecting via USB, always connect directly to a power adapter instead of a computer, as malicious software on the computer could compromise your device.\n\n  Before signing any transaction, double-check the details to ensure you know the information. Never sign data you do not trust!\n\n  To enhance security, the Wi-Fi and Bluetooth has been disabled.\n\n  Following the above guidelines, your wallet is secure.");
    lv_obj_center(label);

    lv_obj_t *footer = lv_obj_create(cont);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_size(footer, container_width, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_bottom(footer, 50, 0);
    lv_obj_t *button = lv_button_create(footer);
    lv_obj_set_size(button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
    label = lv_label_create(button);
    lv_label_set_text(label, "Next");
    lv_obj_center(label);

    tab_data_t *tab_data = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, tab_data, sizeof(tab_data_t));
    tab_data->tab_action = READ_WARNING_MESSAGE;

    lv_obj_add_event_cb(button, ui_event_handler, LV_EVENT_CLICKED, tab_data);
}
static void init_tab_choose_mnemonic_type(void)
{
    /* tab_choose_mnemonic_type */
    lv_obj_t *div = lv_obj_create(tab_choose_mnemonic_type);
    lv_obj_set_size(div, container_width, container_height);
    NO_PADDING(div);
    lv_obj_t *cont_col = lv_obj_create(div);
    NO_PADDING(cont_col);
    lv_obj_set_style_pad_bottom(cont_col, 20, 0);
    lv_obj_set_style_pad_top(cont_col, 20, 0);

    lv_obj_set_size(cont_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(cont_col, container, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    {
        lv_obj_t *obj = lv_button_create(cont_col);
        lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, "24 words");
        lv_obj_center(label);
        tab_data_t *tab_data = NULL;
        ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, tab_data, sizeof(tab_data_t));
        tab_data->tab_action = CHOOSE_MNEMONIC_TYPE;
        static int mnemonic_type_24 = MNEMONIC_TYPE_24;
        tab_data->parameter = (void *)(&mnemonic_type_24);

        lv_obj_add_event_cb(obj, ui_event_handler, LV_EVENT_CLICKED, tab_data);
    }
    lv_obj_center(cont_col);
}
static void init_tab_done(void)
{
    /*
        UI:
        ┌───────────────────────────────────────┐
        │  Welcome to QR-Based Hardware Wallet! │
        │  Your account is successfully set up. │
        │                                       │
        │             ┌───────────┐             │
        │             │   Enter   │             │
        │             └───────────┘             │
        │                                       │
        └───────────────────────────────────────┘
     */

    lv_obj_t *cont = lv_obj_create(tab_done);
    lv_obj_set_size(cont, container_width, container_height);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    NO_PADDING(cont);
    lv_obj_t *label = lv_label_create(cont);
    lv_obj_set_style_margin_all(label, 10, 0);
    lv_obj_set_size(label, container_width * 0.9, LV_SIZE_CONTENT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "\n\n  Welcome to QR Hardware Wallet!\n\n\n  Your account is successfully set up.");
    lv_obj_center(label);

    lv_obj_t *footer = lv_obj_create(cont);
    lv_obj_set_style_border_width(footer, 0, 0);
    lv_obj_set_size(footer, container_width, LV_SIZE_CONTENT);
    btn_done = lv_button_create(footer);
    lv_obj_align(btn_done, LV_ALIGN_CENTER, 0, 0);
    label = lv_label_create(btn_done);
    lv_label_set_text(label, "Enter");
    lv_obj_center(label);
    lv_obj_add_flag(btn_done, LV_OBJ_FLAG_HIDDEN);

    tab_data_t *tab_data = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, tab_data, sizeof(tab_data_t));
    tab_data->tab_action = DONE;
    lv_obj_add_event_cb(btn_done, ui_event_handler, LV_EVENT_CLICKED, tab_data);
}
static void task_store_wallet_data(void *parameters)
{
    if (!wallet_db_init_wallet_data(phrase_cache, pin_cache, &root_private_key))
    {
        ui_panic("init_wallet_data failed", PANIC_REBOOT);
    }
    else
    {
        if (lvgl_port_lock(0))
        {
            lv_obj_remove_flag(btn_done, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(btn_done, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lvgl_port_unlock();
        }
    }

    vTaskDelete(NULL);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_wizard_init()
{
    screen = lv_screen_active();

    ALLOC_UTILS_INIT_MEMORY_STRUCT(alloc_utils_memory_struct_pointer);

    if (lvgl_port_lock(0))
    {
        lv_obj_add_event_cb(screen, ui_event_handler, UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED, NULL);
        lv_obj_add_event_cb(screen, ui_event_handler, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);
        lv_obj_add_event_cb(screen, ui_event_handler, UI_EVENT_PHRASE_CONFIRM, NULL);
        lv_obj_add_event_cb(screen, ui_event_handler, UI_EVENT_PIN_CONFIRM, NULL);
        ui_master_page = malloc(sizeof(ui_master_page_t));
        ui_master_page_init(NULL, screen, false, false, tab_title_list[0], ui_master_page);
        container = ui_master_page_get_container(ui_master_page);
        tv = lv_tabview_create(container);
        NO_PADDING(tv);
        tab_index = TAB_INDEX_CHOOSE_LANGUAGE;
        ui_master_page_get_container_size(ui_master_page, &container_width, &container_height);
        lv_obj_set_size(tv, container_width, container_height);
        // lv_tabview_set_tab_bar_position(tv, LV_DIR_BOTTOM);
        lv_tabview_set_tab_bar_size(tv, 0);
        tab_language = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_language);
        tab_warning_message = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_warning_message);
        tab_choose_mnemonic_type = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_choose_mnemonic_type);
        tab_enter_mnemonic = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_enter_mnemonic);
        tab_enter_pin = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_enter_pin);
        tab_done = lv_tabview_add_tab(tv, NULL);
        NO_PADDING(tab_done);

        init_tab_language();
        init_tab_warning_message();
        init_tab_choose_mnemonic_type();
        init_tab_done();

        lv_obj_remove_flag(lv_tabview_get_content(tv), LV_OBJ_FLAG_SCROLLABLE);

        lvgl_port_unlock();
    }
    ui_toast_show("NOTE:\nSECURE BOOT in current version does not implemented, If the device is lost, your private key may be compromised!", 3000);
}
void ui_wizard_destroy(void)
{
    if (lvgl_port_lock(0))
    {
        ui_master_page_destroy(ui_master_page);
        free(ui_master_page);
        ui_master_page = NULL;
        lvgl_port_unlock();
    }
    if (alloc_utils_memory_struct_pointer != NULL)
    {
        ALLOC_UTILS_FREE_MEMORY(alloc_utils_memory_struct_pointer);
    }
    if (phrase_cache != NULL)
    {
        free(phrase_cache);
        phrase_cache = NULL;
    }
    if (pin_cache != NULL)
    {
        free(pin_cache);
        pin_cache = NULL;
    }
    if (root_private_key != NULL)
    {
        free(root_private_key);
        root_private_key = NULL;
    }
}