#ifndef UI_STYLE_H
#define UI_STYLE_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"

/**********************
 *      MACROS
 **********************/
#define NO_BODER_PADDING_STYLE(_object)               \
    do                                                \
    {                                                 \
        lv_obj_set_style_radius(_object, 0, 0);       \
        lv_obj_set_style_border_width(_object, 0, 0); \
        lv_obj_set_style_pad_all(_object, 0, 0);      \
        lv_obj_set_style_margin_all(_object, 0, 0);   \
    } while (0)

#endif /* UI_STYLE_H */
