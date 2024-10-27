#ifndef UI_CONNECT_QRCODE_H
#define UI_CONNECT_QRCODE_H

/*********************
 *      INCLUDES
 *********************/
#include "controller/ctrl_home.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_connect_qrcode_init(ctrl_home_network_data_t *network_data);
    void ui_connect_qrcode_destroy(void);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_CONNECT_QRCODE_H */
