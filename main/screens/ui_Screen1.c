// 退休计划 UI，适配 296x128 墨水屏
#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    // 白底屏幕
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_white(), 0);

    // === 标题: 24px 居中 ===
    ui_title = lv_label_create(ui_Screen1);
    lv_obj_set_style_text_font(ui_title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(ui_title, lv_color_black(), 0);
    lv_obj_set_width(ui_title, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_title, LV_SIZE_CONTENT);
    lv_obj_align(ui_title, LV_ALIGN_TOP_MID, 0, 6);
    lv_label_set_text(ui_title, "== RETIRE PLAN ==");

    // === 年份数字: 36px 红色，左侧 ===
    ui_Year = lv_label_create(ui_Screen1);
    lv_obj_set_style_text_font(ui_Year, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_color(ui_Year, lv_color_hex(0xFF0000), 0);
    lv_obj_set_width(ui_Year, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Year, LV_SIZE_CONTENT);
    lv_obj_align(ui_Year, LV_ALIGN_LEFT_MID, 50, -6);
    lv_label_set_text(ui_Year, "35");

    // === 年份标签 "Years" ===
    ui_Label3 = lv_label_create(ui_Screen1);
    lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(ui_Label3, lv_color_black(), 0);
    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);
    lv_obj_align_to(ui_Label3, ui_Year, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_label_set_text(ui_Label3, "Years");

    // === 天数数字: 36px 红色，右侧 ===
    ui_Days = lv_label_create(ui_Screen1);
    lv_obj_set_style_text_font(ui_Days, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_color(ui_Days, lv_color_hex(0xFF0000), 0);
    lv_obj_set_width(ui_Days, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Days, LV_SIZE_CONTENT);
    lv_obj_align(ui_Days, LV_ALIGN_RIGHT_MID, -60, -6);
    lv_label_set_text(ui_Days, "45");

    // === 天数标签 "Days" ===
    lv_obj_t *days_label = lv_label_create(ui_Screen1);
    lv_obj_set_style_text_font(days_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(days_label, lv_color_black(), 0);
    lv_obj_set_width(days_label, LV_SIZE_CONTENT);
    lv_obj_set_height(days_label, LV_SIZE_CONTENT);
    lv_obj_align_to(days_label, ui_Days, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_label_set_text(days_label, "Days");
}
