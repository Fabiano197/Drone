
DRONELIBSRC := $(DRONELIB)/src/battery.c \
			   $(DRONELIB)/src/motor.c \
			   $(DRONELIB)/src/tof.c \
			   $(DRONELIB)/src/i2c_bus.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_api.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_api_core.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_api_ranging.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_api_strings.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_api_calibration.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_i2c_platform.c \
			   $(DRONELIB)/VL53L0X/src/vl53l0x_platform.c 

DRONELIBINC := $(DRONELIB)/include \
			   $(DRONELIB)/VL53L0X/inc

ALLCSRC += $(DRONELIBSRC)
ALLINC  += $(DRONELIBINC)