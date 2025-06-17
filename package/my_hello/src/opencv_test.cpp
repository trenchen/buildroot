#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>     // usleep

#include "lvgl.h"

using namespace std;

cv::VideoCapture cap(0);
cv::Mat frame;

lv_image_dsc_t imgdsc;
lv_timer_t * timer;
lv_obj_t * img1;


static void opencv_play_cb(lv_timer_t * timer)
{
	cap >> frame;
	
	if(!frame.empty()){
		//cout << "Channels: " << frame.channels() << " Type: " << frame.type() << std::endl;
		cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
		
		imgdsc.header.w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
		imgdsc.header.h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		imgdsc.header.stride = cap.get(cv::CAP_PROP_FRAME_WIDTH) * 3;
		imgdsc.header.cf = LV_COLOR_FORMAT_RGB888;
		imgdsc.data_size = imgdsc.header.stride * imgdsc.header.h;
		imgdsc.data = frame.data;
		
		cout << "width: " <<  imgdsc.header.w << "height: " << imgdsc.header.h << endl;
		
		lv_image_set_src(img1, &imgdsc);
	}
}


int main(void)
{
	cout << "opencv_test world" << endl;
	cout << cv::getBuildInformation() << endl;
	
	lv_init();
	
	lv_display_t * display = lv_linux_drm_create();
	lv_linux_drm_set_file(display, "/dev/dri/card0", -1 /* use first connector */);
	
	img1 = lv_image_create(lv_screen_active());
    //lv_image_set_src(img1, "S:/mnt/test.png");
	lv_obj_center(img1);
	
	if (!cap.isOpened()) {
		cout << "Cannot open camera" << endl;
        return -1;
    }
	
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
	
	lv_timer_create(opencv_play_cb, 33, NULL);
	


#if 0
	cv::Mat src = cv::imread("/mnt/test.png");
	if(src.empty()){
		cout << "load png failed" << endl;
		return -1;
	}
	
	cv::imwrite("/mnt/testv1.jpg", src);
#endif
	
	
	while (1) {
        lv_timer_handler();  // 處理 LVGL 任務
        usleep(5000);        // 延遲 5ms
    }
	
	return 0;
}

