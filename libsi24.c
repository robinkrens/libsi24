/**
 * File              : libsi24.c
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 22.01.2023
 * Last Modified By  : Robin Krens <robin@robinkrens.nl>
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libsi24.h"
#include "libsi24reg.h"

struct si24_t {
	const si24_opts_t *opts;
	const si24_ioctl_t *ctl;
	si24_event_handler_t eh;
	si24_status_reg_t sr;
};

static uint8_t _reg_read(si24_t *si, uint8_t reg,
		uint8_t *data, int sz)
{
	uint8_t buf[sz+1];
	
	memset(buf, 0, sz+1); 
	buf[0] = reg | SI24_R_REGISTER;
	
	if (si->ctl->write_and_read(buf, sz+1) == -1) {
		si24_event_t ev;
		ev.type = EV_ERR_BUS;
		si->eh(si, &ev);
		return -1;
	}
	
	memcpy(data, (buf+1), sz);
	
	return 0;
}

static uint8_t _reg_write(si24_t *si, uint8_t reg,
		const uint8_t *data, int sz)
{
	uint8_t buf[sz+1];
	
	buf[0] = reg | SI24_W_REGISTER;
	memcpy((buf+1), data, sz);

	if (si->ctl->write_and_read(buf, sz+1) == -1) {
		si24_event_t ev;
		ev.type = EV_ERR_BUS;
		si->eh(si, &ev);
		return -1;
	}

	return 0;
}

static int _config(si24_t * si)
{
	int ret = 0;
	uint8_t config_reg = (1 << PWR_UP);
	uint8_t feature_reg = 0x2; /* default value */
	uint8_t rf_setup_reg = 0xE; /* default value */
	uint8_t setup_retr_reg = 0x3; /* default value */
	const uint8_t rf_ch_reg = 0x40; /* default value */
	const si24_opts_t * params = si->opts; 
	
	if (params->enable_crc) {
		config_reg |= (1 << EN_CRC);
		config_reg |= (si->opts->crc << CRCO);
	}

	if (params->enable_ack) {
		uint8_t dyn = (1 << DPL_P0);
		ret += _reg_write(si, SI24_REG_DYNPD, &dyn, 1);
		feature_reg |= (1 << EN_DYN_ACK);
		ret += _reg_write(si, SI24_REG_FEATURE, &feature_reg, 1);
	}

	uint8_t aw;
	if (params->mac_addr & 0xF0000) {
		aw = AW_5;
		ret += _reg_write(si, SI24_REG_SETUP_AW, &aw, 1); 
	} else if (params->mac_addr & 0xF000) {
		aw = AW_4;
		ret += _reg_write(si, SI24_REG_SETUP_AW, &aw, 1); 
	} else {
		aw = AW_3;
		ret += _reg_write(si, SI24_REG_SETUP_AW, &aw, 1); 
	}

	if (params->mode == SEND_MODE && params->enable_ack) {
		ret += _reg_write(si, SI24_REG_RX_ADDR_P0, (uint8_t *) &params->mac_addr, aw);
	}
	
	if (params->mode == RECV_MODE) {
		config_reg |= (1 << PRIM_RX);
		uint8_t ch = 0x1;
		ret += _reg_write(si, SI24_REG_EN_RXADDR, &ch, 1); 
		ret += _reg_write(si, SI24_REG_RX_ADDR_P0, (uint8_t *) &params->mac_addr, aw);
		ret += _reg_write(si, SI24_REG_RX_PW_P0, (uint8_t *) &params->payload, 1);
	} else {
		ret += _reg_write(si, SI24_REG_TX_ADDR, (uint8_t *) &params->mac_addr, aw);
	}

	rf_setup_reg |= (params->speed << RF_DR_HIGH);
	rf_setup_reg |= (params->txpwr << RF_PWR);
	ret += _reg_write(si, SI24_REG_RF_SETUP, &rf_setup_reg, 1);

	setup_retr_reg = ARD(params->timeout) | ARC(params->retries);
	ret += _reg_write(si, SI24_REG_SETUP_RETR, &setup_retr_reg, 1);

	ret += _reg_write(si, SI24_REG_RF_CH, &rf_ch_reg, 1);
	ret += _reg_write(si, SI24_REG_CONFIG, &config_reg, 1);

	return ret;
}

si24_t* si24_init(const si24_opts_t *opts, si24_event_handler_t eh)
{
	struct si24_t *si = (si24_t*) calloc(1, sizeof(si24_t));
	if (si == 0)
		return 0;

	si->opts = opts;
	si->ctl = opts->ioctl;
	si->eh = eh;

	int ret = _config(si);
	if (ret < 0) {
		free(si);
		return 0;
	}

	return si;
}

size_t si24_send(si24_t* si, const unsigned char * buf, size_t size)
{
	si24_event_t ev;
	uint16_t timeout = 0;
	int sz;
	si24_status_reg_t flags;

	if (si->opts->mode == RECV_MODE)
		return -1;

	_reg_read(si, 0x7, (uint8_t *) &flags, 1);

	if (flags.bits.TX_FULL) {
		ev.type = EV_TX_FULL;
		si->eh(si, &ev);
		return -1;
	}

	for (size_t idx = 0; idx < size; idx += si->opts->payload) {
		sz = (size - idx) < si->opts->payload ? (size - idx) : si->opts->payload;  
		if (si->opts->enable_ack) {
			_reg_write(si, SI24_W_TX_PAYLOAD, buf, sz);
			while ((!flags.bits.TX_DS || !flags.bits.MAX_RT) && timeout < 1000) {
				_reg_read(si, SI24_REG_STATUS, (uint8_t []){0x0}, 1);
				timeout++;
			}
			if (flags.bits.MAX_RT) {
				ev.type = EV_ERR_MAX_RETRIES;
				si->eh(si, &ev);
				si24_reset(si);
				return -1;
			}

		} else {
			_reg_write(si, SI24_W_TX_PAYLOAD_NO_ACK, buf, sz);
			si->ctl->chip_enable(1);
			while (!flags.bits.TX_DS && timeout < 1000) {
				_reg_read(si, SI24_REG_STATUS, (uint8_t []){0x0}, 1);
				timeout++;
			}
		}

		if (timeout >= 1000) {
			ev.type = EV_ERR_TIMEOUT;
			si->eh(si, &ev);
			si24_reset(si);
			return -1;
		}
		timeout = 0;
	}
	
	si->ctl->chip_enable(0);

	return 0;
}

size_t si24_recv(si24_t* si, unsigned char * buf, size_t size) 
{
	(void) buf;
	(void) size;
	if (si->opts->mode == SEND_MODE)
		return -1;
	
	si24_status_reg_t flags;
	_reg_read(si, 0x7, (uint8_t *) &flags, 1);
	
	if (!flags.bits.RX_DR)
		return -1;

	return 0;
}

void si24_reset(si24_t* si)
{
	si->ctl->chip_enable(0);
}

void si24_free(si24_t * si)
{
	free(si);
}

int main(void)
{
}
