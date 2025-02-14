#include "ccs811.h"

static const char *TAG = "CCS811";

static void ccs811_init(i2c_port_t i2c_num)
{
    uint8_t i2c_buff[8];
    vTaskDelay(100 / portTICK_RATE_MS);
    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,CCS811_SENSOR_ADDR,HW_ID,i2c_buff,1)); 
    ESP_LOGI(TAG, "Hardware ID:0x%x",i2c_buff[0]);
    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,CCS811_SENSOR_ADDR,HW_VERSION,i2c_buff,2));
    ESP_LOGI(TAG, "Hardware Version:0x%x",i2c_buff[0]);
    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,CCS811_SENSOR_ADDR,FW_BOOT_VERSION,i2c_buff,2));
    ESP_LOGI(TAG, "Fireware Boot Version:0x%x",i2c_buff[0]);
    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,CCS811_SENSOR_ADDR,FW_APP_VERSION,i2c_buff,2));
    ESP_LOGI(TAG, "Fireware App Version:0x%x",i2c_buff[0]);

    ESP_ERROR_CHECK(i2c_master_read_data(i2c_num,CCS811_SENSOR_ADDR,STATUS,i2c_buff,1));
    ESP_LOGI(TAG, "Status:0x%x",i2c_buff[0]);
    if( i2c_buff[0] & 0x10)
    {
        ESP_LOGI(TAG, "Valid application firmware loaded");
        // int ret = i2c_master_write_reg(i2c_num,CCS811_SENSOR_ADDR,APP_START);
        // ESP_LOGI(TAG, "Write Status %d",ret);
        // vTaskDelay(1000 / portTICK_RATE_MS); 
        // ESP_LOGI(TAG, "APP Status:0x%x",i2c_buff[0]);
        // if(i2c_buff[0] & 0x80)
        // {
        //     i2c_buff[0] = DRIVE_MODE_1SEC | INTERRUPT_DRIVEN;
        //     ret = i2c_master_write(i2c_num,CCS811_SENSOR_ADDR,MEAS_MODE, i2c_buff, 1);
        //     ESP_LOGI(TAG, "Write Status %d",ret);
        // }
    }
    else
    {
        ESP_LOGI(TAG, "No application firmware loaded");
    }
}

/**
 * @brief the ccs811 task
*/
static void ccs811_task(void *arg)
{
    ccs811_init(I2C_EXAMPLE_MASTER_NUM);
    while(1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

/**
 * @brief create the cs811 task
*/
void ccs811_task_init()
{
    xTaskCreate(ccs811_task, "ccs811_task", 4096, NULL, 10, NULL);
}
