/**
 * File              : libsi24.h
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 22.01.2023
 * Last Modified By  : Robin Krens <robin@robinkrens.nl>
 */

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

enum si24_status_t {
    SHUTDOWN = 0,
    STANDBY,
    IDLETX,
    TX,
    RX
};

typedef enum si24_status_t si24_status_t;

enum si24_mode_t {
    RECV_MODE = 0,
    SEND_MODE
};

typedef enum si24_mode_t si24_mode_t;

enum si24_crc_t {
    ONE_BYTE = 1,
    TWO_BYTE = 2
};

typedef enum si24_crc_t si24_crc_t;

enum si24_speed_t {
	MBPS1 = 0,
	MBPS2 = 1,
	KBPS250 = 2
};

typedef enum si24_speed_t si24_speed_t;

enum si24_txpower_t {
	MINUS12DB = 0,
	MINUS6DB,
	MINUS4DB,
	ZERODB,
	PLUS1DB,
	PLUS3DB,
	PLUS4DB,
	PLUS7DB
};

typedef enum si24_txpower_t si24_txpower_t;

enum si24_event_type_t {
    EV_RX_COMPLETE = 0,
    EV_TX_COMPLETE,
    EV_TX_FULL,
    EV_ERR_TIMEOUT,
    EV_ERR_BUS,
    EV_ERR_MAX_RETRIES,
    EV_ERR_CRC,
    EV_ERR_CONFIG
};

typedef enum si24_event_type_t si24_event_type_t;

union si24_event_t {
	enum si24_event_type_t type;
	struct error_t {
		enum si24_event_type_t _type;
		const char *file;
		const char *func;
		const char *msg;
		int line;
	} error;
};

typedef union si24_event_t si24_event_t;

/* low level IO control */
typedef struct {
    int (*write_and_read)(unsigned char *data, size_t sz);
    int (*chip_enable)(unsigned val);
} si24_ioctl_t;

typedef struct {
    si24_mode_t mode;
    unsigned enable_ack;
    unsigned non_blocking;
    unsigned enable_crc;
    unsigned enable_dynpd;
    si24_crc_t crc;
    si24_ioctl_t *ioctl;
    si24_speed_t speed;
    si24_txpower_t txpwr;
    unsigned payload;
    unsigned timeout; /* 1: 250 us, 15: 4000 us */
    unsigned retries; /* 1 to 15 */
    unsigned mac_addr;
} si24_opts_t;

/* private data structure */
typedef struct si24_t si24_t;

typedef void (*si24_event_handler_t)(si24_t* si24, si24_event_t* event);

extern si24_t* si24_init(const si24_opts_t* si24opts, si24_event_handler_t eh);
extern void si24_free(si24_t* si24);
extern size_t si24_send(si24_t* si24, const unsigned char * buf, size_t size);
extern size_t si24_recv(si24_t* si24, unsigned char * buf, size_t size);
extern void si24_reset(si24_t* si24);

#if defined(__cplusplus)
}
#endif
