#ifndef SERVERAPP_H_
#define SERVERAPP_H_

#include <map>
#include "RakNetTypes.h"

class RakPeerInterface;

struct GameObject 
{
	GameObject(unsigned int newid)
	: x_(0), y_(0)
	{
		id = newid;
	}

	unsigned int id;
	float x_;
	float y_;
    int type_;
};

class ServerApp
{
	RakPeerInterface* rakpeer_;
	typedef std::map<SystemAddress, GameObject> ClientMap;

	ClientMap clients_;
	SystemAddress defaultAddr;
	SystemAddress players[2];

    unsigned int newID;
	float dt;
	float timeFromLastFrame;
	float timeToSpawnPowerUp;
	float timeToSpawnBlackHole;
	float timeToSpawnTimeBomb;
	float screenwidth;
	float screenheight;

	bool haveBlackHole;
	
	void SendWelcomePackage(SystemAddress& addr);
	void SendDisconnectionNotification(SystemAddress& addr);
	void ProcessInitialPosition( SystemAddress& addr, float x_, float y_, int type_);
    void UpdatePosition( SystemAddress& addr, float x_, float y_ );
	void spawnPowerUp();
	void spawnBlackHole();
	void spawnTimeBomb();
public:
	ServerApp();
	~ServerApp();
	void Loop();
};

#endif