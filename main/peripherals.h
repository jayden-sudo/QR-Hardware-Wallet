#ifndef PERIPHERALS_H
#define PERIPHERALS_H

/*********************
 * Camera Module Settings
 *********************/
#define CONFIG_CAMERA_MODULE_WROVER_KIT 0      /*     ─┐                                              */
#define CONFIG_CAMERA_MODULE_ESP_EYE 0         /*      │                                              */
#define CONFIG_CAMERA_MODULE_ESP_S2_KALUGA 0   /*      │                                              */
#define CONFIG_CAMERA_MODULE_ESP_S3_EYE 1      /*      │                                              */
#define CONFIG_CAMERA_MODULE_ESP32_CAM_BOARD 0 /*      ├─  Camera Module Settings                     */
#define CONFIG_CAMERA_MODULE_M5STACK_PSRAM 0   /*      │                                              */
#define CONFIG_CAMERA_MODULE_M5STACK_WIDE 0    /*      │                                              */
#define CONFIG_CAMERA_MODULE_AI_THINKER 0      /*      │                                              */
#define CONFIG_CAMERA_MODULE_CUSTOM 0          /*     ─┘                                              */

#if CONFIG_CAMERA_MODULE_CUSTOM
/* custom camera module setting start */   /*      ─┐                                              */
#define CAMERA_MODULE_CUSTOM_NAME "CUSTOM" /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_PWDN 0    /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_RESET 0   /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_XCLK 0    /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_SIOD 0    /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_SIOC 0    /*       │                                              */
                                           /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D7 0      /*       ├─   CONFIG_CAMERA_MODULE_CUSTOM               */
#define CAMERA_MODULE_CUSTOM_PIN_D6 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D5 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D4 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D3 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D2 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D1 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_D0 0      /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_VSYNC 0   /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_HREF 0    /*       │                                              */
#define CAMERA_MODULE_CUSTOM_PIN_PCLK 0    /*      ─┘                                              */
/* custom camera module setting end */
#endif

#define XCLK_FREQ_HZ 15000000                             /*      ─┐                                         */
/* NOTE: only support PIXFORMAT_RGB565 yet */             /*       │                                         */
#define CAMERA_PIXFORMAT PIXFORMAT_RGB565                 /*       │                                         */
/* NOTE: only support FRAMESIZE_240X240 yet */            /*       │                                         */
#define CAMERA_FRAME_SIZE FRAMESIZE_240X240               /*       ├─ Camera config                          */
#define CAMERA_FB_COUNT 2                                 /*       │                                         */
/* CAMERA_SWAP */                                         /*       │                                         */
/*It’s useful for boards with reversed camera mounting.*/ /*       │                                         */
#define CAMERA_SWAP_X 0                                   /*       │                                         */
#define CAMERA_SWAP_Y 1                                   /*      ─┘                                         */

/*********************
 * LCD Module Settings
 *********************/
// set lcd module here
#define CONFIG_LCD_ST7735 0  /*     ─┐                                            */
#define CONFIG_LCD_ST7789 1  /*      │                                            */
#define CONFIG_LCD_ST7796 0  /*      ├─  Set LCD Module                           */
#define CONFIG_LCD_ILI9341 0 /*      │                                            */
#define CONFIG_LCD_GC9A01 0  /*     ─┘                                            */

// set lcd gpio pins here
#define CONFIG_LCD_GPIO_SCLK 39 /*      ─┐                                       */
#define CONFIG_LCD_GPIO_MOSI 40 /*       │                                       */
#define CONFIG_LCD_GPIO_RST 41  /*       ├─  Set LCD Pins                        */
#define CONFIG_LCD_GPIO_DC 42   /*       │                                       */
#define CONFIG_LCD_GPIO_CS 2    /*       │                                       */
#define CONFIG_LCD_GPIO_BL 1    /*      ─┘                                       */

/* set lcd resolution here */                            /*       ─┐                                           */
#define CONFIG_LCD_H_RES 240                             /*        │                                           */
#define CONFIG_LCD_V_RES 320                             /*        │                                           */
/* set lcd mirror here */                                /*        │                                           */
#define CONFIG_LCD_SWAP_XY 0                             /*        │                                           */
#define CONFIG_LCD_MIRROR_X 0                            /*        │                                           */
#define CONFIG_LCD_MIRROR_Y 0                            /*        │                                           */
/* set lcd other settings here */                        /*        ├─ LCD Config                               */
#define CONFIG_LCD_GPIO_BL_PWM 0                         /*        │                                           */
#define CONFIG_LCD_SPI_NUM (SPI3_HOST)                   /*        │                                           */
#define CONFIG_LCD_PIXEL_CLK_HZ (40 * 1000 * 1000)       /*        │                                           */
#define CONFIG_LCD_CMD_BITS 8                            /*        │                                           */
#define CONFIG_LCD_PARAM_BITS 8                          /*        │                                           */
#define CONFIG_LCD_COLOR_SPACE (ESP_LCD_COLOR_SPACE_BGR) /*        │                                           */
#define CONFIG_LCD_BITS_PER_PIXEL 16                     /*        │                                           */
#define CONFIG_LCD_DRAW_BUFF_DOUBLE 1                    /*        │                                           */
#define CONFIG_LCD_DRAW_BUFF_HEIGHT 50                   /*        │                                           */
#define CONFIG_LCD_BL_ON_LEVEL 1                         /*       ─┘                                           */

/*********************
 * Touch Module Settings
 *********************/
/* Set touch module here */
#define CONFIG_TOUCH_CST816S 0 /*      ─┐                                   */
#define CONFIG_TOUCH_FT5X06 0  /*       │                                   */
#define CONFIG_TOUCH_FT6X36 1  /*       ├─  Set touch module                */
#define CONFIG_TOUCH_GT1151 0  /*       │                                   */
#define CONFIG_TOUCH_GT911 0   /*       │                                   */
#define CONFIG_TOUCH_TT21100 0 /*      ─┘                                   */

/* Touch pins */
#define CONFIG_TOUCH_I2C_SCL 21  /*      ─┐                                 */
#define CONFIG_TOUCH_I2C_SDA 20  /*       ├─  Set touch Pins                */
#define CONFIG_TOUCH_GPIO_INT 19 /*      ─┘                                 */

/* set touch mirror here */            /*        ─┐                                           */
#define CONFIG_TOUCH_SWAP_XY 0         /*         │                                           */
#define CONFIG_TOUCH_MIRROR_X 0        /*         │                                           */
#define CONFIG_TOUCH_MIRROR_Y 0        /*         ├─ Touch Config                             */
/* Touch settings */                   /*         │                                           */
#define CONFIG_TOUCH_I2C_NUM 0         /*         │                                           */
#define CONFIG_TOUCH_I2C_CLK_HZ 400000 /*        ─┘                                           */

#endif // PERIPHERALS_H