#ifndef __I_NET_H__
#define __I_NET_H__

#include <stdint.h>

// Called by D_DoomMain.
int I_InitNetwork (void);
void I_NetCmd (void);
void I_NetMessage(const char*, ...);
void I_NetError(const char* error);
void I_NetProgress(int val);
void I_NetInit(const char* msg, int num);
bool I_NetLoop(bool (*timer_callback)(void*), void* userdata);
void I_NetDone();

enum ENetConstants
{
	DOOMCOM_ID = 0x12345678,
	BACKUPTICS = 36,	// number of tics to remember
	MAXTICDUP = 3,
	LOCALCMDTICS = (BACKUPTICS*MAXTICDUP),
	MAX_MSGLEN = 14000,

	CMD_SEND = 1,
	CMD_GET = 2,
};

enum ENCMD
{
	NCMD_NEWHOST			= 0x100,	// Current host has chosen a new server host.
	NCMD_EXIT				= 0x80,		// Client has left the game
	NCMD_RETRANSMIT 		= 0x40,		// 
	NCMD_SETUP				= 0x20,		// Guest is letting the host know who it is
	NCMD_MULTI				= 0x10,		// Guest is getting multiple players in this packet (packet server only)
	NCMD_QUITTERS			= 0x08,		// Client is getting info about one or more players quitting (packet server only)
	NCMD_COMPRESSED			= 0x04,		// Remainder of packet is compressed

	NCMD_XTICS				= 0x03,		// Packet contains >2 net tics
	NCMD_2TICS				= 0x02,		// Packet contains 2 net tics
	NCMD_1TICS				= 0x01,		// Packet contains 1 net tic
	NCMD_0TICS				= 0x00,		// Packet contains no net tics

	NCMD_USERINFO			= NCMD_SETUP + 1,	// Guest is getting another client's user info
	NCMD_GAMEINFO			= NCMD_SETUP + 2,	// Guest is getting the state of the game from the host
	NCMD_GAMEREADY			= NCMD_SETUP + 3,	// Host has verified the game is ready to be started
};

struct FClientStack : public TArray<int>
{
	inline bool InGame(int i) const { return SortedFind(i, true) < Size(); }

	void operator+=(const int i)
	{
		if (!InGame(i))
			SortedInsert(i);
	}

	void operator-=(const int i)
	{
		SortedDelete(i);
	}
};

extern FClientStack NetworkClients;

//
// Network packet data.
//
struct doomcom_t
{
	// info common to all nodes
	uint32_t id;			// should be DOOMCOM_ID
	int16_t	ticdup;			// 1 = no duplication, 2-3 = dup for slow nets
	int16_t	numplayers;

	// info specific to this node
	int16_t	consoleplayer;

	// communication between DOOM and the driver
	int16_t	command;		// CMD_SEND or CMD_GET
	int16_t	remoteplayer;		// dest for send, set by get (-1 = no packet).
	// packet data to be sent
	int16_t	datalength;		// bytes in data to be sent
	uint8_t	data[MAX_MSGLEN];
}; 

extern doomcom_t doomcom;
extern bool netgame, multiplayer;
extern int consoleplayer;

#endif
