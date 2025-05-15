#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_mode.h>

#define WIDTH 1920
#define HEIGHT 1080

volatile int running = 1;
void handle_sigint(int _) {
    running = 0;
}

struct framebuffer {
    uint32_t fb_id;
    uint32_t handle;
    uint32_t pitch;
    uint64_t size;
    uint8_t *map;
};

int main() {
    int fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    drmModeRes *res = drmModeGetResources(fd);
    drmModeConnector *conn = NULL;
    drmModeCrtc *crtc = NULL;
    drmModeModeInfo mode;
    uint32_t conn_id = 0, crtc_id = 0;

    for (int i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(fd, res->connectors[i]);
        if (conn->connection == DRM_MODE_CONNECTED) {
            conn_id = conn->connector_id;
            mode = conn->modes[0];
            break;
        }
        drmModeFreeConnector(conn);
        conn = NULL;
    }

    if (!conn) {
        fprintf(stderr, "No connected connector found\n");
        return 1;
    }

   
	drmModeEncoder *enc = drmModeGetEncoder(fd, conn->encoder_id);
    crtc_id = enc->crtc_id;

    struct framebuffer fbs[2];
    memset(fbs, 0, sizeof(fbs));

    for (int i = 0; i < 2; i++) {
        struct drm_mode_create_dumb create = {0};
        create.width = mode.hdisplay;
        create.height = mode.vdisplay;
        create.bpp = 32;
        if (ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) {
            perror("CREATE_DUMB");
            return 1;
        }

        fbs[i].handle = create.handle;
        fbs[i].pitch = create.pitch;
        fbs[i].size = create.size;

        if (drmModeAddFB(fd, mode.hdisplay, mode.vdisplay, 24, 32, create.pitch,
                         create.handle, &fbs[i].fb_id) < 0) {
            perror("AddFB");
            return 1;
        }

        struct drm_mode_map_dumb map = {0};
        map.handle = create.handle;
        if (ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map) < 0) {
            perror("MAP_DUMB");
            return 1;
        }

        fbs[i].map = mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
        if (fbs[i].map == MAP_FAILED) {
            perror("mmap");
            return 1;
        }

        // 填滿不同顏色
        uint32_t color = (i == 0) ? 0xff0000ff : 0xff00ff00;  // Red or Green
        for (uint32_t y = 0; y < mode.vdisplay; y++) {
            for (uint32_t x = 0; x < mode.hdisplay; x++) {
                ((uint32_t *)(fbs[i].map + y * fbs[i].pitch))[x] = color;
            }
        }
    }

    // 初次顯示
    if (drmModeSetCrtc(fd, crtc_id, fbs[0].fb_id, 0, 0, &conn_id, 1, &mode) < 0) {
        perror("SetCrtc");
        return 1;
    }

    signal(SIGINT, handle_sigint);
    printf("Press Ctrl+C to quit\n");

    int current = 0;
    while (running) {
        sleep(1);
        current ^= 1;
        if (drmModePageFlip(fd, crtc_id, fbs[current].fb_id, DRM_MODE_PAGE_FLIP_EVENT, NULL) < 0) {
            perror("PageFlip");
            break;
        }

        // wait for flip to complete
        drmEventContext evctx = { .version = DRM_EVENT_CONTEXT_VERSION, .page_flip_handler = NULL };
        drmHandleEvent(fd, &evctx);
    }

    // 清理
    for (int i = 0; i < 2; i++) {
        drmModeRmFB(fd, fbs[i].fb_id);
        struct drm_mode_destroy_dumb destroy = { .handle = fbs[i].handle };
        munmap(fbs[i].map, fbs[i].size);
        ioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
    }

    drmModeFreeEncoder(enc);
    drmModeFreeConnector(conn);
    drmModeFreeResources(res);
    close(fd);
    return 0;
}
