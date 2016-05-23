LOCAL_PATH:= $(call my-dir)

# intbufq

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
    intbufq.c \
	getch.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
	#LOCAL_SHARED_LIBRARIES += librt
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_intbufq

include $(BUILD_EXECUTABLE)

# multiplay

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	multiplay.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_multiplay

include $(BUILD_EXECUTABLE)

# engine

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	engine.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_engine

include $(BUILD_EXECUTABLE)

# object

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	object.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_object

include $(BUILD_EXECUTABLE)

# configbq

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	configbq.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_configbq

include $(BUILD_EXECUTABLE)

# reverb

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	reverb.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_reverb

include $(BUILD_EXECUTABLE)

# srcsink

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	srcsink.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_srcsink

include $(BUILD_EXECUTABLE)

# outputmix

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	outputmix.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_outputmix

include $(BUILD_EXECUTABLE)

# urimime

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	urimime.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_urimime

include $(BUILD_EXECUTABLE)

# dim

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	dim.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_dim

include $(BUILD_EXECUTABLE)

# multithread

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	multithread.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_multithread

include $(BUILD_EXECUTABLE)

# playbq

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	playbq.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT \
    libsndfile

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_playbq

# commented out because libsndfile is not yet standard
#include $(BUILD_EXECUTABLE)

# monkey

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := tests

LOCAL_C_INCLUDES:= \
	system/media/opensles/include

LOCAL_SRC_FILES:= \
	monkey.c

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libOpenSLES

LOCAL_STATIC_LIBRARIES := \
    libOpenSLESUT

ifeq ($(TARGET_OS),linux)
	LOCAL_CFLAGS += -DXP_UNIX
endif

LOCAL_CFLAGS += -UNDEBUG

LOCAL_MODULE:= slesTest_monkey

include $(BUILD_EXECUTABLE)
