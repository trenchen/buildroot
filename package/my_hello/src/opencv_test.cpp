#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>     // usleep

#include "lvgl.h"

using namespace std;

int main(void)
{
	cout << "opencv_test world" << endl;
	//cout << cv::getBuildInformation() << endl;
	
		lv_init();
	
	lv_display_t * display = lv_linux_drm_create();
	lv_linux_drm_set_file(display, "/dev/dri/card0", -1 /* use first connector */);
	
	lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, "S:/mnt/test.png");
	lv_obj_center(img1);
	
	cv::Mat src = cv::imread("/mnt/test.png");
	if(src.empty()){
		cout << "load png failed" << endl;
		return -1;
	}
	
	cv::imwrite("/mnt/testv1.jpg", src);
	
	
	
	while (1) {
        lv_timer_handler();  // 處理 LVGL 任務
        usleep(5000);        // 延遲 5ms
    }
	
	return 0;
}

