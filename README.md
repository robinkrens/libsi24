# Library for SI24R1
SI24R1 is a very low power wireless chip in the 2.4 GHz ISM radio band.
This is a clone of the original NRF24l01 chip. I do not have the
original chip, but as far as I can tell this are pin and silicon compatible.

## How to use
You will need to glue ``int (*write_and_read)(unsigned char *data, size_t sz)``
and ``void (*chip_enable)(unsigned val)`` functions to your specific SPI 
hardware. See examples folders on how to initialze and configure structs.

## Currently supported functionality
- Receiver mode 
- Transfer mode
- Dynamic payloads

## Not supported yet 
- Multiple channels

## References
- https://datasheet.lcsc.com/szlcsc/1811142211_Nanjing-Zhongke-Microelectronics-Si24R1_C14436.pdf
