/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_home.h"
#include "ui/ui_style.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "esp_log.h"
#include "alloc_utils.h"
#include "controller/ctrl_home.h"
#include "ui/ui_toast.h"
#include "ui/ui_master_page.h"
#include "ui/ui_pin.h"
#include "ui/ui_events.h"
#include "wallet_db.h"
#include "app.h"
#include "app_backlight.h"
#include "ui/ui_connect_qrcode.h"
#include "ui/ui_style.h"
#include "ui/ui_qr_code.h"
#include "controller/ctrl_init.h"
#include "freertos/FreeRTOS.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "UI_HOME"
#define ITEM_ICON_WIDTH 20
#define ITEM_ICON_PADDING 10
#define ITEM_HEIGHT_TITLE (ITEM_ICON_WIDTH + ITEM_ICON_PADDING * 2)
#define ITEM_HEIGHT_CONTENT (ITEM_HEIGHT_TITLE + 10)
#define ITEM_HEIGHT (ITEM_HEIGHT_TITLE + ITEM_HEIGHT_CONTENT)

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    SETTINGS_ACTION_LOCK_NOW = 0,
    SETTINGS_ACTION_ERASE_ALL_DATA,
    SETTINGS_ACTION_ERASE_ALL_DATA_CONFIRM,
    SETTINGS_ACTION_ERASE_ALL_DATA_CANCEL,
    SETTINGS_ACTION_INCORRECT_PIN_COUNT_MAX_CHANGED,
    SETTINGS_ACTION_SHOW_GITHUB_PAGE,
} settings_action_t;

typedef enum
{
    PIN_VERIFY_POST_ACTION_ERASE,
    PIN_VERIFY_POST_ACTION_CHANGE_INCORRECT_PIN_COUNT_MAX,
    PIN_VERIFY_POST_ACTION_RESET_PIN,
} pin_verify_post_action_t;

typedef struct
{
    settings_action_t action;
    void *parameter;
} settings_action_data_t;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *tv = NULL;
static lv_obj_t *preview_image = NULL;
static lv_obj_t *incorrect_pin_count_max_dd = NULL;
static alloc_utils_memory_struct *alloc_utils_memory_struct_pointer;
static ui_master_page_t *sub_master_page = NULL;
static pin_verify_post_action_t pin_verify_post_action;
static settings_action_data_t *settings_action_tmp_1 = NULL;
static settings_action_data_t *settings_action_tmp_2 = NULL;
static char temp[64];
static lv_obj_t *erase_msgbox = NULL;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void wallet_list_item_event_handler(lv_event_t *e);
static void create_tab_wallet(lv_obj_t *parent);
static void create_tab_scanner(lv_obj_t *parent);
static void create_tab_settings(lv_obj_t *parent);
static void lv_tabview_event_handler(lv_event_t *e);
static void ui_event_handler(lv_event_t *e);
static char *verify_pin(char *pin_str);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_home_init(void);
void ui_home_destroy(void);
void ui_home_start_qr_scan(void);
void ui_home_stop_qr_scan(void);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wallet_list_item_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        lv_obj_t *clicked_item = lv_event_get_target(e);
        ctrl_home_network_data_t *network_data = (ctrl_home_network_data_t *)lv_obj_get_user_data(clicked_item);
        while (network_data == NULL)
        {
            clicked_item = lv_obj_get_parent(clicked_item);
            network_data = (ctrl_home_network_data_t *)lv_obj_get_user_data(clicked_item);
        }
        if (network_data->compatible_wallet_group == NULL)
        {
            char *text = (char *)malloc(sizeof(char) * (strlen(network_data->name) + 50));
            sprintf(text, "The network %s is not implemented yet.", network_data->name);
            ui_toast_show(text, 2000);
            free(text);
        }
        else
        {
            ui_connect_qrcode_init(network_data);
        }
    }
}
static void create_tab_wallet(lv_obj_t *parent)
{
    NO_BODER_PADDING_STYLE(parent);
    // lv_obj_set_scroll_dir(parent, LV_DIR_NONE);

    int parent_width = lv_obj_get_width(parent);
    lv_obj_t *list = lv_list_create(parent);
    NO_BODER_PADDING_STYLE(list);
    lv_obj_set_style_margin_top(list, 3, 0);
    lv_obj_set_style_margin_bottom(list, 3, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);

    // NO_BODER_PADDING_STYLE(list);
    // lv_obj_set_style_bg_color(list, lv_color_hex(0xff0000), 0);
    lv_obj_set_size(list, parent_width, lv_pct(100));
    lv_obj_center(list);

    ctrl_home_network_data_t *network_data = ctrl_home_list_networks();
    size_t index = 0;
    int padding = ITEM_ICON_PADDING + 2;
    while (network_data != NULL)
    {
        if (index > 0)
        {
            // split line
            lv_obj_t *split_line = lv_obj_create(list);
            lv_obj_set_size(split_line, parent_width - padding, 1);
            lv_obj_set_style_bg_color(split_line, lv_color_hex(0xcfcfcf), 0);
            lv_obj_set_style_margin_left(split_line, ITEM_ICON_PADDING, 0);
        }
        {
            /*
            UI:
                ┌─────┬──────────────┐
                │ Icon│ Network name │
                ├─────┴──────────────┤
                │ Address            │
                └────────────────────┘
             */

            /*Column 1: fix width ${item_height}
             *Column 2: 1 unit from the remaining free space*/
            static int32_t col_dsc[] = {ITEM_HEIGHT_TITLE, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

            /*Row 1: fix width ${item_height/2}
             *Row 2: fix width ${item_height/2}*/
            static int32_t row_dsc[] = {ITEM_HEIGHT_TITLE, ITEM_HEIGHT_CONTENT, LV_GRID_TEMPLATE_LAST};

            /*args */
            /*Create a container with grid*/
            lv_obj_t *cont = lv_obj_create(list);
            lv_obj_set_user_data(cont, (void *)network_data);
            NO_BODER_PADDING_STYLE(cont);
            lv_obj_set_style_pad_row(cont, 0, 0);
            lv_obj_set_style_pad_column(cont, 0, 0);

            /*Disable scroll */
            // lv_obj_set_scroll_dir(cont, LV_DIR_NONE);

            // lv_obj_set_style_bg_color(cont, lv_color_hex(0x00ff00), 0);

            lv_obj_set_size(cont, parent_width, ITEM_HEIGHT);
            lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

            /*icon */
            lv_obj_t *obj = lv_obj_create(cont);
            NO_BODER_PADDING_STYLE(obj);
            lv_obj_set_style_pad_left(obj, ITEM_ICON_PADDING, 0);
            lv_obj_set_style_pad_top(obj, ITEM_ICON_PADDING, 0);
            lv_obj_set_size(obj, ITEM_HEIGHT_TITLE, ITEM_HEIGHT_TITLE);
            lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
                                 LV_GRID_ALIGN_STRETCH, 0, 2);
            lv_obj_t *network_icon = lv_img_create(obj);
            lv_img_set_src(network_icon, network_data->icon);

            /*Chain */
            obj = lv_obj_create(cont);
            NO_BODER_PADDING_STYLE(obj);
            lv_obj_set_style_pad_top(obj, padding, 0);
            lv_obj_set_size(obj, parent_width - ITEM_HEIGHT_TITLE, ITEM_HEIGHT_TITLE);
            // lv_obj_set_style_bg_color(obj, lv_color_hex(0x0ff000), 0);
            lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, 1, 1,
                                 LV_GRID_ALIGN_CENTER, 0, 1);
            lv_obj_t *label = lv_label_create(obj);
            lv_obj_set_style_pad_top(label, 0, 0);
            lv_obj_set_size(label, parent_width - ITEM_HEIGHT_TITLE, ITEM_HEIGHT_TITLE - padding);
            lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(label, wallet_list_item_event_handler, LV_EVENT_CLICKED, NULL);
            lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
            lv_label_set_text(label, network_data->name);

            /*Address */
            obj = lv_obj_create(cont);
            NO_BODER_PADDING_STYLE(obj);
            lv_obj_set_style_pad_left(obj, padding, 0);
            lv_obj_set_style_pad_right(obj, padding, 0);
            lv_obj_set_size(obj, parent_width, ITEM_HEIGHT_CONTENT);
            // lv_obj_set_style_bg_color(obj, lv_color_hex(0xcfcfcf), 0);
            lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, 0, 2,
                                 LV_GRID_ALIGN_CENTER, 1, 1);
            label = lv_label_create(obj);
            /*Disable scroll */
            lv_obj_set_scroll_dir(label, LV_DIR_NONE);
            lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(label, wallet_list_item_event_handler, LV_EVENT_CLICKED, NULL);
            lv_obj_set_size(label, parent_width - 2 * padding, ITEM_HEIGHT_CONTENT);
            lv_obj_set_style_pad_left(label, 5, 0);
            lv_obj_set_style_pad_right(label, 5, 0);
            // auto warp
            lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
            lv_label_set_text(label, network_data->address);
        }
        network_data = (ctrl_home_network_data_t *)network_data->next;
        index++;
    }
}
static void create_tab_scanner(lv_obj_t *parent)
{
    NO_BODER_PADDING_STYLE(parent);

    preview_image = lv_image_create(parent);
    lv_obj_center(preview_image);
    lv_obj_set_size(preview_image, 240, 240);
}
static void create_tab_settings(lv_obj_t *parent)
{
    NO_BODER_PADDING_STYLE(parent);

    lv_obj_t *list = lv_list_create(parent);
    NO_BODER_PADDING_STYLE(list);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);

    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_center(list);

    lv_list_add_text(list, "Lock Now");
    lv_obj_t *btn = lv_list_add_button(list, LV_SYMBOL_CLOSE, "Lock"); // ctrl_home_destroy();

    settings_action_data_t *lock_action = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, lock_action, sizeof(settings_action_data_t));
    lock_action->action = SETTINGS_ACTION_LOCK_NOW;
    lv_obj_add_event_cb(btn, ui_event_handler, LV_EVENT_CLICKED, lock_action);

    lv_list_add_text(list, "Erase All Data");
    btn = lv_list_add_button(list, LV_SYMBOL_TRASH, "Erase Wallet");
    settings_action_data_t *erase_all_data_action = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, erase_all_data_action, sizeof(settings_action_data_t));
    erase_all_data_action->action = SETTINGS_ACTION_ERASE_ALL_DATA;
    lv_obj_add_event_cb(btn, ui_event_handler, LV_EVENT_CLICKED, erase_all_data_action);
    /*
           UI:
            ┌────────┬───────┐
            │  icon  │  DDL  │
            └────────┴───────┘
            */

    lv_list_add_text(list, "To protect your data, all data will be erased if the PIN is entered incorrectly too many times");
    lv_obj_t *ddlwarp = lv_list_add_button(list, LV_SYMBOL_WARNING, NULL);
    lv_obj_remove_flag(ddlwarp, LV_OBJ_FLAG_CLICKABLE);

    incorrect_pin_count_max_dd = lv_dropdown_create(ddlwarp);
    NO_BODER_PADDING_STYLE(incorrect_pin_count_max_dd);
    lv_dropdown_set_options(incorrect_pin_count_max_dd, "After 2 times\n"
                                                        "After 3 times\n"
                                                        "After 4 times\n"
                                                        "After 5 times\n"
                                                        "After 6 times\n"
                                                        "After 7 times\n"
                                                        "After 8 times\n"
                                                        "After 9 times\n"
                                                        "After 10 times");

    // lv_obj_align(dd, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_flex_grow(incorrect_pin_count_max_dd, 1);

    // select current value
    wallet_data_version_1_t walletData;
    wallet_db_load_wallet_data(&walletData);
    lv_dropdown_set_selected(incorrect_pin_count_max_dd, walletData.incorrectPinCountMax - 2);

    settings_action_data_t *incorrect_pin_count_max_changed_action = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, incorrect_pin_count_max_changed_action, sizeof(settings_action_data_t));
    incorrect_pin_count_max_changed_action->action = SETTINGS_ACTION_INCORRECT_PIN_COUNT_MAX_CHANGED;
    lv_obj_add_event_cb(incorrect_pin_count_max_dd, ui_event_handler, LV_EVENT_VALUE_CHANGED, incorrect_pin_count_max_changed_action);

    if (app_backlight_support())
    {
        lv_list_add_text(list, "Backlight");
        btn = lv_list_add_button(list, NULL, "100%");
    }
    lv_list_add_text(list, "About");
    btn = lv_list_add_button(list, NULL, "Github");
    settings_action_data_t *show_github_page_action = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, show_github_page_action, sizeof(settings_action_data_t));
    show_github_page_action->action = SETTINGS_ACTION_SHOW_GITHUB_PAGE;
    lv_obj_add_event_cb(btn, ui_event_handler, LV_EVENT_CLICKED, show_github_page_action);
    char *version_str = NULL;
    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, version_str, sizeof(char) * (strlen(APP_VERSION) + strlen(APP_VERSION_RELEASE_DATE) + 50));
    sprintf(version_str, "Version: %s\nRelease Date: %s", APP_VERSION, APP_VERSION_RELEASE_DATE);
    lv_list_add_button(list, NULL, version_str);
}
static void lv_tabview_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        uint32_t tab_index = lv_tabview_get_tab_active(obj);
        if (tab_index == 1)
        {
            ctrl_home_scan_qr_start(preview_image);
        }
        else
        {
            ctrl_home_scan_qr_stop();
        }
    }
}
static void ui_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        settings_action_data_t *ui_action = (settings_action_data_t *)lv_event_get_user_data(e);
        if (ui_action == NULL)
        {
            return;
        }
        if (ui_action->action == SETTINGS_ACTION_LOCK_NOW)
        {
            ctrl_home_lock_screen();
        }
        else if (ui_action->action == SETTINGS_ACTION_ERASE_ALL_DATA)
        {

            if (lvgl_port_lock(0))
            {
                erase_msgbox = lv_msgbox_create(NULL);
                lv_msgbox_add_title(erase_msgbox, "Erase");
                lv_msgbox_add_text(erase_msgbox, "All data will be erased.");
                lv_obj_set_size(erase_msgbox, lv_pct(100), LV_SIZE_CONTENT);
                lv_obj_t *btn;
                btn = lv_msgbox_add_footer_button(erase_msgbox, "Erase");
                if (settings_action_tmp_1 == NULL)
                {
                    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, settings_action_tmp_1, sizeof(settings_action_data_t));
                }
                settings_action_tmp_1->action = SETTINGS_ACTION_ERASE_ALL_DATA_CONFIRM;
                lv_obj_add_event_cb(btn, ui_event_handler, LV_EVENT_CLICKED, settings_action_tmp_1);
                btn = lv_msgbox_add_footer_button(erase_msgbox, "Cancel");

                if (settings_action_tmp_2 == NULL)
                {
                    ALLOC_UTILS_MALLOC_MEMORY(alloc_utils_memory_struct_pointer, settings_action_tmp_2, sizeof(settings_action_data_t));
                }
                settings_action_tmp_2->action = SETTINGS_ACTION_ERASE_ALL_DATA_CANCEL;
                lv_obj_add_event_cb(btn, ui_event_handler, LV_EVENT_CLICKED, settings_action_tmp_2);

                lvgl_port_unlock();
            }
        }
        else if (ui_action->action == SETTINGS_ACTION_ERASE_ALL_DATA_CONFIRM)
        {
            /* close message box */
            if (lvgl_port_lock(0))
            {
                lv_msgbox_close(erase_msgbox);
                erase_msgbox = NULL;

                lvgl_port_unlock();
            }

            // verify passcode
            if (sub_master_page != NULL)
            {
                ui_master_page_destroy(sub_master_page);
                free(sub_master_page);
                sub_master_page = NULL;
            }

            sub_master_page = malloc(sizeof(ui_master_page_t));
            ui_master_page_init(NULL, tv, false, true, "Erase All Data", sub_master_page);
            int32_t container_width = 0;
            int32_t container_height = 0;
            ui_master_page_get_container_size(sub_master_page, &container_width, &container_height);
            lv_obj_t *container = ui_master_page_get_container(sub_master_page);
            pin_verify_post_action = PIN_VERIFY_POST_ACTION_ERASE;
            ui_pin_verify(container, container_width, container_height, tv, verify_pin);
        }
        else if (ui_action->action == SETTINGS_ACTION_ERASE_ALL_DATA_CANCEL)
        {
            /* close message box */
            if (lvgl_port_lock(0))
            {
                lv_msgbox_close(erase_msgbox);
                erase_msgbox = NULL;
                ESP_LOGI(TAG, "erase_msgbox closed");

                lvgl_port_unlock();
            }
        }
        else if (ui_action->action == SETTINGS_ACTION_SHOW_GITHUB_PAGE)
        {
            ui_qr_code_init("Github", "Homepage", "https://github.com/jayden-sudo/QR-Hardware-Wallet", "https://github.com/jayden-sudo/QR-Hardware-Wallet");
        }
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        settings_action_data_t *ui_action = (settings_action_data_t *)lv_event_get_user_data(e);
        if (ui_action == NULL)
        {
            return;
        }
        if (ui_action->action == SETTINGS_ACTION_INCORRECT_PIN_COUNT_MAX_CHANGED)
        {
            wallet_data_version_1_t walletData;
            wallet_db_load_wallet_data(&walletData);
            int idx = 0;
            if (lvgl_port_lock(0))
            {
                idx = lv_dropdown_get_selected(incorrect_pin_count_max_dd);
                lvgl_port_unlock();
            }

            if (walletData.incorrectPinCountMax - 2 != idx)
            {
                if (sub_master_page != NULL)
                {
                    ui_master_page_destroy(sub_master_page);
                    free(sub_master_page);
                    sub_master_page = NULL;
                }
                sub_master_page = malloc(sizeof(ui_master_page_t));
                sprintf(temp, "Set maximum attempts to %d times", idx + 2);
                ui_master_page_init(NULL, tv, false, true, temp, sub_master_page);
                int32_t container_width = 0;
                int32_t container_height = 0;
                ui_master_page_get_container_size(sub_master_page, &container_width, &container_height);
                lv_obj_t *container = ui_master_page_get_container(sub_master_page);
                pin_verify_post_action = PIN_VERIFY_POST_ACTION_CHANGE_INCORRECT_PIN_COUNT_MAX;
                ui_pin_verify(container, container_width, container_height, tv, verify_pin);
            }
        }
    }
    else if (code == UI_EVENT_PIN_CONFIRM)
    {
        ESP_LOGI(TAG, "UI_EVENT_PIN_CONFIRM");
    }
    else if (code == UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED)
    {
        if (sub_master_page != NULL)
        {
            ui_master_page_destroy(sub_master_page);
            free(sub_master_page);
            sub_master_page = NULL;
        }
        ui_pin_destroy();

        // reset UI
        if (pin_verify_post_action == PIN_VERIFY_POST_ACTION_CHANGE_INCORRECT_PIN_COUNT_MAX)
        {
            wallet_data_version_1_t walletData;
            wallet_db_load_wallet_data(&walletData);
            if (lvgl_port_lock(0))
            {
                lv_dropdown_set_selected(incorrect_pin_count_max_dd, walletData.incorrectPinCountMax - 2);
                lvgl_port_unlock();
            }
        }
    }
}
static char *verify_pin(char *pin_str)
{
    char *ret = wallet_db_verify_pin(pin_str);
    if (ret == NULL)
    {
        free(wallet_db_pop_private_key());

        if (pin_verify_post_action == PIN_VERIFY_POST_ACTION_ERASE)
        {
            wallet_db_reset_device();
        }
        else if (pin_verify_post_action == PIN_VERIFY_POST_ACTION_RESET_PIN)
        {
            // show reset pin page
            // #TODO
        }
        else if (pin_verify_post_action == PIN_VERIFY_POST_ACTION_CHANGE_INCORRECT_PIN_COUNT_MAX)
        {
            wallet_data_version_1_t walletData;
            wallet_db_load_wallet_data(&walletData);
            uint32_t idx = 0;
            if (lvgl_port_lock(0))
            {
                idx = lv_dropdown_get_selected(incorrect_pin_count_max_dd);
                lvgl_port_unlock();
            }
            walletData.incorrectPinCountMax = idx + 2;
            wallet_db_save_wallet_data(&walletData);

            // close master page
            if (sub_master_page != NULL)
            {
                ui_master_page_destroy(sub_master_page);
                free(sub_master_page);
                sub_master_page = NULL;
            }
            ui_pin_destroy();
        }
    }
    return ret;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_home_init(void)
{
    ui_init_events();

    ALLOC_UTILS_INIT_MEMORY_STRUCT(alloc_utils_memory_struct_pointer);
    if (lvgl_port_lock(0))
    {
        tv = lv_tabview_create(lv_scr_act());

        /* sub master page event listener */
        lv_obj_add_event_cb(tv, ui_event_handler, UI_EVENT_PIN_CONFIRM, NULL);
        lv_obj_add_event_cb(tv, ui_event_handler, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);

        if (tv == NULL)
        {
            ESP_LOGE(TAG, "Failed to create tabview");
            return;
        }
        lv_obj_set_size(tv, lv_pct(100), lv_pct(100));
        lv_tabview_set_tab_bar_position(tv, LV_DIR_BOTTOM);
        lv_tabview_set_tab_bar_size(tv, 70);

        lv_obj_t *tab_wallet = lv_tabview_add_tab(tv, "Wallet");
        if (tab_wallet == NULL)
        {
            ESP_LOGE(TAG, "Failed to create tab");
            return;
        }
        lv_obj_t *tab_scanner = lv_tabview_add_tab(tv, "Scanner");
        if (tab_scanner == NULL)
        {
            ESP_LOGE(TAG, "Failed to create tab");
            return;
        }
        lv_obj_t *tab_settings = lv_tabview_add_tab(tv, "Settings");
        if (tab_settings == NULL)
        {
            ESP_LOGE(TAG, "Failed to create tab");
            return;
        }
        create_tab_wallet(tab_wallet);
        create_tab_scanner(tab_scanner);
        create_tab_settings(tab_settings);
        lv_obj_add_event_cb(tv, lv_tabview_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

        lvgl_port_unlock();
    }
}
void ui_home_destroy(void)
{
    if (lvgl_port_lock(0))
    {
        if (tv != NULL)
        {
            lv_obj_del(tv);
            tv = NULL;
        }
        lvgl_port_unlock();
    }

    ALLOC_UTILS_FREE_MEMORY(alloc_utils_memory_struct_pointer);

    settings_action_tmp_1 = NULL;
    settings_action_tmp_2 = NULL;
    if (sub_master_page != NULL)
    {
        ui_master_page_destroy(sub_master_page);
        free(sub_master_page);
        sub_master_page = NULL;
    }

    ui_qr_code_destroy();
    ui_pin_destroy();
    ui_connect_qrcode_destroy();

    if (erase_msgbox != NULL)
    {
        if (lvgl_port_lock(0))
        {
            lv_msgbox_close(erase_msgbox);
            lvgl_port_unlock();
        }
        erase_msgbox = NULL;
    }
}
void ui_home_start_qr_scan(void)
{
    if (tv != NULL)
    {
        if (lvgl_port_lock(0))
        {
            if (lv_tabview_get_tab_active(tv) != 1)
            {
                lv_tabview_set_active(tv, 1, LV_ANIM_OFF);
            }
            lvgl_port_unlock();
        }
    }
}
void ui_home_stop_qr_scan(void)
{
    if (tv != NULL)
    {
        if (lvgl_port_lock(0))
        {
            if (lv_tabview_get_tab_active(tv) != 0)
            {
                lv_tabview_set_active(tv, 0, LV_ANIM_OFF);
            }
            lvgl_port_unlock();
        }
    }
}