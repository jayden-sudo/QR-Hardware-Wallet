#ifndef UI_DECODER_H
#define UI_DECODER_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"
#include "qrcode_protocol.h"
#include "wallet.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_decoder_init(Wallet *_wallet, qrcode_protocol_bc_ur_data_t *_qrcode_protocol_bc_ur_data, lv_obj_t *event_target);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_DECODER_H */
