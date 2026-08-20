# Soldered NAZIV PROIZVODA Component

| ![Product name](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :------------------------------------------------------------------------------------: |
|                      [NAZIV PROIZVODA](https://www.solde.red/SKU)                      |

OPIS PROIZVODA + LINK NA [Qwiic ecosystem](https://soldered.com/collections/qwiic-ecosystem).

### Using the template

Before publishing a new component make sure to replace:

- `NAZIV PROIZVODA`, `OPIS PROIZVODA`, product image, SKU link, and the "Original source" line in this README
- `version`, `description`, `url` in `idf_component.yml`
- `components:` name and `namespace:` in `.github/workflows/upload_component.yml`
- filenames in `src/` and `include/` plus matching `SRCS` and `INCLUDE_DIRS` in `CMakeLists.txt` and `#include` in the `.c` file
- dependency key in `examples/.../idf_component.yml` (path stays `../../..`)
- `@file`, `@brief`, `@param`, `@return` Doxygen comments in `include/*.h`, `src/*.c`, and `examples/basic/main/main.c` to describe the real API

Also make sure to add examples.

Run `./format.sh` before committing to auto-format `src/`, `include/`, and the example against the project's astyle rules (`.astyle-rules.yml`). CI runs the same check on every push/PR via `.github/workflows/format-check.yml` and fails on unformatted code.

**Remove this section of README after everything is done!**

For uploading to Registry you need to register a trusted publisher under a component. To make the release to the registry you must bump `version` in `idf_component.yml` to `X.Y.Z`, push that commit, and confirm Format Check + Build Examples both pass on it (Actions tab) before tagging. Only once both are green: `git tag vX.Y.Z && git push origin vX.Y.Z`.

### Repository Contents

- **/src** - source files (.c)
- **/include** - header files (.h)
- **/examples** - examples for using the library
- **_other_** - idf_component.yml manifest file for ESP Component Registry

### Hardware design

You can find hardware design for this board in _NAZIV PROIZVODA_ hardware repository.

### Documentation

Access library documentation [here](https://docs.soldered.com/).

### About Soldered

<img src="https://raw.githubusercontent.com/SolderedElectronics/Soldered-Generic-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Documentation](https://docs.soldered.com)

### Original source

This library is possible thanks to original [arduino-mcp23017](https://github.com/blemasle/arduino-mcp23017) library. Thank you, blemasle.

### Open-source license

Soldered invests vast amounts of time into hardware & software for these products, which are all open-source. Please support future development by buying one of our products.

Check license details in the LICENSE file. Long story short, use these open-source files for any purpose you want to, as long as you apply the same open-source licence to it and disclose the original source. No warranty - all designs in this repository are distributed in the hope that they will be useful, but without any warranty. They are provided "AS IS", therefore without warranty of any kind, either expressed or implied. The entire quality and performance of what you do with the contents of this repository are your responsibility. In no event, Soldered (TAVU) will be liable for your damages, losses, including any general, special, incidental or consequential damage arising out of the use or inability to use the contents of this repository.

## Have fun!

And thank you from your fellow makers at Soldered Electronics.
