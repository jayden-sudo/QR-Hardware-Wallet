#ifndef UI_MNEMONIC_H
#define UI_MNEMONIC_H

/*********************
 *      INCLUDES
 *********************/
#include "ui_events.h"
#include "esp_lvgl_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_mnemonic_init(lv_obj_t *lv_parent, size_t lv_parent_width, size_t lv_parent_height, lv_obj_t *event_target, int mnemonic_type);
    void ui_mnemonic_destroy(void);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_MNEMONIC_H */