#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "my_drm.h"

static int drm_fd = -1;
static uint32_t connector_id = 0;
static uint32_t crtc_id = 0;
static drmModeModeInfo mode;
static uint32_t fb_id = 0;
static uint8_t *fb_ptr = NULL;

#define DRM_DEV "/dev/dri/card0"

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	printf("flush cb\n");
    // 顯示全部畫面（簡化版）
    drmModeSetCrtc(drm_fd, crtc_id, fb_id, 0, 0, &connector_id, 1, &mode);
    lv_display_flush_ready(disp);
}



lv_display_t * lv_drm_init(void)
{
    drmModeRes *res = NULL;
    drmModeConnector *conn = NULL;
    drmModeEncoder *enc = NULL;

    drm_fd = open(DRM_DEV, O_RDWR | O_CLOEXEC);
    if (drm_fd < 0) {
        perror("open DRM_DEV");
        return NULL;
    }

    res = drmModeGetResources(drm_fd);
    if (!res) {
        perror("drmModeGetResources");
        close(drm_fd);
        return NULL;
    }

    for (int i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(drm_fd, res->connectors[i]);
        if (conn && conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0) {
            connector_id = conn->connector_id;
            mode = conn->modes[0];
            break;
        }
        drmModeFreeConnector(conn);
        conn = NULL;
    }

    if (!conn) {
        fprintf(stderr, "No connected display found\n");
        drmModeFreeResources(res);
        close(drm_fd);
        return NULL;
    }

    enc = drmModeGetEncoder(drm_fd, conn->encoder_id);
    if (!enc) {
        perror("drmModeGetEncoder");
        drmModeFreeConnector(conn);
        drmModeFreeResources(res);
        close(drm_fd);
        return NULL;
    }

    crtc_id = enc->crtc_id;

    // 建立 dumb buffer
    struct drm_mode_create_dumb create = {
        .width = mode.hdisplay,
        .height = mode.vdisplay,
        .bpp = 32
    };
    ioctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);

    struct drm_mode_map_dumb map = { .handle = create.handle };
    ioctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &map);

    fb_ptr = mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, map.offset);

    drmModeAddFB(drm_fd, mode.hdisplay, mode.vdisplay, 24, 32, create.pitch, create.handle, &fb_id);
	
	memset(fb_ptr, 0xff, create.size);

	drmModeSetCrtc(drm_fd, crtc_id, fb_id, 0, 0, &connector_id, 1, &mode);
    // 建立 LVGL display
    lv_display_t *disp = lv_display_create(mode.hdisplay, mode.vdisplay);
    lv_display_set_flush_cb(disp, flush_cb);

    return disp;
}