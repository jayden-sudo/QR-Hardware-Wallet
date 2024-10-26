/*********************
 *      INCLUDES
 *********************/
#include "ui/ui_master_page.h"
#include "string.h"

/*********************
 *      DEFINES
 *********************/
#define HEADER_HEIGHT 30

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void back_event_handler(lv_event_t *e);
static void close_event_handler(lv_event_t *e);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ui_master_page_init(lv_obj_t *parent, lv_obj_t *event_target, bool back_button_visibility, bool close_button_visibility, char *title, ui_master_page_t *ui_master_page);
void ui_master_page_destroy(ui_master_page_t *ui_master_page);
lv_obj_t *ui_master_page_get_container(ui_master_page_t *ui_master_page);
void ui_master_page_get_container_size(ui_master_page_t *ui_master_page, int32_t *container_width, int32_t *container_height);
void ui_master_page_set_title(char *title, ui_master_page_t *ui_master_page);
void ui_master_page_set_back_button_visibility(bool visibility, ui_master_page_t *ui_master_page);
void ui_master_page_set_close_button_visibility(bool visibility, ui_master_page_t *ui_master_page);

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void back_event_handler(lv_event_t *e)
{
    ui_master_page_t *ui_master_page = lv_event_get_user_data(e);
    if (ui_master_page != NULL && ui_master_page->event_target != NULL)
    {
        lv_result_t re = lv_obj_send_event(ui_master_page->event_target, UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED, NULL);
        if (re == LV_RESULT_OK)
        {
            // lv_obj_remove_flag(ui_master_page->back_button, LV_OBJ_FLAG_CLICKABLE);
        }
    }
}
static void close_event_handler(lv_event_t *e)
{
    ui_master_page_t *ui_master_page = lv_event_get_user_data(e);
    if (ui_master_page != NULL && ui_master_page->event_target != NULL)
    {
        lv_result_t re = lv_obj_send_event(ui_master_page->event_target, UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED, NULL);
        if (re == LV_RESULT_OK)
        {
            // lv_obj_remove_flag(ui_master_page->close_button, LV_OBJ_FLAG_CLICKABLE);
        }
    }
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_master_page_init(lv_obj_t *parent, lv_obj_t *event_target, bool back_button_visibility, bool close_button_visibility, char *title, ui_master_page_t *ui_master_page)
{
    /*
    UI
        ┌──────────────────────┐
        │┌────┐           ┌───┐│
        ││ ◄─ │   Title   │ X ││
        │└────┘           └───┘│
        │──────────────────────│
        │                      │
        │                      │
        │        Content       │
        │                      │
        │                      │
        └──────────────────────┘
     */
    lv_obj_t *screen = parent;
    if (parent == NULL)
    {
        screen = lv_scr_act();
    }
    ui_master_page->event_target = event_target;

    ALLOC_UTILS_INIT_MEMORY_STRUCT(ui_master_page->alloc_utils_memory_struct_pointer);

    int screen_width = lv_obj_get_width(screen);
    int screen_height = lv_obj_get_height(screen);

    int content_height = screen_height - HEADER_HEIGHT;

    int title_btn_width = HEADER_HEIGHT * 1.5;
    int title_width = screen_width - title_btn_width * 2;

    if (lvgl_port_lock(0))
    {
        ui_master_page->master_page = lv_obj_create(screen);
        lv_obj_set_size(ui_master_page->master_page, screen_width, screen_height);
        lv_obj_set_style_pad_all(ui_master_page->master_page, 0, 0);
        lv_obj_set_style_border_width(ui_master_page->master_page, 0, 0);

        /* header */

        /* back button */
        lv_obj_t *back_btn = lv_button_create(ui_master_page->master_page);
        lv_obj_set_style_bg_color(back_btn, lv_color_hex(0xaaaaaa), 0);
        lv_obj_t *back_lab = lv_label_create(back_btn);
        lv_label_set_text(back_lab, "<");
        lv_obj_set_style_text_color(back_lab, lv_color_hex(0x000000), 0);
        // lv_obj_set_style_text_color(back_lab, lv_palette_main(LV_PALETTE_GREY), 0);
        // lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
        // lv_obj_set_style_shadow_width(back_btn, 0, 0);
        lv_obj_set_size(back_btn, title_btn_width, HEADER_HEIGHT);
        lv_obj_set_pos(back_btn, 0, 0);
        lv_obj_set_style_radius(back_btn, 0, 0);
        lv_obj_set_style_border_width(back_btn, 0, 0);
        lv_obj_set_style_margin_all(back_btn, 0, 0);
        lv_obj_set_style_pad_all(back_btn, 0, 0);
        lv_obj_set_style_align(back_lab, LV_ALIGN_CENTER, 0);
        ui_master_page->back_button = back_btn;
        lv_obj_add_event_cb(back_btn, back_event_handler, LV_EVENT_CLICKED, ui_master_page);
        if (!back_button_visibility)
        {
            lv_obj_add_flag(back_btn, LV_OBJ_FLAG_HIDDEN);
        }

        /* title */
        lv_obj_t *title_bg = lv_obj_create(ui_master_page->master_page);
        lv_obj_set_scrollbar_mode(title_bg, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_size(title_bg, title_width, HEADER_HEIGHT);
        lv_obj_set_pos(title_bg, title_btn_width, 0);
        lv_obj_set_style_margin_all(title_bg, 0, 0);
        lv_obj_set_style_radius(title_bg, 0, 0);
        lv_obj_set_style_border_width(title_bg, 0, 0);
        lv_obj_t *title_lab = lv_label_create(title_bg);
        lv_obj_set_style_margin_all(title_lab, 0, 0);
        lv_obj_set_size(title_lab, title_width, LV_SIZE_CONTENT);
        lv_label_set_text(title_lab, title);
        lv_obj_set_style_text_align(title_lab, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_align(title_lab, LV_ALIGN_CENTER, 0);
        ui_master_page->title = title_lab;

        /* close button */
        lv_obj_t *close_btn = lv_button_create(ui_master_page->master_page);
        // lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x2E4800), 0);
        lv_obj_set_style_bg_color(close_btn, lv_color_hex(0xFAEA00), 0);
        lv_obj_t *close_lab = lv_label_create(close_btn); 
        lv_label_set_text(close_lab, "X");
        lv_obj_set_style_text_color(close_lab, lv_color_hex(0x000000), 0);
        // lv_obj_set_style_bg_opa(close_btn, LV_OPA_TRANSP, 0);
        // lv_obj_set_style_shadow_width(close_btn, 0, 0);
        lv_obj_set_size(close_btn, title_btn_width, HEADER_HEIGHT);
        lv_obj_set_style_radius(close_btn, 0, 0);
        lv_obj_set_style_border_width(close_btn, 0, 0);
        lv_obj_set_style_margin_all(close_btn, 0, 0);
        lv_obj_set_style_pad_all(close_btn, 0, 0);
        lv_obj_set_style_align(close_lab, LV_ALIGN_CENTER, 0);
        lv_obj_set_pos(close_btn, screen_width - title_btn_width, 0);
        ui_master_page->close_button = close_btn;
        lv_obj_add_event_cb(close_btn, close_event_handler, LV_EVENT_CLICKED, ui_master_page);
        if (!close_button_visibility)
        {
            lv_obj_add_flag(close_btn, LV_OBJ_FLAG_HIDDEN);
        }

        /* content */
        ui_master_page->container = lv_obj_create(ui_master_page->master_page);
        ui_master_page->container_width = screen_width;
        ui_master_page->container_height = content_height;
        lv_obj_set_size(ui_master_page->container, screen_width, content_height);
        lv_obj_set_pos(ui_master_page->container, 0, HEADER_HEIGHT);
        lv_obj_set_style_margin_all(ui_master_page->container, 0, 0);
        lv_obj_set_style_pad_all(ui_master_page->container, 0, 0);
        lv_obj_set_style_radius(ui_master_page->container, 0, 0);
        lv_obj_set_style_border_width(ui_master_page->container, 0, 0);

        lvgl_port_unlock();
    }
}

void ui_master_page_destroy(ui_master_page_t *ui_master_page)
{
    if (ui_master_page == NULL)
    {
        return;
    }
    if (ui_master_page->master_page != NULL)
    {
        if (lvgl_port_lock(0))
        {
            lv_obj_del(ui_master_page->master_page);
            lvgl_port_unlock();
        }
    }
    if (ui_master_page->alloc_utils_memory_struct_pointer != NULL)
    {
        ALLOC_UTILS_FREE_MEMORY(ui_master_page->alloc_utils_memory_struct_pointer);
    }
}

lv_obj_t *ui_master_page_get_container(ui_master_page_t *ui_master_page)
{
    if (ui_master_page == NULL)
    {
        return NULL;
    }
    return ui_master_page->container;
}

void ui_master_page_get_container_size(ui_master_page_t *ui_master_page, int32_t *container_width, int32_t *container_height)
{
    if (ui_master_page == NULL || container_width == NULL || container_height == NULL)
    {
        return;
    }
    *container_width = ui_master_page->container_width;
    *container_height = ui_master_page->container_height;
}
void ui_master_page_set_title(char *title, ui_master_page_t *ui_master_page)
{
    if (ui_master_page == NULL || ui_master_page->title == NULL)
    {
        return;
    }
    if (lvgl_port_lock(0))
    {
        lv_label_set_text(ui_master_page->title, title);
        lvgl_port_unlock();
    }
}
void ui_master_page_set_back_button_visibility(bool visibility, ui_master_page_t *ui_master_page)
{
    if (ui_master_page == NULL || ui_master_page->back_button == NULL)
    {
        return;
    }
    if (lvgl_port_lock(0))
    {
        if (visibility)
        {
            lv_obj_clear_flag(ui_master_page->back_button, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(ui_master_page->back_button, LV_OBJ_FLAG_HIDDEN);
        }
        lvgl_port_unlock();
    }
}
void ui_master_page_set_close_button_visibility(bool visibility, ui_master_page_t *ui_master_page)
{
    if (ui_master_page == NULL || ui_master_page->close_button == NULL)
    {
        return;
    }
    if (lvgl_port_lock(0))
    {
        if (visibility)
        {
            lv_obj_clear_flag(ui_master_page->close_button, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(ui_master_page->close_button, LV_OBJ_FLAG_HIDDEN);
        }
        lvgl_port_unlock();
    }
}
