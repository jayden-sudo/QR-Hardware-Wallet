#ifndef UI_EVENTS_H
#define UI_EVENTS_H

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
     *      VARIABLES
     **********************/
    /* static pointer */
    extern uint32_t UI_EVENT_PHRASE_CONFIRM;
    extern uint32_t UI_EVENT_PHRASE_CANCEL;
    extern uint32_t UI_EVENT_PIN_CONFIRM;
    extern uint32_t UI_EVENT_PIN_CANCEL;
    extern uint32_t UI_EVENT_MASTER_PAGE_BACK_BUTTON_CLICKED;
    extern uint32_t UI_EVENT_MASTER_PAGE_CLOSE_BUTTON_CLICKED;
    extern uint32_t UI_EVENT_DECODER_CANCEL;
    extern uint32_t UI_EVENT_DECODER_CONFIRM;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void ui_init_events(void);

#ifdef __cplusplus
    extern "C"
}
#endif

#endif /* UI_EVENTS_H */
