/**
 * File              : libsi24.h
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 18.01.2023
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

enum si24_type_t {
    RECV = 0,
    SEND
};

typedef enum si24_type_t si24_type_t;

enum si24_crc_t {
    ONE_BYTE = 1,
    TWO_BYTE = 2
};

typedef enum si24_crc_t si24_crc_t;

enum si24_event_t {
    TEST = 0
};

typedef enum si24_event_t si24_event_t;

/* low level IO control */
typedef struct {
    size_t (*read)(unsigned char* buf, size_t size);
    size_t (*write)(const unsigned char* buf, size_t size);
} si24_ioctl_t;

typedef struct {
    si24_type_t type;
    unsigned enable_ack;
    si24_crc_t crc;
    si24_ioctl_t *ioctl;
    unsigned msg_len;
} si24_opts_t;

/* private data structure */
typedef struct si24_t si24_t;

typedef void (*si24_event_handler_t)(si24_t* si24, si24_event_t* event);

extern si24_t* si24_init(const si24_opts_t* si24opts, si24_event_handler_t eh);
extern void si24_free(si24_t* si24);
extern void si24_send(si24_t* si24, const char* buf, size_t size);
extern void si24_recv(si24_t* si24, char* buf, size_t size);
extern void si24_reset(si24_t* si24);

#if defined(__cplusplus)
}
#endif
