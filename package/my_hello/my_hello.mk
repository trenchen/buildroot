MY_HELLO_VERSION = 1.0
MY_HELLO_SITE = $(TOPDIR)/package/my_hello/src
MY_HELLO_SITE_METHOD = local



define MY_HELLO_BUILD_CMDS	
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef


define MY_HELLO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/test_lvgl $(TARGET_DIR)/usr/sbin
#	$(INSTALL) -D -m 0755 $(@D)/drm_test $(TARGET_DIR)/usr/sbin
#	$(INSTALL) -D -m 0755 $(@D)/drm_test2 $(TARGET_DIR)/usr/sbin
endef


 $(eval $(generic-package))
 