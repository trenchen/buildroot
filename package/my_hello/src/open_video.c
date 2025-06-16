#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 設定影像格式
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fd);
        return 1;
    }

    // 使用 read() 擷取一張影像（前提是驅動支援）
    unsigned char buffer[1024*1024];  // 1MB 暫存
    int len = read(fd, buffer, sizeof(buffer));
    if (len < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("Read %d bytes\n", len);

    // 儲存成 JPEG
    FILE *f = fopen("/tmp/frame.jpg", "wb");
    fwrite(buffer, len, 1, f);
    fclose(f);

    close(fd);
    return 0;
}


#if 0
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

void main(void)
{
	int fp, fp2;
	char buf[640*480*4];
	ssize_t num;
	struct v4l2_format fmt = {0};
	
	printf("hello test!!\n");
	fp = open("/dev/video0", O_RDONLY);
	if(fp == -1)
		printf("error:open /dev/video0 failed\n");
	
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fp, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        close(fp);
        return;
    }
	
	fp2 = open("/tmp/test.jpg", O_CREAT | O_WRONLY, S_IRWXU);
	if(fp2 == -1)
		printf("error:open /tmp/test.jpg failed\n");
	
	if(fp != -1 && fp2 != -1){
		while((num = read(fp, buf, 640*480*4)) > 0){
			printf("read number[%d]\n", num);
			write(fp, buf, num);
		}
		if(num == 0){
			printf("read file done\n");
		}
		
		if(num < 0){
			printf("error:read video failed\n");
		}
	}
	
	if(fp > 0)
		close(fp);
	
	if(fp2 > 0)
		close(fp2);
}

#endif
