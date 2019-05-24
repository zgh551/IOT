deps_config := \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/app_update/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/aws_iot/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/esp8266/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/freertos/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/libsodium/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/log/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/lwip/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/mdns/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/mqtt/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/newlib/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/pthread/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/ssl/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/tcpip_adapter/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/wpa_supplicant/Kconfig \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/bootloader/Kconfig.projbuild \
	/home/zgh551/esp/project_template/components/Kconfig.projbuild \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/esptool_py/Kconfig.projbuild \
	/home/zgh551/esp/ESP8266_RTOS_SDK/components/partition_table/Kconfig.projbuild \
	/home/zgh551/esp/ESP8266_RTOS_SDK/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
