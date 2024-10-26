#ifndef WALLET_DB_H
#define WALLET_DB_H

/*********************
 *      INCLUDES
 *********************/
#include "aes_str.h"
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define PRIVATE_KEY_SIZE AES_BLOCK_SIZE * 8

/**********************
 *      TYPEDEFS
 **********************/
typedef struct __attribute__((aligned(4)))
{
    int version;
    bool initialized;
    uint8_t incorrectPinCount;
    uint8_t incorrectPinCountMax;
    bool signPinRequired;
    uint8_t pinPadding[32];
    char privateKey[PRIVATE_KEY_SIZE];
    uint32_t checksum;
} wallet_data_version_1_t;

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif // WALLET_DB_H
