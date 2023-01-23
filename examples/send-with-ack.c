#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsi24.h"

int example_spi_write_read(unsigned char* data, size_t sz)
{
	(void)data;
	(void)sz;
	return 0;
}

void example_ce(unsigned val)
{
	(void)val;
}

void example_event_handler(si24_t* si, si24_event_t* e)
{
	(void)si;

	switch (e->type) {
	case EV_RX_COMPLETE:
		break;
	case EV_TX_COMPLETE:
		break;
	case EV_TX_FULL:
		break;
	case EV_RX_EMPTY:
		break;
	case EV_ERR_TIMEOUT:
		break;
	case EV_ERR_BUS:
		break;
	case EV_ERR_MAX_RETRIES:
		break;
	case EV_ERR_CRC:
		break;
	case EV_ERR_CONFIG:
		break;
	default:
		break;
	}
}

int main(void)
{
	const unsigned char buf[] = "SAMPLE TEST MESSAGE!";

	si24_ioctl_t ctl = {
		.write_and_read = example_spi_write_read,
		.chip_enable = example_ce,
	};

	const si24_opts_t opts = {
		.mode = SEND_MODE,
		.enable_ack = 1,
		.non_blocking = 0,
		.enable_crc = 1,
		.enable_dynpd = 1,
		.crc = TWO_BYTE,
		.ioctl = &ctl,
		.speed = MBPS2,
		.txpwr = PLUS4DB,
		.payload = 5,
		.timeout = 5,
		.retries = 5,
		.mac_addr = { 0xAB, 0xCD, 0xEF, 0xFF, 0xFF }
	};

	struct si24_t* si = si24_init(&opts, example_event_handler);

	int bytes_sent = si24_send(si, buf, sizeof(buf));
	while (bytes_sent != sizeof(buf)) {
		if (bytes_sent == -1)
			break;
		bytes_sent += si24_send(si, buf + bytes_sent, sizeof(buf) - bytes_sent);
	}
}
