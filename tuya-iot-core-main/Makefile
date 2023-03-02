include $(TOPDIR)/rules.mk

PKG_NAME:=tuya-iot-core-main
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/tuya/tuya-iot-core-sdk.git
PKG_SOURCE_VERSION:=210d6b58b014c3b6679abcfe06ed5bb2bca06a00
SOURCE_DIR:=$(PLG_BUILD_DIR)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS:=-S $(PKG_BUILD_DIR) -B $(PKG_BUILD_DIR)/build

define Package/tuya-iot-core-main
	CATEGORY:=Libraries
	TITLE:=tuya-iot-core-main
endef

define Package/tuya-iot-core-main/description
	Tuya SDK for communication with Tuya cloud
endef

define Build/InstallDev
	echo "HELLOOOOOOOOOOOO 1"
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	echo "HELLOOOOOOOOOOOO 2"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/liblink_core.so $(1)/usr/lib/
	echo "HELLOOOOOOOOOOOO 3"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libmiddleware_implementation.so $(1)/usr/lib/
	echo "HELLOOOOOOOOOOOO 4"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libplatform_port.so $(1)/usr/lib/
	echo "HELLOOOOOOOOOOOO 5"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libutils_modules.so $(1)/usr/lib/
	echo "HELLOOOOOOOOOOOO 6"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include/
	echo "HELLOOOOOOOOOOOO 7"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/interface/*.h $(1)/usr/include/
	echo "HELLOOOOOOOOOOOO 8"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
	echo "HELLOOOOOOOOOOOO 9"
endef

define Package/tuya-iot-core-main/install
	echo "OOOOOOOOOOOOLLEH 1"
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include
	echo "OOOOOOOOOOOOLLEH 2"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/liblink_core.so $(1)/usr/lib/
	echo "OOOOOOOOOOOOLLEH 3"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libmiddleware_implementation.so $(1)/usr/lib/
	echo "OOOOOOOOOOOOLLEH 4"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libplatform_port.so $(1)/usr/lib/
	echo "OOOOOOOOOOOOLLEH 5"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libutils_modules.so $(1)/usr/lib/
	echo "OOOOOOOOOOOOLLEH 6"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include/
	echo "OOOOOOOOOOOOLLEH 7"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/interface/*.h $(1)/usr/include/
	echo "OOOOOOOOOOOOLLEH 8"
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/*.h $(1)/usr/include/
endef

$(eval $(call BuildPackage,tuya-iot-core-main))

