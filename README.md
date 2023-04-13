# Library for SI24R1
SI24R1 is a very low power wireless chip in the 2.4 GHz ISM radio band.
This is a clone of the original NRF24l01 chip. I do not have the
original chip, but as far as I can tell this are pin and silicon compatible.

## How to use
You will probably need to include the header and source file to your
project and glue ``int (*write_and_read)(unsigned char *data, size_t sz)``
and ``void (*chip_enable)(unsigned val)`` functions to your specific SPI 
HAL vendor calls. 

## Example projects
Here is a list of example projects that include this library:

- ESP32: https://github.com/robinkrens/esp32-si24r1-example

## Pinout picture
My chip does not have a silk screen. Luckily it is compatible with 
the nrf24l01. Here is a picture for reference:

![pinout](https://github.com/robinkrens/libsi24/raw/master/doc/si24r1-pinout.jpg)

## Currently supported functionality
- [X] Receiver mode 
- [X] Transfer mode
- [X] Dynamic payloads

## Not supported yet 
- [ ] Multiple channels

## References
- https://datasheet.lcsc.com/szlcsc/1811142211_Nanjing-Zhongke-Microelectronics-Si24R1_C14436.pdf
