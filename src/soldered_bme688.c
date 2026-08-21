/**
 * @file soldered_bme688.c
 * @brief Implementation for the soldered-bme688 component
 * @author Soldered Electronics
 */

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "soldered_bme688.h"
#include "bme68x.h"

struct bme688_dev {
    i2c_master_dev_handle_t i2c_dev;
    struct bme68x_dev bme;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
};

/* BME688 I2C multi-byte writes are NOT auto-incremented on the bus: every data byte must be
 * preceded by its own register address (datasheet section 6.2.1). bme68x_set_regs() already
 * builds that [addr, data, addr, data, ...] stream for us; we only need to prepend the leading
 * address byte and send it as a single transaction. Reads, unlike writes, do auto-increment. */
static BME68X_INTF_RET_TYPE bme688_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    i2c_master_dev_handle_t i2c_dev = (i2c_master_dev_handle_t)intf_ptr;
    esp_err_t err = i2c_master_transmit_receive(i2c_dev, &reg_addr, 1, reg_data, len, pdMS_TO_TICKS(100));
    return (err == ESP_OK) ? BME68X_OK : BME68X_E_COM_FAIL;
}

static BME68X_INTF_RET_TYPE bme688_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    i2c_master_dev_handle_t i2c_dev = (i2c_master_dev_handle_t)intf_ptr;
    uint8_t buf[BME68X_LEN_INTERLEAVE_BUFF];
    if (len + 1 > sizeof(buf)) {
        return BME68X_E_INVALID_LENGTH;
    }
    buf[0] = reg_addr;
    memcpy(&buf[1], reg_data, len);
    esp_err_t err = i2c_master_transmit(i2c_dev, buf, len + 1, pdMS_TO_TICKS(100));
    return (err == ESP_OK) ? BME68X_OK : BME68X_E_COM_FAIL;
}

static void bme688_delay_us(uint32_t period_us, void *intf_ptr)
{
    (void)intf_ptr;
    if (period_us >= 1000) {
        vTaskDelay(pdMS_TO_TICKS(period_us / 1000) + 1);
    } else {
        esp_rom_delay_us(period_us);
    }
}

esp_err_t bme688_init(i2c_master_bus_handle_t bus_handle, uint8_t i2c_addr, uint32_t scl_speed_hz,
                      bme688_handle_t *out_handle)
{
    if (!bus_handle || !out_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    bme688_handle_t dev = calloc(1, sizeof(*dev));
    if (!dev) {
        return ESP_ERR_NO_MEM;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = scl_speed_hz,
    };
    esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev->i2c_dev);
    if (err != ESP_OK) {
        free(dev);
        return err;
    }

    dev->bme.intf = BME68X_I2C_INTF;
    dev->bme.read = bme688_i2c_read;
    dev->bme.write = bme688_i2c_write;
    dev->bme.delay_us = bme688_delay_us;
    dev->bme.intf_ptr = dev->i2c_dev;
    dev->bme.amb_temp = 25;

    int8_t rslt = bme68x_init(&dev->bme);
    if (rslt != BME68X_OK) {
        i2c_master_bus_rm_device(dev->i2c_dev);
        free(dev);
        return (rslt == BME68X_E_DEV_NOT_FOUND) ? ESP_ERR_NOT_FOUND : ESP_ERR_INVALID_RESPONSE;
    }

    /* Sane defaults (Bosch's own recommended indoor-air-quality settings) so a fresh handle is
     * usable right away; bme688_configure() / bme688_set_heater_profile() override these later. */
    dev->conf.os_temp = BME68X_OS_2X;
    dev->conf.os_pres = BME68X_OS_4X;
    dev->conf.os_hum = BME68X_OS_1X;
    dev->conf.filter = BME68X_FILTER_OFF;
    dev->conf.odr = BME68X_ODR_NONE;
    if (bme68x_set_conf(&dev->conf, &dev->bme) != BME68X_OK) {
        i2c_master_bus_rm_device(dev->i2c_dev);
        free(dev);
        return ESP_FAIL;
    }

    dev->heatr_conf.enable = BME68X_ENABLE;
    dev->heatr_conf.heatr_temp = 300;
    dev->heatr_conf.heatr_dur = 100;
    if (bme68x_set_heatr_conf(BME68X_FORCED_MODE, &dev->heatr_conf, &dev->bme) != BME68X_OK) {
        i2c_master_bus_rm_device(dev->i2c_dev);
        free(dev);
        return ESP_FAIL;
    }

    *out_handle = dev;
    return ESP_OK;
}

esp_err_t bme688_configure(bme688_handle_t dev, bme688_oversampling_t os_temp, bme688_oversampling_t os_pres,
                           bme688_oversampling_t os_hum, bme688_filter_t filter)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->conf.os_temp = os_temp;
    dev->conf.os_pres = os_pres;
    dev->conf.os_hum = os_hum;
    dev->conf.filter = filter;
    dev->conf.odr = BME68X_ODR_NONE;

    return (bme68x_set_conf(&dev->conf, &dev->bme) == BME68X_OK) ? ESP_OK : ESP_FAIL;
}

esp_err_t bme688_set_heater_profile(bme688_handle_t dev, uint16_t target_temp_c, uint16_t duration_ms)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->heatr_conf.enable = BME68X_ENABLE;
    dev->heatr_conf.heatr_temp = target_temp_c;
    dev->heatr_conf.heatr_dur = duration_ms;

    return (bme68x_set_heatr_conf(BME68X_FORCED_MODE, &dev->heatr_conf, &dev->bme) == BME68X_OK) ? ESP_OK : ESP_FAIL;
}

esp_err_t bme688_set_ambient_temperature(bme688_handle_t dev, int8_t ambient_temp_c)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->bme.amb_temp = ambient_temp_c;
    return ESP_OK;
}

esp_err_t bme688_read(bme688_handle_t dev, bme688_data_t *out_data)
{
    if (!dev || !out_data) {
        return ESP_ERR_INVALID_ARG;
    }

    if (bme68x_set_op_mode(BME68X_FORCED_MODE, &dev->bme) != BME68X_OK) {
        return ESP_FAIL;
    }

    uint32_t delay_us =
        bme68x_get_meas_dur(BME68X_FORCED_MODE, &dev->conf, &dev->bme) + ((uint32_t)dev->heatr_conf.heatr_dur * 1000);
    dev->bme.delay_us(delay_us, dev->bme.intf_ptr);

    struct bme68x_data data;
    uint8_t n_fields = 0;
    int8_t rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &dev->bme);
    if (rslt != BME68X_OK && rslt != BME68X_W_NO_NEW_DATA) {
        return ESP_FAIL;
    }
    if (n_fields == 0) {
        return ESP_ERR_TIMEOUT;
    }

    out_data->temperature = data.temperature;
    out_data->pressure = data.pressure;
    out_data->humidity = data.humidity;
    out_data->gas_resistance = data.gas_resistance;
    out_data->gas_valid = (data.status & BME68X_GASM_VALID_MSK) != 0;
    out_data->heater_stable = (data.status & BME68X_HEAT_STAB_MSK) != 0;

    return ESP_OK;
}

esp_err_t bme688_delete(bme688_handle_t dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = i2c_master_bus_rm_device(dev->i2c_dev);
    free(dev);
    return err;
}
