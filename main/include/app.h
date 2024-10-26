#ifndef APP_H
#define APP_H

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define APP_VERSION "0.0.1"

/* For verification of the firmware's hash, use a static string. */
#define APP_VERSION_RELEASE_DATE "2024-10-24"

/* Salt for rainbow table protection */
#define RAINBOW_TABLE_SALT "Rainbow Table Salt(Change it to your own secret before compiling)"

/*
  Dynamic configuration using the camera: 
    When enabled (set to 1), all known hardware drivers will be compiled.
    During the first startup, the system will initialize by 
    scanning a hardware gpio configuration QR code with the camera.
    Supported board type:
     - Wrover Kit
     - ESP-EYE
     - ESP-S2-KALUGA
     - ESP-S3-EYE
     - ESP-DEVCAM
     - M5STACK-PSRAM
     - M5STACK-WIDE
     - AI-THINKER
 */
#define DYNAMIC_HARDWARE_CONFIG 0

#ifdef __cplusplus
extern "C"
{
#endif

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void app_main(void);

#ifdef __cplusplus
}
#endif

#endif