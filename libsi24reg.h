/**
 * File              : libsi24reg.h
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 18.01.2023
 * Last Modified By  : Robin Krens <robin@robinkrens.nl>
 */


#define SI24_READ_REG 0x00
#define SI24_WRITE_REG 0x20
#define SI24_R_RX_PAYLOAD 0x61
#define SI24_W_TX_PAYLOAD 0xA0
#define SI24_FLUSH_TX 0xE1

typedef struct {
    union {
        unsigned char byte;
        struct {
            unsigned PRIM_XR : 1;
            unsigned PWR_UP : 1;
            unsigned CRCO : 1;
            unsigned EN_CRC : 1;
            unsigned MASK_MAX_RT : 1;
            unsigned MASK_TX_DS : 1;
            unsigned MAKS_RX_DS : 1;
            unsigned _RESERVED : 1;
        } bits;
    };
} config_reg_t;


