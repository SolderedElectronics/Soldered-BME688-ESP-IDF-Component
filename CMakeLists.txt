idf_component_register(SRCS "src/soldered_bme688.c" "src/bosch-bme68x/bme68x.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "src/bosch-bme68x"
                    REQUIRES esp_driver_i2c)
