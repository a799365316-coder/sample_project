// 墨水屏 LVGL 显示驱动
// 部分缓冲: LVGL 分批渲染 → flush_cb 累积到 PSRAM 全帧缓冲 → epd_lvgl_flush 转码刷屏
#include "epd_lvgl_drv.h"
#include "EPD_2in9g.h"
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUF_ROWS  20  // 部分缓冲行数 (296*20*2 = ~12KB, 放内部 SRAM)
static lv_color_t disp_buf[296 * BUF_ROWS];
static lv_color_t *full_fb = NULL;  // 全帧 296x128 RGB565, PSRAM
static uint8_t   *epd_buf  = NULL;  // EPD 四色 9472 字节, PSRAM

// RGB565 → EPD 四色
static inline uint8_t rgb565_to_epd(lv_color_t c) {
    int r = c.ch.red, g = c.ch.green, b = c.ch.blue;
    if (r > 20 && g < 12 && b < 12) return EPD_2IN9G_RED;
    if (r > 20 && g > 40 && b < 8)  return EPD_2IN9G_YELLOW;
    if (r < 8  && g < 10 && b < 8)  return EPD_2IN9G_BLACK;
    return EPD_2IN9G_WHITE;
}

// 全帧缓冲 → EPD 格式 (90° 旋转: 横 296x128 → 竖 128x296)
static void fb_to_epd(void) {
    memset(epd_buf, 0, 9472);
    for (int ly = 0; ly < 128; ly++) {
        for (int lx = 0; lx < 296; lx++) {
            uint8_t c = rgb565_to_epd(full_fb[ly * 296 + lx]);
            int bx = 127 - ly, by = lx;
            epd_buf[by * 32 + bx / 4] |= (c << ((3 - (bx & 3)) * 2));
        }
    }
}

// LVGL flush 回调: 将每批渲染结果拷贝到全帧缓冲
static void my_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    if (color_p) {
        int w = lv_area_get_width(area);
        for (int y = area->y1; y <= area->y2; y++) {
            memcpy(&full_fb[y * 296 + area->x1],
                   &color_p[(y - area->y1) * w],
                   w * sizeof(lv_color_t));
        }
    }
    lv_disp_flush_ready(drv);
}

// LVGL 1ms tick
static void lvgl_tick_cb(void *arg) {
    (void)arg;
    lv_tick_inc(1);
}

void epd_lvgl_init(int epd_rotate) {
    (void)epd_rotate;

    full_fb = (lv_color_t *)heap_caps_malloc(296 * 128 * sizeof(lv_color_t),
                                               MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(full_fb);
    memset(full_fb, 0xFF, 296 * 128 * sizeof(lv_color_t));

    epd_buf = (uint8_t *)heap_caps_malloc(9472, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(epd_buf);

    // 1ms tick 定时器
    esp_timer_handle_t tick_timer;
    const esp_timer_create_args_t tick_args = {
        .callback = lvgl_tick_cb, .name = "lvgl_tick"
    };
    esp_timer_create(&tick_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000);

    // 显示驱动
    lv_init();

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, 296 * BUF_ROWS);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 296;
    disp_drv.ver_res = 128;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = my_flush_cb;
    lv_disp_drv_register(&disp_drv);

    printf("LVGL EPD driver ready (sync, %d-row partial buf).\r\n", BUF_ROWS);
}

void epd_lvgl_flush(void) {
    // invalidate 全屏，触发 LVGL 完整重绘
    lv_obj_invalidate(lv_scr_act());

    // 轮询 lv_timer_handler 约 3 秒，等所有批次渲染 + flush_cb 完成
    for (int i = 0; i < 150; i++) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // 转码 + 全屏刷新
    fb_to_epd();
    EPD_2IN9G_Init_Fast();
    EPD_2IN9G_Display(epd_buf);
    EPD_2IN9G_Sleep();

    printf("Flush done.\r\n");
}
