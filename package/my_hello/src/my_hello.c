#include <stdio.h>      // perror
#include <stdlib.h>     // malloc, free
#include <unistd.h>     // usleep
#include "lvgl.h"


static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_obj_get_child(lv_event_get_target(e), 0);
    lv_label_set_text(label, "helloworld");
}

int main(void)
{
	printf("hello world\n");
	
	lv_init();
	
	lv_display_t * display = lv_linux_drm_create();
	lv_linux_drm_set_file(display, "/dev/dri/card0", -1 /* use first connector */);
	
	lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event2");
	lv_indev_set_display(touch, display);
	
	lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());  /* Create image Widget for cursor. */
	lv_image_set_src(cursor_obj, LV_SYMBOL_POWER);             /* Set image source. */
	lv_indev_set_cursor(touch, cursor_obj);                 /* Connect image to Input Device. */
	
	lv_obj_t * player = lv_ffmpeg_player_create(lv_screen_active());
	lv_ffmpeg_player_set_video_device();
    lv_ffmpeg_player_set_src(player, "/dev/video0");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
	//lv_image_set_scale( player, 300);
    lv_obj_center(player);
	
#if 0
	 // 建立按鈕與標籤
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "push me");
    lv_obj_center(label);
#endif
	
	 while (1) {
        lv_timer_handler();  // 處理 LVGL 任務
        usleep(5000);        // 延遲 5ms
    }
	
	return 0;
}

