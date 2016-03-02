LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/current/iozone.c \
	src/current/libbif.c

LOCAL_CFLAGS += \
	-O3 -Dunix -DHAVE_ANSIC_C -DHAVE_PREAD \
	-DNAME='"linux-arm"' -DLINUX_ARM -Dlinux -D__Android__

LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := libc
LOCAL_MODULE:= iozone

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/current/fileop.c

LOCAL_C_FLAGS += \
	-Wall -O3 -Dlinux

LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := libc
LOCAL_MODULE:= fileop

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/current/pit_server.c

LOCAL_C_FLAGS += \
	-Wall -O3 -Dlinux

LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := libc
LOCAL_MODULE:= pit_server

include $(BUILD_EXECUTABLE)
