#ifndef UI_MASTER_PAGE_H
#define UI_MASTER_PAGE_H

/*********************
 *      INCLUDES
 *********************/
#include "esp_lvgl_port.h"
#include <stdbool.h>
#include "alloc_utils.h"
#include "ui_events.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     *      TYPEDEFS
     **********************/
    typedef struct
    {
        lv_obj_t *master_page;
        lv_obj_t *container;
        int32_t container_width;
        int32_t container_height;
        lv_obj_t *event_target;
        lv_obj_t *back_button;
        lv_obj_t *title;
        lv_obj_t *close_button;
        alloc_utils_memory_struct *alloc_utils_memory_struct_pointer;
    } ui_master_page_t;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    /**
     * @param ui_master_page The ui_master_page structure, never use struct on the stack!!!
     */
    void ui_master_page_init(lv_obj_t *parent, lv_obj_t *event_target, bool back_button_visibility, bool close_button_visibility, char *title, ui_master_page_t *ui_master_page);
    void ui_master_page_destroy(ui_master_page_t *ui_master_page);
    lv_obj_t *ui_master_page_get_container(ui_master_page_t *ui_master_page);
    void ui_master_page_get_container_size(ui_master_page_t *ui_master_page, int32_t *container_width, int32_t *container_height);
    void ui_master_page_set_title(char *title, ui_master_page_t *ui_master_page);
    void ui_master_page_set_back_button_visibility(bool visibility, ui_master_page_t *ui_master_page);
    void ui_master_page_set_close_button_visibility(bool visibility, ui_master_page_t *ui_master_page);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_MASTER_PAGE_H */
