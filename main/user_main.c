/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_log.h"

#include "driver/gpio.h"

//Component 
#include "led.h"
#include "flash_nvs.h"
#include "wifi.h"
#include "smart_config.h"
#include "mqtt_ibm.h"
#include "airkiss.h"
#include "socket_udp_server.h"

#include "cJSON.h"
#include "ccs811.h"
#include "hdc1080.h"
// #include "mqtt_esp_tcp.h"

static const char *TAG = "main";
int cnt =0 ;
/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
        ESP_LOGI(TAG, "system start！");
        printf("SDK version:%s\n", esp_get_idf_version());
        //获取mac地址（station模式）
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        printf("esp_read_mac(): %02x:%02x:%02x:%02x:%02x:%02x \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        char mac_str[12] = {0};
        sprintf(mac_str,"%02x%02x%02x%02x%02x%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf(mac_str);
        esp_chip_info_t chip_id;
        esp_chip_info(&chip_id);
        printf("revision: %d \n", chip_id.revision);
        printf("cores: %d \n", chip_id.cores);
        printf("features: %x \n", chip_id.features);
        printf("--------------------------------------------------------------------------\n\n");

        // cJSON *p_root = cJSON_CreateObject();

        // cJSON_AddStringToObject(p_root,"SDK_Version",esp_get_idf_version());

        // char *s = cJSON_Print(p_root);
        // ESP_LOGI(TAG, s);

        // cJSON_free((void *)s);
        // cJSON_Delete(p_root);

        // led_init();
        flash_nvs_init();
        // smartconfig_init();
        // socket_udp_server_init();
        // mqtt_ibm_init();
        // mqtt_esp_tcp_init();

        if(ESP_OK == i2c_master_init())
        {
                ccs811_task_init();
                hdc1080_task_init();
        }
        while(1)
        {
                // ESP_LOGI(TAG, "cnt: %d\n", cnt++);
                vTaskDelay(1000 / portTICK_RATE_MS);
                // gpio_set_level(16, cnt % 2);
        }
}
