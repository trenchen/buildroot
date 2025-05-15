#include <stdio.h>      // perror
#include <stdlib.h>     // malloc, free
#include <unistd.h>     // usleep
#include "lvgl.h"
#include "my_drm.h"


static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_obj_get_child(lv_event_get_target(e), 0);
    lv_label_set_text(label, "按下了！");
}

int main(void)
{
	printf("hello world\n");
	
	lv_init();
	#if 0
	lv_display_t *disp = lv_drm_init();
    if (!disp) {
        printf("lv_drm_init failed\n");
        return 1;
    }
	#endif 
	
	lv_display_t * display = lv_linux_drm_create();
	lv_linux_drm_set_file(display, "/dev/dri/card0", -1 /* use first connector */);
	
	 // 建立按鈕與標籤
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "push me");
    lv_obj_center(label);
	
	 while (1) {
        lv_timer_handler();  // 處理 LVGL 任務
        usleep(5000);        // 延遲 5ms
    }
	
	return 0;
}


#if 0
static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_obj_get_child(lv_event_get_target(e), 0);
    lv_label_set_text(label, "按下了！");
}

int main(void)
{
    lv_init();          // 初始化 LVGL 核心
    lv_drm_init();      // 初始化 DRM，設定 LVGL_DISPLAY_HOR_RES / VER_RES

    int hor_res = LVGL_DISPLAY_HOR_RES;
    int ver_res = LVGL_DISPLAY_VER_RES;
    int buffer_lines = 100; // 每次畫 100 行（調整此值可平衡 RAM 與效能）

    size_t buffer_size = hor_res * buffer_lines * sizeof(lv_color_t);
    lv_color_t * buf1 = malloc(buffer_size);
    if (!buf1) {
        perror("malloc failed for lvgl draw buffer");
        return 1;
    }

    // 初始化 draw buffer 結構
    static lv_draw_buf_t draw_buf;
    lv_draw_buf_init(&draw_buf, hor_res, ver_res, LV_COLOR_FORMAT_NATIVE);
    lv_draw_buf_set_buf(&draw_buf, buf1, NULL, buffer_size);

    // 建立 LVGL 顯示器
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    lv_display_set_flush_cb(disp, lv_drm_flush);
    lv_display_set_draw_buffers(disp, &draw_buf, NULL);

    // 建立按鈕與標籤
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "按我！");
    lv_obj_center(label);

    // 主迴圈
    while (1) {
        lv_timer_handler();  // 處理 LVGL 任務
        usleep(5000);        // 延遲 5ms
    }

    // 不太可能執行到，但良好習慣
    free(buf1);
    return 0;
}
#endif

