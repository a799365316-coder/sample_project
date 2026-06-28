// EPD 墨水屏 LVGL 显示驱动
// LVGL 在 296x128 帧缓冲区中渲染，flush_cb 将 16 位色转为 EPD 四色格式
#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化 LVGL 并创建墨水屏显示驱动
// epd_rotate: 90=横屏(296x128), 0=竖屏(128x296)
void epd_lvgl_init(int epd_rotate);

// 将 LVGL 帧缓冲转为 EPD 格式并全屏刷新
void epd_lvgl_flush(void);

#ifdef __cplusplus
}
#endif
