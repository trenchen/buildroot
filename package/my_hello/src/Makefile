LVGL_DIR_NAME ?= lvgl
#LV_DRIVERS_DIR_NAME ?= lv_drivers

LVGL_DIR ?= ${shell pwd}


CSRCS = my_hello.c

include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

OBJEXT          ?= .o
COBJS           = $(CSRCS:.c=$(OBJEXT))
CXXOBJS         = $(CXXSRCS:.cpp=$(OBJEXT))

all: test_lvgl

%.o: %.c lv_conf.h
	$(CC) $(CFLAGS) -I./ -I$(LVGL_DIR)/$(LVGL_DIR_NAME) `pkg-config --cflags libdrm` -o $@ -c $<
#	$(CC) $(CFLAGS) `pkg-config --cflags libdrm` -o $@ -c $<

%.o: %.cpp lv_conf.h
	$(CXX) $(CFLAGS) -I./ -I$(LVGL_DIR)/$(LVGL_DIR_NAME) `pkg-config --cflags libdrm` -o $@ -c $<
			 
test_lvgl : $(COBJS) $(CXXOBJS)
	$(CC) $(CFLAGS) `pkg-config --cflags libdrm` -o $@  $^ $(LDFLAGS) `pkg-config --libs libdrm` -lavformat -lavdevice -lavcodec -lavutil -lswscale -lswresample -lm -lz -lpthread
	
#drm_test : drm_test.c
#	$(CC) $(CFLAGS) `pkg-config --cflags libdrm` -o $@ $^ $(LDFLAGS) `pkg-config --libs libdrm`
	
#drm_test2 : drm_test2.c
#	$(CC) $(CFLAGS) `pkg-config --cflags libdrm` -o $@ $^ $(LDFLAGS) `pkg-config --libs libdrm`
	