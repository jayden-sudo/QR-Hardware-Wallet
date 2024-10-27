#ifndef UI_QR_CODE_H
#define UI_QR_CODE_H

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_qr_code_init(char *title, char *text_pre, char *qr_code, char *text_post);
    void ui_qr_code_destroy(void);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_QR_CODE_H */
