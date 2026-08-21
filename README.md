# Soldered BME688 Component

| ![BME688 breakout](https://cms.soldered.com/products/333203/media/333203_featured-photo_ce08e8.jpg) |
| :---------------------------------------------------------------------------------------------------: |
|                                [BME688 breakout](https://www.solde.red/333203)                                |

ESP-IDF driver for the Bosch BME688 environmental sensor (temperature, pressure, humidity and gas/VOC
resistance) over I2C, for use with the [Qwiic ecosystem](https://soldered.com/collections/qwiic-ecosystem).

### Repository Contents

- **/src** - source files (.c), including the vendored Bosch `bme68x` Sensor API under `src/bosch-bme68x/`
- **/include** - public header (`soldered_bme688.h`)
- **/examples** - examples for using the library
- **_other_** - idf_component.yml manifest file for ESP Component Registry

### Usage

This driver is a thin ESP-IDF wrapper (I2C glue over the new `driver/i2c_master.h` API) around Bosch's own
open-source `bme68x` Sensor API, which is vendored unmodified under `src/bosch-bme68x/`. See
`include/soldered_bme688.h` for the public API and `examples/basic` for a full working example: create an
`i2c_master_bus_handle_t`, call `bme688_init()`, optionally `bme688_configure()` /
`bme688_set_heater_profile()`, then call `bme688_read()` in a loop.

### Hardware design

You can find hardware design for this board in the _BME688_ hardware repository.

### Documentation

Access library documentation [here](https://docs.soldered.com/).

### About Soldered

<img src="https://raw.githubusercontent.com/SolderedElectronics/Soldered-Generic-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Documentation](https://docs.soldered.com)

### Original source

The register access, compensation formulas and gas-heater sequencing in `src/bosch-bme68x/` are Bosch
Sensortec's own open-source [BME68x Sensor API](https://github.com/boschsensortec/BME68x-Sensor-API)
(BSD-3-Clause), vendored here unmodified. Thank you, Bosch Sensortec.

### Open-source license

Soldered invests vast amounts of time into hardware & software for these products, which are all open-source. Please support future development by buying one of our products.

Check license details in the LICENSE file. Long story short, use these open-source files for any purpose you want to, as long as you apply the same open-source licence to it and disclose the original source. No warranty - all designs in this repository are distributed in the hope that they will be useful, but without any warranty. They are provided "AS IS", therefore without warranty of any kind, either expressed or implied. The entire quality and performance of what you do with the contents of this repository are your responsibility. In no event, Soldered (TAVU) will be liable for your damages, losses, including any general, special, incidental or consequential damage arising out of the use or inability to use the contents of this repository.

## Have fun!

And thank you from your fellow makers at Soldered Electronics.
