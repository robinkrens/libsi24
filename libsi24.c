/**
 * File              : libsi24.c
 * Author            : Robin Krens <robin@robinkrens.nl>
 * Date              : 18.01.2023
 * Last Modified Date: 18.01.2023
 * Last Modified By  : Robin Krens <robin@robinkrens.nl>
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsi24.h"
#include "libsi24reg.h"

struct si24_t {
	const si24_opts_t *opts;
	const si24_ioctl_t *ctl;
	si24_event_handler_t eh;
};

si24_t* si24_init(const si24_opts_t *opts, si24_event_handler_t eh)
{
	struct si24_t *si = (si24_t*) calloc(1, sizeof(si24_t));
	if (si == 0)
		return 0;

	si->opts = opts;
	si->ctl = opts->ioctl;
	si->eh = eh;

	return si;
}

int main(void)
{
}
