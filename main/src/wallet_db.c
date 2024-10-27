/*********************
 *      INCLUDES
 *********************/
#include "wallet_db.h"
#include "ui/ui_panic.h"
#include "kv_fs.h"
#include "wallet.h"
#include "ui/ui_loading.h"
#include "esp_log.h"
#include <esp_random.h>
#include <esp_system.h>
#include "crc32.h"
#include "sha256_str.h"
#include "app.h"

/*********************
 *      DEFINES
 *********************/
#define DEFAULT_INCORRECT_PIN_COUNT 5
#define LOCK_SCREEN_TIMEOUT_MS 1000 * 60 * 5 // 5m
#define SIGN_PIN_REQUIRED true
#define VERSION 1

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "wallet_db";
static wallet_data_version_1_t *walletData_cache = NULL;
static char rootPrivateKey_cache[PRIVATE_KEY_SIZE + 1] = {0};
static char temp[64];

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void pin_avoid_rainbow_table(const char *pinStr, const unsigned char padding[32], unsigned char key[32]);
static uint32_t checksum(wallet_data_version_1_t *walletData);
static void reset_task(void *parameters);
size_t wallet_data_to_bin(wallet_data_version_1_t *walletData, char **hex);
void wallet_data_from_bin(wallet_data_version_1_t *walletData, const char *hex, size_t len);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
bool wallet_db_load_wallet_data(wallet_data_version_1_t *walletData);
void wallet_db_save_wallet_data(wallet_data_version_1_t *walletData);
bool wallet_db_init_wallet_data(char *phrase_str, char *pin_str, char **private_key_str);
void wallet_db_clear_cache();
char *wallet_db_verify_pin(char *pin_str);
char *wallet_db_pop_private_key();
void wallet_db_reset_device();
char *wallet_db_passcode_static_error_msg();

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void pin_avoid_rainbow_table(const char *pinStr, const unsigned char padding[32], unsigned char key[32])
{
    size_t len = sizeof(char) * (strlen(pinStr) + strlen(RAINBOW_TABLE_SALT) + 32 + 1);
    char *str = malloc(len);
    if (str == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        abort();
    }
    strcpy(str, pinStr);
    strncpy((char *)str + strlen(pinStr), (char *)padding, 32);
    strncpy((char *)str + strlen(pinStr) + 32, RAINBOW_TABLE_SALT, strlen(RAINBOW_TABLE_SALT));
    str[len - 1] = '\0';
    sha256_str(str, key);
    free(str);
    str = NULL;
}
static uint32_t checksum(wallet_data_version_1_t *walletData)
{
    // copy walletData to m,except `uint32_t checksum`
    size_t size = sizeof(wallet_data_version_1_t) - sizeof(uint32_t);
    unsigned char *m = (unsigned char *)malloc(size);
    memcpy(m, walletData, size);
    uint32_t c = crc32(0, m, size);
    free(m);
    return c;
}
static void reset_task(void *parameters)
{
    ui_loading_show();
    kv_erase();
    esp_restart();
    vTaskDelete(NULL);
}

size_t wallet_data_to_bin(wallet_data_version_1_t *walletData, char **hex)
{
    size_t size = sizeof(wallet_data_version_1_t);
    *hex = (char *)malloc(size);
    if (*hex == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        abort();
    }
    memcpy(*hex, walletData, size);
    return size;
}
void wallet_data_from_bin(wallet_data_version_1_t *walletData, const char *hex, size_t len)
{
    size_t size = sizeof(wallet_data_version_1_t);
    if (len < size)
    {
        ESP_LOGI(TAG, "len < sizeof(wallet_data_version_1_t)");
        memset(walletData, 0, size);
    }
    else
    {
        memcpy(walletData, hex, size);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
bool wallet_db_load_wallet_data(wallet_data_version_1_t *walletData)
{
    if (walletData_cache != NULL)
    {
        memcpy(walletData, walletData_cache, sizeof(wallet_data_version_1_t));
        return true;
    }

    char *hex = NULL;
    size_t len = 0;
    int ret = kv_load(KV_FS_KEY_WALLET, &hex, &len);
    if (ret != 0)
    {
        ui_panic("kv_load failed", PANIC_REBOOT);
    }

    wallet_data_from_bin(walletData, hex, len);
    if (hex != NULL)
    {
        free(hex);
        hex = NULL;
    }
    if (walletData->initialized == true)
    {
        // checksum
        int checksum_value = checksum(walletData);
        if (walletData->checksum != checksum_value)
        {
            wallet_db_reset_device();
            ui_panic("Checksum error, reset wallet...", PANIC_REBOOT);
        }
        else
        {
            if (walletData_cache == NULL)
            {
                walletData_cache = (wallet_data_version_1_t *)malloc(sizeof(wallet_data_version_1_t));
            }
            memcpy(walletData_cache, walletData, sizeof(wallet_data_version_1_t));
            return true;
        }
    }
    return false;
}
void wallet_db_save_wallet_data(wallet_data_version_1_t *walletData)
{
    if (walletData->initialized == false)
    {
        ui_panic("walletData is not initialized", PANIC_REBOOT);
        return;
    }
    walletData->checksum = checksum(walletData);
    char *walletDataStr = NULL;
    size_t len = wallet_data_to_bin(walletData, &walletDataStr);
    if (kv_save(KV_FS_KEY_WALLET, walletDataStr, len) != ESP_OK)
    {
        ui_panic("kv_save failed", PANIC_REBOOT);
        return;
    }
    free(walletDataStr);
    walletDataStr = NULL;
    if (walletData_cache == NULL)
    {
        walletData_cache = (wallet_data_version_1_t *)malloc(sizeof(wallet_data_version_1_t));
    }
    memcpy(walletData_cache, walletData, sizeof(wallet_data_version_1_t));
}
bool wallet_db_init_wallet_data(char *phrase_str, char *pin_str, char **private_key_str)
{
    Wallet wallet = wallet_init_from_mnemonic(phrase_str);
    char *root_private_key = wallet_root_private_key(wallet);
    wallet_data_version_1_t *walletData = malloc(sizeof(wallet_data_version_1_t));
    memset(walletData, 0, sizeof(wallet_data_version_1_t));
    walletData->signPinRequired = SIGN_PIN_REQUIRED;
    esp_fill_random(walletData->pinPadding, 32);
    unsigned char *key = malloc(32);
    pin_avoid_rainbow_table(pin_str, walletData->pinPadding, key);
    aes_encrypt(key,
                (unsigned char *)root_private_key,
                PRIVATE_KEY_SIZE, (unsigned char *)(walletData->privateKey));
    // test decrypt
    {
        unsigned char rootPrivateKey[PRIVATE_KEY_SIZE + 1];
        aes_decrypt(key,
                    (unsigned char *)(walletData->privateKey), PRIVATE_KEY_SIZE,
                    rootPrivateKey);
        if (strcmp(root_private_key, (char *)rootPrivateKey) != 0)
        {
            free(root_private_key);
            root_private_key = NULL;
            ui_panic("decrypt failed", PANIC_REBOOT);
            return false;
        }
        if (strncmp((char *)rootPrivateKey, "xprv", 4) != 0)
        {
            free(root_private_key);
            root_private_key = NULL;
            ui_panic("decrypt failed", PANIC_REBOOT);
            return false;
        }
    }
    free(key);
    key = NULL;

    walletData->initialized = true;
    walletData->version = VERSION;
    walletData->incorrectPinCount = 0;
    walletData->incorrectPinCountMax = DEFAULT_INCORRECT_PIN_COUNT;
    walletData->lockScreenTimeout = LOCK_SCREEN_TIMEOUT_MS;
    wallet_db_save_wallet_data(walletData);
    // read from storage test
    wallet_data_version_1_t *walletData_read = malloc(sizeof(wallet_data_version_1_t));
    if (wallet_db_load_wallet_data(walletData_read) == false)
    {
        ui_panic("load_wallet_data failed", PANIC_REBOOT);
        return false;
    }
    if (walletData_read->initialized == false)
    {
        ui_panic("walletData_read is not initialized", PANIC_REBOOT);
        return false;
    }
    if (strncmp(walletData_read->privateKey, walletData->privateKey, PRIVATE_KEY_SIZE) != 0)
    {
        ui_panic("walletData_read privateKey not match", PANIC_REBOOT);
        return false;
    }
    free(walletData);
    walletData = NULL;
    free(walletData_read);
    walletData_read = NULL;
    wallet_free(wallet);
    wallet = NULL;
    // free(root_private_key);
    // root_private_key = NULL;
    *private_key_str = root_private_key;
    return true;
}
void wallet_db_clear_cache()
{
    if (walletData_cache != NULL)
    {
        free(walletData_cache);
        walletData_cache = NULL;
    }
    memset(temp, 0, sizeof(temp));
}
char *wallet_db_verify_pin(char *pin_str)
{
    if (walletData_cache == NULL)
    {
        ui_panic("walletData_cache is NULL", PANIC_REBOOT);
        return NULL;
    }
    if (walletData_cache->initialized == false)
    {
        ui_panic("walletData_cache is not initialized", PANIC_REBOOT);
        return NULL;
    }
    // check pin
    unsigned char *key = malloc(32);
    pin_avoid_rainbow_table(pin_str, walletData_cache->pinPadding, key);
    unsigned char rootPrivateKey[PRIVATE_KEY_SIZE + 1];
    aes_decrypt(key,
                (unsigned char *)(walletData_cache->privateKey), PRIVATE_KEY_SIZE,
                rootPrivateKey);
    rootPrivateKey[PRIVATE_KEY_SIZE] = '\0';
    free(key);
    key = NULL;
    char *_root_private_key_str = (char *)rootPrivateKey;
    if (strncmp(_root_private_key_str, "xprv", 4) == 0 && strlen(_root_private_key_str) < PRIVATE_KEY_SIZE + 1)
    {
        if (walletData_cache->incorrectPinCount > 0)
        {
            walletData_cache->incorrectPinCount = 0;
            wallet_db_save_wallet_data(walletData_cache);
        }

        strcpy(rootPrivateKey_cache, _root_private_key_str);
        /* verify pin success */
        return NULL;
    }
    else
    {
        if (strncmp(_root_private_key_str, "xprv", 4) == 0)
        {
            // DEBUG
            ESP_LOGE(TAG, "verify pin failed, rootprivateKey len:%d", strlen(_root_private_key_str));
            ESP_LOGE(TAG, "rootprivateKey:%s", _root_private_key_str);
        }

        walletData_cache->incorrectPinCount++;
        if (walletData_cache->incorrectPinCount >= walletData_cache->incorrectPinCountMax)
        {
            wallet_db_reset_device();
            ui_panic("Too many incorrect passcode attempts. Device reset.", PANIC_REBOOT);
            return NULL;
        }
        // save to storage
        wallet_db_save_wallet_data(walletData_cache);
        return wallet_db_passcode_static_error_msg();
    }
}
char *wallet_db_pop_private_key()
{
    char *_privateKey = malloc(PRIVATE_KEY_SIZE + 1);
    strcpy(_privateKey, rootPrivateKey_cache);
    memset(rootPrivateKey_cache, 0, sizeof(rootPrivateKey_cache));
    return _privateKey;
}
void wallet_db_reset_device()
{
    xTaskCreate(reset_task, "reset_task", 4 * 1024, NULL, 1, NULL);
}
char *wallet_db_passcode_static_error_msg()
{
    wallet_data_version_1_t walletData;
    if (wallet_db_load_wallet_data(&walletData) == false)
    {
        return "Can't load wallet data";
    }
    if (walletData.incorrectPinCount > 0)
    {
        sprintf(temp, "Passcode error %d/%d", (walletData.incorrectPinCount) + 1, walletData.incorrectPinCountMax);
        return temp;
    }
    return NULL;
}