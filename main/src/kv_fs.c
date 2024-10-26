/*********************
 *      INCLUDES
 *********************/
#include "kv_fs.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include "esp_littlefs.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

/*********************
 *      DEFINES
 *********************/
#define BASE_PATH "/wallet"

/**********************
 *      VARIABLES
 **********************/
const char *TAG = "KV_FS";
const esp_vfs_littlefs_conf_t conf = {
    .base_path = BASE_PATH,
    .partition_label = "wallet",
    .format_if_mount_failed = true,
    .dont_mount = false,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int mount_littlefs(void);
static int unmount_littlefs(void);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
int kv_save(const char *key, const char *hex, size_t len);
int kv_load(const char *key, char **hex, size_t *len);
int kv_delete(const char *key);
int kv_erase(void);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int mount_littlefs(void)
{
    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
    }
    return ret;
}

static int unmount_littlefs(void)
{
    esp_err_t ret = esp_vfs_littlefs_unregister(conf.partition_label);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to unmount LittleFS (%s)", esp_err_to_name(ret));
    }
    return ret;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int kv_save(const char *key, const char *hex, size_t len)
{
    if (mount_littlefs() != ESP_OK)
    {
        return ESP_FAIL;
    }

    size_t size = strlen(key) + 10 /* strlen("/wallet/") */;
    char *path = malloc(size);

    sprintf(path, "%s/%s", BASE_PATH, key);
    FILE *f = fopen(path, "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to create file");
        return ESP_FAIL;
    }
    fwrite(hex, sizeof(char), len, f);
    fclose(f);
    free(path);
    unmount_littlefs();
    return ESP_OK;
}
int kv_load(const char *key, char **hex, size_t *len)
{
    if (mount_littlefs() != ESP_OK)
    {
        return ESP_FAIL;
    }

    size_t size = strlen(key) + 10 /* strlen("/wallet/") */;
    char *path = malloc(size);

    sprintf(path, "%s/%s", BASE_PATH, key);

    struct stat st;
    if (stat(path, &st) == 0)
    {
        *hex = malloc(st.st_size);
        if (*hex == NULL)
        {
            ESP_LOGE(TAG, "Failed to allocate memory");
            goto done;
        }
        FILE *f = fopen(path, "rb");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for reading");
            free(*hex);
            *hex = NULL;
            goto done;
        }
        fread(*hex, sizeof(char), st.st_size, f);
        fclose(f);
        *len = st.st_size;
        goto done;
    }
    else
    {
        *len = 0;
        *hex = NULL;
        goto done;
    }

done:
    free(path);
    unmount_littlefs();
    return ESP_OK;
}
int kv_delete(const char *key)
{
    if (mount_littlefs() != ESP_OK)
    {
        return ESP_FAIL;
    }

    size_t size = strlen(key) + 10 /* strlen("/wallet/") */;
    char *path = malloc(size);

    sprintf(path, "%s/%s", BASE_PATH, key);
    struct stat st;
    if (stat(path, &st) == 0)
    {
        // Delete it if it exists
        if (unlink(path) != 0)
        {
            ESP_LOGE(TAG, "Failed to delete file 1");
            free(path);
            unmount_littlefs();
            return ESP_FAIL;
        }
        else
        {
            if (stat(path, &st) == 0)
            {
                ESP_LOGE(TAG, "Failed to delete file 2");
                free(path);
                unmount_littlefs();
                return ESP_FAIL;
            }
        }
    }
    free(path);
    unmount_littlefs();
    return ESP_OK;
}
int kv_erase(void)
{
    esp_err_t ret = esp_littlefs_format(conf.partition_label);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to format LittleFS<1> (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "format success<1>");
    /*
        wallet, data, spiffs,         , 128K
     */
    size_t max_size = 128 * 1024 - 9 * 1024;
    char *fill = malloc(max_size);
    memset(fill, 0, max_size);
    ret = kv_save("fill", fill, max_size);
    free(fill);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save fill");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "save fill success");
    ret = esp_littlefs_format(conf.partition_label);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to format LittleFS<2> (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "format success<2>");
    return ESP_OK;
}