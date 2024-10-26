#ifndef UI_PIN_H
#define UI_PIN_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     *      TYPEDEFS
     **********************/
    typedef char *(*verify_function)(char *);

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_pin_set(lv_obj_t *lv_parent, size_t lv_parent_width, size_t lv_parent_height, lv_obj_t *event_target);
    void ui_pin_verify(lv_obj_t *lv_parent, size_t lv_parent_width, size_t lv_parent_height, lv_obj_t *event_target, verify_function verify_fun);
    void ui_pin_destroy(void);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_PIN_H */
