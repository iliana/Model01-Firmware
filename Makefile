export LOCAL_CFLAGS = -DVERSION="ilianaw-$(shell git describe --abbrev=7 --dirty --always)" -DKALEIDOSCOPE_ENABLE_V1_PLUGIN_API=0 -DKALEIDOSCOPE_HIDADAPTOR_DISABLE_ABSOLUTE_MOUSE

# This stub makefile for a Kaleidoscope plugin pulls in 
# all targets from the Kaleidoscope-Plugin library

SKETCHBOOK_DIR ?= $(CURDIR)
BOARD_HARDWARE_PATH ?= $(SKETCHBOOK_DIR)/hardware
KALEIDOSCOPE_PLUGIN_MAKEFILE_DIR ?= keyboardio/avr/build-tools/makefiles/

include $(BOARD_HARDWARE_PATH)/$(KALEIDOSCOPE_PLUGIN_MAKEFILE_DIR)/rules.mk
