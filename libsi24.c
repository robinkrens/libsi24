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

#define DEBUG 0
#define TIMEOUT 0xFFFF

struct si24_t {
	const si24_opts_t *opts;
	const si24_ioctl_t *ctl;
	si24_event_handler_t eh;
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
	
	if(DEBUG) {
		printf("REG 0x%x:\t", reg);
		for (int i = 1; i <= sz; ++i) {
			fprintf(stdout, "0x%x(%c)", buf[i], buf[i]);
		}
		fprintf(stdout, "\n");
	}

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
	uint8_t feature_reg = 0x0; /* default value */
	uint8_t rf_setup_reg = 0xE; /* default value */
	uint8_t setup_retr_reg = 0x3; /* default value */
	const uint8_t rf_ch_reg = 0x40; /* default value */
	const si24_opts_t * params = si->opts; 
	
	if (params->enable_crc) {
		config_reg |= (1 << EN_CRC);
		config_reg |= (si->opts->crc << CRCO);
	}

	if (params->enable_ack) {
		if (params->mode == SEND_MODE) {
			setup_retr_reg = ARD(params->timeout) | ARC(params->retries);
			ret += _reg_write(si, SI24_REG_SETUP_RETR, &setup_retr_reg, 1);
		}
	} else {
		if (params->mode == SEND_MODE) {
			feature_reg |= (1 << EN_DYN_ACK);
			ret += _reg_write(si, SI24_REG_FEATURE, &feature_reg, 1);
		}
	}

	if (params->enable_dynpd) {
		uint8_t dyn = (1 << DPL_P0);
		ret += _reg_write(si, SI24_REG_DYNPD, &dyn, 1);
		feature_reg |= (1 << EN_DPL);
		ret += _reg_write(si, SI24_REG_FEATURE, &feature_reg, 1);
	} else { /* fixed payload size */
		if (params->mode == RECV_MODE) {
			ret += _reg_write(si, SI24_REG_RX_PW_P0, (uint8_t *) &params->payload, 1);
		}
	}

	uint8_t aw = AW_5;
	ret += _reg_write(si, SI24_REG_SETUP_AW, &aw, 1); 

	if (params->mode == SEND_MODE && params->enable_ack) {
		ret += _reg_write(si, SI24_REG_RX_ADDR_P0, params->mac_addr, sizeof(params->mac_addr));
	}
	
	if (params->mode == RECV_MODE) {
		config_reg |= (1 << PRIM_RX);
		uint8_t ch = 0x1;
		ret += _reg_write(si, SI24_REG_EN_RXADDR, &ch, 1); 
		ret += _reg_write(si, SI24_REG_RX_ADDR_P0, params->mac_addr, sizeof(params->mac_addr));
	} else {
		ret += _reg_write(si, SI24_REG_TX_ADDR, params->mac_addr, sizeof(params->mac_addr));
	}

	rf_setup_reg |= (params->speed << RF_DR_HIGH);
	rf_setup_reg |= (params->txpwr << RF_PWR);
	ret += _reg_write(si, SI24_REG_RF_SETUP, &rf_setup_reg, 1);


	ret += _reg_write(si, SI24_REG_RF_CH, &rf_ch_reg, 1);
	ret += _reg_write(si, SI24_REG_CONFIG, &config_reg, 1);

	if (params->mode == RECV_MODE) {
		/* start accepting data immediately,
		 * for send mode it is onyl activated upon sending */
		params->ioctl->chip_enable(1);
	}

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
	size_t bytes_sent = 0;
	uint16_t timeout = 0;
	int sz;
	uint8_t flags;
	
	if (si->opts->mode == RECV_MODE)
		return -1;

	_reg_read(si, SI24_REG_STATUS, (uint8_t *) &flags, 1);

	if (flags & (1 << TX_FULL)) {
		ev.type = EV_TX_FULL;
		si->eh(si, &ev);
		return -1;
	}

	int payload = si->opts->payload;

	if (si->opts->enable_dynpd)
		payload = size > 32 ? 32 : size;

	for (size_t idx = 0; idx < size; idx += payload) {
		sz = (size - idx) < payload ? (size - idx) : payload;  
		if (si->opts->enable_ack) {
			_reg_write(si, SI24_W_TX_PAYLOAD, buf + idx, sz);
			si->ctl->chip_enable(1);
			while ((!(flags & (1 << TX_DS)) && !(flags & (1 << MAX_RT))) && timeout < TIMEOUT) {
				_reg_read(si, SI24_REG_STATUS, &flags, 1);
				timeout++;
			}
			if (flags & (1 << MAX_RT)) {
				ev.type = EV_ERR_MAX_RETRIES;
				si->eh(si, &ev);
				si24_reset(si);
				return bytes_sent;
			}

		} else {
			_reg_write(si, SI24_W_TX_PAYLOAD_NO_ACK, buf + idx, sz);
			si->ctl->chip_enable(1);
			while (!(flags & (1 << TX_DS)) && timeout < TIMEOUT) {
				_reg_read(si, SI24_REG_STATUS, &flags, 1);
				timeout++;
			}
		}

		if (timeout >= TIMEOUT) {
			ev.type = EV_ERR_TIMEOUT;
			si->eh(si, &ev);
			si24_reset(si);
			return bytes_sent;
		}

		flags |= (1 << TX_DS);
		_reg_write(si, SI24_REG_STATUS, &flags, 1);
		_reg_read(si, SI24_REG_STATUS, &flags, 1);
		bytes_sent += sz;
		timeout = 0;
	}

	ev.type = EV_TX_COMPLETE;
	si->eh(si, &ev);
	si->ctl->chip_enable(0);

	return bytes_sent;
}

size_t si24_recv(si24_t* si, unsigned char * buf, size_t size) 
{
	si24_event_t ev;
	size_t bytes_read = 0;
	uint8_t p_size = si->opts->payload;
	uint8_t tmpbuf[p_size];
	uint8_t flags;
	uint8_t fifo_flags;
	
	if (si->opts->mode == SEND_MODE)
		return -1;

	_reg_read(si, SI24_REG_STATUS, &flags, 1);

	if (!(flags & (1 << RX_DR))) {
		ev.type = EV_RX_EMPTY;
		si->eh(si, &ev);
		return bytes_read;
	}

	/* do not accept any new incoming data */
	si->opts->ioctl->chip_enable(0);

	_reg_read(si, SI24_REG_FIFO_SATUS, &fifo_flags, 1);
	while(!(fifo_flags & (1 << RX_EMPTY)) &&
			bytes_read < size) {
		
		if (si->opts->enable_dynpd) {
			uint8_t d_sz = 0;
			_reg_read(si, SI24_RX_PL_WID, &d_sz, 1);
			p_size = d_sz;
		}
		int m_size = (size - bytes_read) > p_size ? p_size : (size - bytes_read);
		_reg_read(si, SI24_R_RX_PAYLOAD, tmpbuf, m_size);

		memcpy(buf + bytes_read, tmpbuf, m_size);
		bytes_read += m_size;

		_reg_read(si, SI24_REG_FIFO_SATUS, &fifo_flags, 1);
	}

	/* only clear data ready flag when FIFO is empty */
	if (fifo_flags & (1 << RX_EMPTY)) {
		flags |= (1 << RX_DR);
		_reg_write(si, SI24_REG_STATUS, &flags, 1);
	}
	
	ev.type = EV_RX_COMPLETE;
	si->eh(si, &ev);

	si->opts->ioctl->chip_enable(1);
	
	return bytes_read;
}

void si24_reset(si24_t* si)
{
	if (si->opts->mode == RECV_MODE) {
		_reg_write(si, SI24_FLUSH_RX, 0, 0);
	}
	else if (si->opts->mode == SEND_MODE) {
		_reg_write(si, SI24_FLUSH_TX, 0, 0);
	}

	uint8_t status_reg = {0};
	status_reg |= (1 << RX_DR);
	status_reg |= (1 << TX_DS);
	status_reg |= (1 << MAX_RT);

	_reg_write(si, SI24_REG_STATUS, (uint8_t *) &status_reg, 1);

	si->ctl->chip_enable(0);
}

void si24_free(si24_t * si)
{
	free(si);
}
