/**
 * File              : libsi24reg.h
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 22.01.2023
 * Last Modified By  : Robin Krens <robin@robinkrens.nl>
 */

/* SI24R1 commands */
#define SI24_R_REGISTER 0x00
#define SI24_W_REGISTER 0x20
#define SI24_R_RX_PAYLOAD 0x61
#define SI24_W_TX_PAYLOAD 0xA0
#define SI24_FLUSH_TX 0xE1
#define SI24_FLUSH_RX 0xE2
#define SI24_REUSE_TX_PL 0xE3
#define SI24_RX_PL_WID 0x60
#define SI24_W_ACK_PAYLOAD 0xA8
#define SI24_W_TX_PAYLOAD_NO_ACK 0xB0
#define SI24_NOP 0xFF

/* SI24R1 register addresses */
#define SI24_REG_CONFIG 0x00
#define SI24_REG_EN_AA 0x01
#define SI24_REG_EN_RXADDR 0x02
#define SI24_REG_SETUP_AW 0x03
#define SI24_REG_SETUP_RETR 0x04
#define SI24_REG_RF_CH 0x05
#define SI24_REG_RF_SETUP 0x06
#define SI24_REG_STATUS 0x07
#define SI24_REG_OBSERVE_TX 0x08
#define SI24_REG_RSSI 0x09
#define SI24_REG_RX_ADDR_P0 0x0A
#define SI24_REG_RX_ADDR_P1 0x0B 
#define SI24_REG_RX_ADDR_P2 0x0C
#define SI24_REG_RX_ADDR_P3 0x0D
#define SI24_REG_RX_ADDR_P4 0x0E
#define SI24_REG_RX_ADDR_P5 0x0F
#define SI24_REG_TX_ADDR 0x10
#define SI24_REG_RX_PW_P0 0x11
#define SI24_REG_RX_PW_P1 0x12
#define SI24_REG_RX_PW_P2 0x13
#define SI24_REG_RX_PW_P3 0x14
#define SI24_REG_RX_PW_P4 0x15
#define SI24_REG_RX_PW_P5 0x16
#define SI24_REG_FIFO_SATUS 0x17
#define SI24_REG_DYNPD 0x1C
#define SI24_REG_FEATURE 0x1D

/* config register */
#define PRIM_RX 0x0
#define PWR_UP 0x1
#define CRCO 0x2
#define EN_CRC 0x3
#define MASK_MAX_RT 0x4
#define MASK_TX_DS 0x5
#define MASK_RX_DR 0x6

#define ENAA_P0 0x0
#define ENAA_P1 0x1
#define ENAA_P2 0x2
#define ENAA_P3 0x3
#define ENAA_P4 0x4
#define ENAA_P5 0x5

#define ERX_P0 0x0
#define ERX_P1 0x1
#define ERX_P2 0x2
#define ERX_P3 0x3
#define ERX_P4 0x4
#define ERX_P5 0x5

#define AW_3 0x1
#define AW_4 0x2
#define AW_5 0x3

#define ARD(x) (x << 4)
#define ARC(x) (x & 0xF)

#define RF_CH 0x0

#define RF_PWR 0x0
#define RF_DR_HIGH 0x03
#define PLL_LOCK 0x04
#define RF_DR_LOW 0x05
#define CONT_WAVE 0x07

#define PLOS_CNT(x) (x & 0xF0)
#define ARC_CNT(x) (x & 0x0F)

#define RSSI 0x0

#define RX_EMPTY 0x0
#define RX_FULL 0x1
#define TX_EMPTY 0x4
#define TX_REUSE 0x6

#define DPL_P0 0x0
#define DPL_P1 0x1
#define DPL_P2 0x2
#define DPL_P3 0x3
#define DPL_P4 0x4
#define DPL_P5 0x5

#define EN_DYN_ACK 0x0
#define EN_ACK_PAY 0x1
#define EN_DPL 0x2
			
#define TX_FULL 0x0
#define RX_P_NO 0x1
#define MAX_RT 0x4
#define TX_DS 0x5
#define RX_DR 0x6
