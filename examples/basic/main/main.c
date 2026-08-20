/**
 * @file main.c
 * @brief Basic usage example for the soldered-bme688 component
 * @author Soldered Electronics
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "soldered-bme688.h"

/* Adjust these to match how the sensor is wired on your board */
#define I2C_PORT    I2C_NUM_0
#define I2C_SDA_GPIO 8
#define I2C_SCL_GPIO 9

static const char *TAG = "bme688-example";

void app_main(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    bme688_handle_t bme688;
    ESP_ERROR_CHECK(bme688_init(bus_handle, BME688_I2C_ADDR_LOW, 100000, &bme688));

    /* Optional: override the defaults set by bme688_init() */
    ESP_ERROR_CHECK(bme688_configure(bme688, BME688_OS_2X, BME688_OS_4X, BME688_OS_1X, BME688_FILTER_OFF));
    ESP_ERROR_CHECK(bme688_set_heater_profile(bme688, 300, 100));

    while (1) {
        bme688_data_t data;
        esp_err_t err = bme688_read(bme688, &data);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "T=%.2f degC  P=%.2f Pa  RH=%.2f %%  gas=%.0f ohm (valid=%d, stable=%d)", data.temperature,
                     data.pressure, data.humidity, data.gas_resistance, data.gas_valid, data.heater_stable);
        } else {
            ESP_LOGE(TAG, "bme688_read failed: %s", esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
