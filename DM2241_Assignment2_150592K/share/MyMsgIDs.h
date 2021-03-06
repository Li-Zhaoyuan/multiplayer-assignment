#ifndef MYSMSGIDS_H_
#define MYSMSGIDS_H_

#include "MessageIdentifiers.h"

enum MyMsgIDs
{
	ID_WELCOME = ID_USER_PACKET_ENUM,
	ID_REJECT,
	ID_NEWSHIP,
	ID_LOSTSHIP,
	ID_INITIALPOS,
	ID_MOVEMENT,
    ID_COLLIDE,
    ID_NEWMISSILE,
	ID_UPDATEMISSILE,
	ID_NEWBULLET,
	ID_UPDATEBULLET,
	ID_NEWBUFF,
	ID_SPAWNBLACKHOLE,
	ID_SPAWNTIMEBOMB
};

#endif