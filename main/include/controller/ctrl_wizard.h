#ifndef CTRL_WIZARD_H
#define CTRL_WIZARD_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"
#include "wallet.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ctrl_wizard(char **privateKeyStr);
    void ctrl_wizard_free();
    void ctrl_wizard_set_private_key(char *private_key_str);

#ifdef __cplusplus
}
#endif

#endif /* CTRL_WIZARD_H */
