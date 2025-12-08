/*
 *	wiiuse
 *
 *	Written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	Copyright 2006-2007
 *
 *	This file is part of wiiuse.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	$Header: /cvsroot/devkitpro/libogc/wiiuse/wiiboard.c,v 1.6 2008/05/26 19:24:53 shagkur Exp $
 *
 */

/**
 *	@file
 *	@brief Wiiboard expansion device.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef WIN32
	#include <Winsock2.h>
#endif

#include "definitions.h"
#include "wiiuse_internal.h"
#include "dynamics.h"
#include "events.h"
#include "wiiboard.h"
#include "io.h"

/**
 *	@brief Handle the handshake data from the wiiboard.
 *
 *	@param wb		A pointer to a wii_board_t structure.
 *	@param data		The data read in from the device.
 *	@param len		The length of the data block, in bytes.
 *
 *	@return	Returns 1 if handshake was successful, 0 if not.
 */
int wii_board_handshake(struct wiimote_t* wm, struct wii_board_t* wb, ubyte* data, uword len) 
{
	int offset = 0;

	if (data[offset]==0xff) {
		if (data[offset+0x10]==0xff) {
			WIIUSE_DEBUG("Wii Balance Board handshake appears invalid, trying again.");
			wiiuse_read_data(wm, data, WM_EXP_MEM_CALIBR, EXP_HANDSHAKE_LEN, wiiuse_handshake_expansion);
			return 0;
		}
		offset += 0x10;
	}

	// The refrence values for 0 Kg
	wb->ctr[0] = (data[offset+0x04]<<8) | data[offset+0x05];
	wb->cbr[0] = (data[offset+0x06]<<8) | data[offset+0x07];
	wb->ctl[0] = (data[offset+0x08]<<8) | data[offset+0x09];
	wb->cbl[0] = (data[offset+0x0a]<<8) | data[offset+0x0b];

	// The reference values for 17 Kg
	wb->ctr[1] = (data[offset+0x0c]<<8) | data[offset+0x0d];
	wb->cbr[1] = (data[offset+0x0e]<<8) | data[offset+0x0f];
	wb->ctl[1] = (data[offset+0x10]<<8) | data[offset+0x11];
	wb->cbl[1] = (data[offset+0x12]<<8) | data[offset+0x13];

	// The reference values for 34 Kg
	wb->ctr[2] = (data[offset+0x14]<<8) | data[offset+0x15];
	wb->cbr[2] = (data[offset+0x16]<<8) | data[offset+0x17];
	wb->ctl[2] = (data[offset+0x18]<<8) | data[offset+0x19];
	wb->cbl[2] = (data[offset+0x1a]<<8) | data[offset+0x1b];

	// The minimum battery value (always 0x6a).
	wb->cbat = data[offset+0x01];

	// The reference temperature.
        wb->ctemp = data[offset+0x40];

	/* handshake done */
	wm->event = WIIUSE_WII_BOARD_INSERTED;
	wm->exp.type = EXP_WII_BOARD;

	return 1; 
}


/**
 *	@brief The wii board disconnected.
 *
 *	@param cc		A pointer to a wii_board_t structure.
 */
void wii_board_disconnected(struct wii_board_t* wb)
{
	memset(wb, 0, sizeof(struct wii_board_t));
}


/**
 *	@brief Handle wii board event.
 *
 *	@param wb		A pointer to a wii_board_t structure.
 *	@param msg		The message specified in the event packet.
 */
void wii_board_event(struct wii_board_t* wb, ubyte* msg)
{
    WIIUSE_DEBUG("wii_board_event");
	wb->rtr = (msg[0]<<8)|msg[1];
	wb->rbr = (msg[2]<<8)|msg[3];
	wb->rtl = (msg[4]<<8)|msg[5];
	wb->rbl = (msg[6]<<8)|msg[7];
	wb->rtemp = msg[0x8];
	wb->rbat = msg[0xa];
}
