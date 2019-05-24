#include "hdc1080.h"

static const char *TAG = "HDC1080";

static void hdc1080_init(i2c_port_t i2c_num)
{
    uint8_t i2c_buff[8];
    // vTaskDelay(100 / portTICK_RATE_MS);
    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,HDC1080_SENSOR_ADDR,SERIAL_ID_F,i2c_buff,2)); 
    ESP_LOGI(TAG, "Device ID:0x%x",i2c_buff[0]);
}


void hdc1080_task_init()
{
    hdc1080_init(I2C_EXAMPLE_MASTER_NUM);
}

