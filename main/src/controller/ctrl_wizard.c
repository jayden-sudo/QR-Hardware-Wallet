/*********************
 *      INCLUDES
 *********************/
#include "controller/ctrl_wizard.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "kv_fs.h"
#include <string.h>
#include "ui/ui_pin.h"
#include "ui/ui_wizard.h"
#include "qrcode_protocol.h"
#include "wallet.h"
#include "ui/ui_events.h"
#include "app_peripherals.h"
#include "ui/ui_toast.h"
#include "wallet_db.h"
#include "ui/ui_panic.h"

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    STEP_INIT = 0,
    STEP_VERIFY_PIN,
} UI_STEP;

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "CTRL_WIZARD";
static char **_privateKeyStr = NULL;
static wallet_data_version_1_t *walletData_cache = NULL;
static UI_STEP current_step;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static char *verify_pin(char *pin);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ctrl_wizard(char **privateKeyStr);
void ctrl_wizard_free();
void save_wallet_data(wallet_data_version_1_t *walletData);
void ctrl_wizard_set_private_key(char *privateKeyStr);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static char *verify_pin(char *pin_str)
{
    // return NULL if pin is correct
    if (STEP_VERIFY_PIN == current_step)
    {
        char *ret = wallet_db_verify_pin(pin_str);
        if (ret == NULL)
        {
            char *rootPrivateKey = wallet_db_pop_private_key();
            ctrl_wizard_set_private_key(rootPrivateKey);
            free(rootPrivateKey);
            rootPrivateKey = NULL;
        }
        return ret;
    }
    else
    {
        ESP_LOGE(TAG, "ERROR step: %d", current_step);
        return "Unknown error";
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ctrl_wizard(char **privateKeyStr)
{
    // reset wallet data
    // kv_delete(KV_FS_KEY_WALLET);
    _privateKeyStr = privateKeyStr;
    // ui_loading_show();
    walletData_cache = malloc(sizeof(wallet_data_version_1_t));
    memset(walletData_cache, 0, sizeof(wallet_data_version_1_t));
    lv_obj_t *screen = lv_scr_act();
    ui_init_events();

    /* check if wallet is initialized */
    bool initialized = false;
    if (wallet_db_load_wallet_data(walletData_cache))
    {
        initialized = walletData_cache->initialized;
    }

    /* hide loading */
    // ui_loading_hide();

    if (!initialized)
    {
        current_step = STEP_INIT;
        free(walletData_cache);
        walletData_cache = NULL;
        ui_wizard();
    }
    else
    {
        current_step = STEP_VERIFY_PIN;
        // lv_obj_add_event_cb(screen, ui_event_handler, UI_EVENT_PIN_CONFIRM, NULL);
        /* show pin verify page */
        ui_pin_verify(screen, lv_obj_get_width(screen), lv_obj_get_height(screen), screen, verify_pin);
    }
}
void ctrl_wizard_free()
{
    if (walletData_cache != NULL)
    {
        free(walletData_cache);
        walletData_cache = NULL;
    }
    if (current_step == STEP_INIT)
    {
        ui_wizard_free();
    }
    else
    {
        ui_pin_free();
    }
}
void ctrl_wizard_set_private_key(char *privateKeyStr)
{
    *_privateKeyStr = malloc(PRIVATE_KEY_SIZE + 1);
    strcpy(*_privateKeyStr, privateKeyStr);
}
