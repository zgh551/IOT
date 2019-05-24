COMPONENT_SRCDIRS := led flash_nvs smartconfig wifi mqtt_ibm airkiss socket_udp_server json_packet/src \
sensor/ccs811/src sensor/hdc1080/src driver/i2c_module/src

COMPONENT_ADD_INCLUDEDIRS += led/include flash_nvs/include smartconfig/include wifi/include mqtt_ibm/include \
airkiss/include socket_udp_server/inc json_packet/inc sensor/ccs811/inc sensor/hdc1080/inc driver/i2c_module/inc

COMPONENT_ADD_LDFLAGS += $(COMPONENT_PATH)/airkiss/lib/libairkiss_aes.a \
                         $(COMPONENT_PATH)/airkiss/lib/libairkiss_aes_log.a
