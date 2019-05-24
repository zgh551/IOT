# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(PROJECT_PATH)/components/include $(PROJECT_PATH)/components/led/include $(PROJECT_PATH)/components/flash_nvs/include $(PROJECT_PATH)/components/smartconfig/include $(PROJECT_PATH)/components/wifi/include $(PROJECT_PATH)/components/mqtt_ibm/include $(PROJECT_PATH)/components/airkiss/include $(PROJECT_PATH)/components/socket_udp_server/inc $(PROJECT_PATH)/components/json_packet/inc $(PROJECT_PATH)/components/sensor/ccs811/inc $(PROJECT_PATH)/components/sensor/hdc1080/inc $(PROJECT_PATH)/components/driver/i2c_module/inc
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/components -lcomponents $(PROJECT_PATH)/components/airkiss/lib/libairkiss_aes.a $(PROJECT_PATH)/components/airkiss/lib/libairkiss_aes_log.a
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += components
component-components-build: 
