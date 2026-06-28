/*****************************************************************************
* | 文件名        :   main.cpp
* | 功能          :   LVGL 倒计时演示，墨水屏 296x128 横屏显示
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "EPD_2in9g.h"
#include "epd_lvgl_drv.h"
#include "ui.h"

extern "C" void app_main(void)
{
    printf("LVGL EPD Demo\r\n");
    if (DEV_Module_Init() != 0) { return; }

    // 1. EPD 初始化 + 清屏
    printf("e-Paper Init...\r\n");
    EPD_2IN9G_Init();
    EPD_2IN9G_Clear(EPD_2IN9G_WHITE);
    DEV_Delay_ms(2000);

    // 2. LVGL 显示驱动初始化 (横屏 296x128)
    printf("LVGL Init...\r\n");
    epd_lvgl_init(90);

    // 3. SquareLine UI 初始化
    printf("UI Init...\r\n");
    ui_init();

    // 4. 首次刷新
    printf("First flush\r\n");
    epd_lvgl_flush();

    // 5. 倒计时循环: 年/天递减
    int years = 35, days = 45;
    for (int minute = 10; minute >= 0; minute--) {
        if (minute > 0) {
            printf("Wait 57s for next update...\r\n");
            DEV_Delay_ms(57000);
        }

        // 更新倒计时数字
        days--;
        if (days < 0) {
            years--;
            days = 364;
            lv_label_set_text_fmt(ui_Year, "%d", years);
        }
        lv_label_set_text_fmt(ui_Days, "%d", days);

        printf("Update: %d years, %d days\r\n", years, days);
        epd_lvgl_flush();
    }

    printf("Demo done.\r\n");
    while (1) { vTaskDelay(pdMS_TO_TICKS(10000)); }
}
