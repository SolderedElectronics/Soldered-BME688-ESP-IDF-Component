/**
 * @file soldered-bme688.h
 * @brief Public API for the soldered-bme688 component
 * @author Soldered Electronics
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/** I2C address when SDO is pulled low */
#define BME688_I2C_ADDR_LOW  0x76
/** I2C address when SDO is pulled high */
#define BME688_I2C_ADDR_HIGH 0x77

/** Opaque driver handle returned by bme688_init() */
typedef struct bme688_dev *bme688_handle_t;

/** Oversampling settings for temperature, pressure and humidity. Values match the sensor's register encoding. */
typedef enum {
    BME688_OS_NONE = 0, /*!< Skip this measurement */
    BME688_OS_1X = 1,
    BME688_OS_2X = 2,
    BME688_OS_4X = 3,
    BME688_OS_8X = 4,
    BME688_OS_16X = 5,
} bme688_oversampling_t;

/** IIR filter coefficient applied to temperature and pressure. Values match the sensor's register encoding. */
typedef enum {
    BME688_FILTER_OFF = 0,
    BME688_FILTER_2 = 1,
    BME688_FILTER_4 = 2,
    BME688_FILTER_8 = 3,
    BME688_FILTER_16 = 4,
    BME688_FILTER_32 = 5,
    BME688_FILTER_64 = 6,
    BME688_FILTER_128 = 7,
} bme688_filter_t;

/** One compensated measurement */
typedef struct {
    float temperature;    /*!< Degrees Celsius */
    float pressure;       /*!< Pascal */
    float humidity;       /*!< Percent relative humidity, 0-100 */
    float gas_resistance; /*!< Gas sensor resistance in Ohm, only meaningful if gas_valid && heater_stable */
    bool gas_valid;        /*!< False if the gas measurement was disturbed (e.g. by a heater profile change) */
    bool heater_stable;    /*!< False if the heater plate did not reach the target temperature in time */
} bme688_data_t;

/**
 * @brief Probe the sensor on an existing I2C bus, reset it and read its factory calibration data.
 *
 * @param[in]  bus_handle     I2C master bus the sensor is wired to (already created by the caller)
 * @param[in]  i2c_addr       BME688_I2C_ADDR_LOW or BME688_I2C_ADDR_HIGH
 * @param[in]  scl_speed_hz   I2C bus clock for this device, e.g. 100000 or 400000
 * @param[out] out_handle     Set to a newly allocated driver handle on success
 *
 * @return ESP_OK on success, ESP_ERR_NOT_FOUND if the chip ID doesn't match a BME688,
 *         ESP_ERR_INVALID_RESPONSE if the variant ID doesn't match, or an I2C driver error.
 */
esp_err_t bme688_init(i2c_master_bus_handle_t bus_handle, uint8_t i2c_addr, uint32_t scl_speed_hz,
                      bme688_handle_t *out_handle);

/**
 * @brief Configure oversampling and IIR filtering for temperature, pressure and humidity.
 *
 * Safe to call again later to change settings; takes effect on the next bme688_read().
 *
 * @param[in] dev      Handle from bme688_init()
 * @param[in] os_temp  Temperature oversampling
 * @param[in] os_pres  Pressure oversampling
 * @param[in] os_hum   Humidity oversampling
 * @param[in] filter   IIR filter coefficient for temperature/pressure
 */
esp_err_t bme688_configure(bme688_handle_t dev, bme688_oversampling_t os_temp, bme688_oversampling_t os_pres,
                           bme688_oversampling_t os_hum, bme688_filter_t filter);

/**
 * @brief Configure the gas sensor heater plate for the next bme688_read() call.
 *
 * @param[in] dev             Handle from bme688_init()
 * @param[in] target_temp_c   Heater plate target temperature, 200-400 degC is the sensor's typical range
 * @param[in] duration_ms     How long to hold the heater at temperature before sampling gas resistance
 */
esp_err_t bme688_set_heater_profile(bme688_handle_t dev, uint16_t target_temp_c, uint16_t duration_ms);

/**
 * @brief Tell the driver the current ambient temperature, used only to correct the heater's target
 * resistance calculation. Defaults to 25 degC if never called; a wrong value costs a few degrees of
 * heater accuracy, it does not break the driver. Call before bme688_set_heater_profile(), since the
 * heater resistance is computed and written to the sensor at that call, not at read time.
 */
esp_err_t bme688_set_ambient_temperature(bme688_handle_t dev, int8_t ambient_temp_c);

/**
 * @brief Trigger one forced-mode measurement (temperature, pressure, humidity and gas), block until the
 * sensor has the result ready, and return the compensated values.
 *
 * @param[in]  dev       Handle from bme688_init()
 * @param[out] out_data  Filled in on success
 *
 * @return ESP_OK on success, ESP_ERR_TIMEOUT if the sensor never reported new data in time.
 */
esp_err_t bme688_read(bme688_handle_t dev, bme688_data_t *out_data);

/**
 * @brief Free a handle created by bme688_init() and remove the device from its I2C bus.
 */
esp_err_t bme688_delete(bme688_handle_t dev);

#ifdef __cplusplus
}
#endif
