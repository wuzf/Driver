LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := chardevapk
LOCAL_CERTIFICATE := platform
LOCAL_REQUIRED_MODULES := libchardevjni
LOCAL_JNI_SHARED_LIBRARIES := libchardevjni

include $(BUILD_PACKAGE)
include $(LOCAL_PATH)/jni/Android.mk

# Use the folloing include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
