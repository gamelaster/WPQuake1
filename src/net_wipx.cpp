/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// net_wipx.c

#include "quakedef.h"
#include "winquake.h"
#ifdef ID_PC
#include <wsipx.h>
#endif
#include "net_wipx.h"

extern cvar_t hostname;

#define MAXHOSTNAMELEN		256

static int net_acceptsocket = -1;		// socket for fielding new connections
static int net_controlsocket;
static struct qsockaddr broadcastaddr;

extern qboolean winsock_initialized;
#ifdef ID_PC
extern WSADATA		winsockdata;
#endif

#define IPXSOCKETS 18
static int ipxsocket[IPXSOCKETS];
static int sequence[IPXSOCKETS];

//=============================================================================

int WIPX_Init (void)
{
	return 0;
}

//=============================================================================

void WIPX_Shutdown (void)
{

}

//=============================================================================

void WIPX_Listen (qboolean state)
{

}

//=============================================================================

int WIPX_OpenSocket (int port)
{

	return -1;
}

//=============================================================================

int WIPX_CloseSocket (int handle)
{

	return 1;
}


//=============================================================================

int WIPX_Connect (int handle, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

int WIPX_CheckNewConnections (void)
{

	return -1;
}

//=============================================================================

static byte packetBuffer[NET_DATAGRAMSIZE + 4];

int WIPX_Read (int handle, byte *buf, int len, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

int WIPX_Broadcast (int handle, byte *buf, int len)
{
	return 0;
}

//=============================================================================

int WIPX_Write (int handle, byte *buf, int len, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

char *WIPX_AddrToString (struct qsockaddr *addr)
{
	static char buf[28];
	return buf;
}

//=============================================================================

int WIPX_StringToAddr (char *string, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

int WIPX_GetSocketAddr (int handle, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

int WIPX_GetNameFromAddr (struct qsockaddr *addr, char *name)
{
	return 0;
}

//=============================================================================

int WIPX_GetAddrFromName(char *name, struct qsockaddr *addr)
{
	
	return -1;
}

//=============================================================================

int WIPX_AddrCompare (struct qsockaddr *addr1, struct qsockaddr *addr2)
{

	return 0;
}

//=============================================================================

int WIPX_GetSocketPort (struct qsockaddr *addr)
{
	return 0;
}


int WIPX_SetSocketPort (struct qsockaddr *addr, int port)
{
	return 0;
}

//=============================================================================
