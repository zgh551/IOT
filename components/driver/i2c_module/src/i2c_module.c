#include "i2c_module.h"

static const char *TAG = "I2C_Module";

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init()
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = 0;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = 0;
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}

/****************************************************************************************************/
//                                      I2C Write Operation
/****************************************************************************************************/
/**
 * @brief i2c write operation, write the regist address with the data
 *
 * 1. send data
 * ___________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | write data_len byte + ack  | stop |
 * --------|---------------------------|-------------------------|----------------------------|------|
 *
 * @param i2c_num I2C port number
 * @param sensor_add the address of the sensor
 * @param reg_address slave reg address
 * @param data data to send
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_master_write_data(i2c_port_t i2c_num, uint8_t sensor_add, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief i2c write operation, only the regist address without the data
 *
 * 1. send data
 * _____________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | stop |
 * --------|---------------------------|-------------------------|------|
 *
 * @param i2c_num I2C port number
 * @param sensor_add the address of the sensor
 * @param reg_address slave reg address
 * @param data data to send
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_master_write_reg(i2c_port_t i2c_num, uint8_t sensor_add, uint8_t reg_address)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/****************************************************************************************************/
//                                      I2C Read Operation
/****************************************************************************************************/
/**
 * @brief i2c read operation, read datas
 * 1. send reg address
 * ______________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | stop |
 * --------|---------------------------|-------------------------|------|
 *
 * 2. read data
 * ___________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | read data_len byte + ack(last nack)  | stop |
 * --------|---------------------------|--------------------------------------|------|
 *
 * @param i2c_num I2C port number
 * @param sensor_add the address of the sensor
 * @param reg_address slave reg address
 * @param data data to read
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_master_read_data(i2c_port_t i2c_num, uint8_t sensor_add, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;

    ESP_LOGI(TAG, "I2C Start");
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | WRITE_BIT, ACK_CHECK_EN);
    ESP_LOGI(TAG, "sensor address:0x%x",sensor_add);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    ESP_LOGI(TAG, "regiter address:0x%x",reg_address);
    i2c_master_stop(cmd);
    ESP_LOGI(TAG, "I2C Stop");
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "I2C Read Data");
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, data_len, LAST_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_master_read_data_continued(i2c_port_t i2c_num, uint8_t sensor_add, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, sensor_add << 1 | READ_BIT, ACK_CHECK_EN);
    // i2c_master_read_byte(cmd, &data[0], ACK_CHECK_EN);
    // i2c_master_read_byte(cmd, &data[1], LAST_NACK_VAL);
    i2c_master_read(cmd, data, data_len,LAST_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// static esp_err_t i2c_master_ccs811_read_reg(i2c_port_t i2c_num)
// {
//     int ret;
//     uint8_t dat;
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, CCS811_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
//     i2c_master_read_byte(cmd,&dat,LAST_NACK_VAL);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);
//     return ret;
// }
// static esp_err_t i2c_master_ccs811_read_test(i2c_port_t i2c_num, uint8_t reg_address, uint8_t *data, size_t data_len)
// {
//     int ret;
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, CCS811_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
//     i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);

//     if (ret != ESP_OK) {
//         return ret;
//     }

//     cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, CCS811_SENSOR_ADDR << 1 | READ_BIT, LAST_NACK_VAL);
//     // i2c_master_read(cmd, data, data_len, LAST_NACK_VAL);
//     i2c_master_stop(cmd);
//     ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
//     i2c_cmd_link_delete(cmd);

//     return ret;
// }