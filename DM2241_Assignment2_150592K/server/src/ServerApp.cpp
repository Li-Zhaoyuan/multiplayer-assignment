#include <iostream>

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "Bitstream.h"
#include "GetTime.h"

#include "config.h"
#include "MyMsgIDs.h"

#include "ServerApp.h"
#include <string>

ServerApp::ServerApp() : 
	rakpeer_(RakNetworkFactory::GetRakPeerInterface()),
	newID(0),
	dt(0.f),
	timeFromLastFrame(0.f),
	timeToSpawnPowerUp(0.f),
	timeToSpawnBlackHole(0.f),
	timeToSpawnTimeBomb(0.0f)
{
    rakpeer_->Startup( 100, 30, &SocketDescriptor( DFL_PORTNUMBER, 0 ), 1 );
    rakpeer_->SetMaximumIncomingConnections( DFL_MAX_CONNECTION );
	rakpeer_->SetOccasionalPing(true);
	std::cout << "Server Started" << std::endl;
}

ServerApp::~ServerApp()
{
	rakpeer_->Shutdown(100);
	RakNetworkFactory::DestroyRakPeerInterface(rakpeer_);
}

void ServerApp::Loop()
{
	dt = (RakNet::GetTime() - timeFromLastFrame)/1000.0f;
	timeFromLastFrame = RakNet::GetTime();
	if (clients_.size() >= 2)
	{
		timeToSpawnPowerUp += dt;
		timeToSpawnBlackHole += dt;
		timeToSpawnTimeBomb += dt;
		if (timeToSpawnPowerUp >= 10.f)
		{
			spawnPowerUp();
			timeToSpawnPowerUp = 0.f;
		}
		if (timeToSpawnBlackHole > 15.f)
		{
			spawnBlackHole();
			timeToSpawnBlackHole = 0.f;
		}
		if (timeToSpawnTimeBomb> 20.f)
		{
			spawnTimeBomb();
			timeToSpawnTimeBomb = 0.f;
		}
	}
	else
	{
		timeToSpawnPowerUp = 0.f;
		timeToSpawnBlackHole = 0.f;
		timeToSpawnTimeBomb = 0.f;
	}
	if (Packet* packet = rakpeer_->Receive())
	{
		RakNet::BitStream bs(packet->data, packet->length, false);
		RakNet::GetTime();
		unsigned char msgid = 0;
		RakNetTime timestamp = 0;

		bs.Read(msgid);

		if (msgid == ID_TIMESTAMP)
		{
			bs.Read(timestamp);
			bs.Read(msgid);
		}

		switch (msgid)
		{
		case ID_NEW_INCOMING_CONNECTION:
			SendWelcomePackage(packet->systemAddress);
			break;

		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			SendDisconnectionNotification(packet->systemAddress);
			break;

		case ID_INITIALPOS:
			{
				float x_, y_;
                int type_;
				std::cout << "ProcessInitialPosition" << std::endl;
				bs.Read( x_ );
				bs.Read( y_ );
                bs.Read( type_ );
				bs.Read(screenwidth);
				bs.Read(screenheight);
				ProcessInitialPosition( packet->systemAddress, x_, y_, type_);
			}
			break;

		case ID_MOVEMENT:
			// hint : movement stuffs goes here!
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			break;
		case ID_NEWMISSILE:
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			break;
		case ID_UPDATEMISSILE:
				bs.ResetReadPointer();
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			break;
		case ID_NEWBULLET:
			bs.ResetReadPointer();
			rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			break;
		/*case ID_UPDATEBULLET:
			bs.ResetReadPointer();
			rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
			break;*/
		default:
			std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;
		}

		rakpeer_->DeallocatePacket(packet);
	}
	
}
void ServerApp::spawnPowerUp()
{
	float RrandomY = rand() % 400 + 100;
	float LrandomY = rand() % 400 + 100;
	int buffType = rand() % 2;
	unsigned char msgid = ID_NEWBUFF;
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(screenwidth / 4);
	bs.Write(RrandomY);
	bs.Write((screenwidth / 4) * 3);
	bs.Write(LrandomY);
	bs.Write(buffType);
	for (ClientMap::iterator it = clients_.begin(); it != clients_.end(); ++it)
	{	
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, (it)->first, true);
	}
	bs.Reset();
}

void ServerApp::spawnTimeBomb()
{
	float RrandomY = rand() % 400 + 100;
	float LrandomY = rand() % 400 + 100;
	//int buffType = rand() % 2;

	unsigned char msgid = ID_SPAWNTIMEBOMB;
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(screenwidth / 4.f);
	bs.Write(RrandomY);
	bs.Write((screenwidth / 4.f) * 3.f);
	bs.Write(LrandomY);
	bs.Write(10.f);
	for (ClientMap::iterator it = clients_.begin(); it != clients_.end(); ++it)
	{
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, (it)->first, true);
	}
	bs.Reset();
}

void ServerApp::spawnBlackHole()
{
	float randomY = rand() % 400 + 100;
	float X = (screenwidth / 2);
	unsigned char msgid = ID_SPAWNBLACKHOLE;
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(X);
	bs.Write(randomY);
	for (ClientMap::iterator it = clients_.begin(); it != clients_.end(); ++it)
	{
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, (it)->first, true);
	}
	bs.Reset();
}

void ServerApp::SendWelcomePackage(SystemAddress& addr)
{
	++newID;
	unsigned int shipcount = static_cast<unsigned int>(clients_.size());
	if (shipcount < 2)
	{
		unsigned char msgid = ID_WELCOME;


		RakNet::BitStream bs;
		bs.Write(msgid);

		bs.Write(newID);
		bs.Write(shipcount);

		for (ClientMap::iterator itr = clients_.begin(); itr != clients_.end(); ++itr)
		{
			std::cout << "Ship " << itr->second.id << " pos" << itr->second.x_ << " " << itr->second.y_ << std::endl;
			bs.Write(itr->second.id);
			bs.Write(itr->second.x_);
			bs.Write(itr->second.y_);
			bs.Write(itr->second.type_);
		}
		unsigned int playerNumber = 0;

		for (int i = 0; i < 2; ++i)
		{
			if (players[i] == defaultAddr)
			{
				players[i] = addr;
				playerNumber = i + 1;
				break;
			}
		}
		bs.Write(playerNumber);
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, false);

		bs.Reset();

		GameObject newobject(newID);

		clients_.insert(std::make_pair(addr, newobject));

		std::cout << "New guy, assigned id " << newID << ", Player: " << playerNumber << std::endl;

	}
	else
	{
		unsigned char msgid = ID_REJECT;


		RakNet::BitStream bs;
		bs.Write(msgid);
		bs.Write(newID);
		bs.Write(shipcount);

		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, false);

		bs.Reset();
	}
}

void ServerApp::SendDisconnectionNotification(SystemAddress& addr)
{
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	unsigned char msgid = ID_LOSTSHIP;
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(itr->second.id);
	for (int i = 0; i < 2; ++i)
	{
		if (players[i] == addr)
		{
			players[i] = defaultAddr;
			break;
		}
	}

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);

	std::cout << itr->second.id << " has left the building" << std::endl;

	clients_.erase(itr);

}

void ServerApp::ProcessInitialPosition( SystemAddress& addr, float x_, float y_, int type_ ){
	unsigned char msgid;
	RakNet::BitStream bs;
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	itr->second.x_ = x_;
	itr->second.y_ = y_;
    itr->second.type_ = type_;

	std::cout << "Received pos " << itr->second.x_ << " " << itr->second.y_ << std::endl;
    std::cout << "Received type " << itr->second.type_ << std::endl;

	msgid = ID_NEWSHIP;
    bs.Write( msgid );
    bs.Write( itr->second.id );
    bs.Write( itr->second.x_ );
    bs.Write( itr->second.y_ );
    bs.Write( itr->second.type_ );

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);
}

void ServerApp::UpdatePosition( SystemAddress& addr, float x_, float y_ )
{
    ClientMap::iterator itr = clients_.find( addr );
    if( itr == clients_.end( ) )
        return;

    itr->second.x_ = x_;
    itr->second.y_ = y_;
}