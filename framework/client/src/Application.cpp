#include <iostream>

#include "hge.h"

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "Bitstream.h"
#include "GetTime.h"

#include "config.h"
#include "MyMsgIDs.h"

#include "Globals.h"
#include "ship.h"
#include "Application.h"

#define BUFFERSIZE 256
/** 
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/

Application::Application() : 
	hge_(hgeCreate(HGE_VERSION)),
	fpsbox(0),
	databox(0),
	rejectBox(0),
	destoryedShipText(0),
	youDiedText(0),
	missileReadyText(0),
	haveMissileText(0),
	ammoLeftText(0),
	rakpeer_(RakNetworkFactory::GetRakPeerInterface()),
	timer_( 0 ),
	totalsent_(0),
	totalreceived_(0),
	mymissile(0),
	bulletFireRate(0.f),
	//haveMissile(false),
	have_missile( false ),
	rejectedFromServer(false),
	keydown_enter( false )//,
    //asteroid( 0 )
{
}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/

Application::~Application() throw()
{
	delete fpsbox;
	fpsbox = 0;
	delete databox;
	databox = 0;
	delete rejectBox;
	rejectBox = 0;
	delete destoryedShipText;
	destoryedShipText = 0;
	delete youDiedText;
	youDiedText = 0;
	delete haveMissileText;
	haveMissileText = 0;
	delete missileReadyText;
	missileReadyText = 0;
	delete ammoLeftText;
	ammoLeftText = 0;

	while (boomList.size() > 0)
	{
		Explosion *ex = boomList.back();
		delete ex;
		boomList.pop_back();
	}

	while (friendlyBulletList.size() > 0)
	{
		Bullets *ex = friendlyBulletList.back();
		delete ex;
		friendlyBulletList.pop_back();
	}

	/*if (!inactive_friendlyBulletList.empty())
	{
		inactive_friendlyBulletList.clear();
	}*/
	while (enemyBulletList.size() > 0)
	{
		Bullets *ex = enemyBulletList.back();
		delete ex;
		enemyBulletList.pop_back();
	}

	/*if (!inactive_enemyBulletList.empty())
	{
		inactive_enemyBulletList.clear();
	}*/
	while (buffList.size() > 0)
	{
		Buff *ex = buffList.back();
		delete ex;
		buffList.pop_back();
	}

	/*if (!inactive_buffList.empty())
	{
		inactive_buffList.clear();
	}*/

	if (blackHole != NULL)
	{
		delete blackHole;
	}

	Shutdown();
	rakpeer_->Shutdown(100);
	RakNetworkFactory::DestroyRakPeerInterface(rakpeer_);
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/

bool Application::Init()
{
	srand( RakNet::GetTime() );

	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "DM2241 Multiplayer Game Programming");
//	hge_->System_SetState(HGE_LOGFILE, "movement.log");
	hge_->System_SetState(HGE_DONTSUSPEND, true);

	if(hge_->System_Initiate()) 
	{
        //asteroid = new Asteroid( "asteroid.png" );
        
        fpsbox = new TextBox( "font1.fnt" );
		fpsbox->SetPos( 5, 5 );

		databox = new TextBox("font1.fnt");
		databox->SetPos( 580, 5 );

		float screenheight = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
		rejectBox = new TextBox("font1.fnt");
		rejectBox->SetPos(5, screenheight/2);
		destoryedShipText = new TextBox("font1.fnt");
		destoryedShipText->SetPos(5, screenheight / 2);
		youDiedText = new TextBox("font1.fnt");
		youDiedText->SetPos(5, screenheight / 2 - 10);
		haveMissileText = new TextBox("font1.fnt");
		haveMissileText->SetPos(5, screenheight / 2 - 30);
		missileReadyText = new TextBox("font1.fnt");
		missileReadyText->SetPos(5, screenheight / 2 - 50);
		ammoLeftText = new TextBox("font1.fnt");
		ammoLeftText->SetPos(5, screenheight / 2 - 70);

		float screenwidth = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
        int ShipType = rand() % 4 + 1;

		
		activeBullets = 0;
		float init_pos_x = (float)(screenwidth/4);
        float init_pos_y = (float)(rand() % 400 + 100);
        ships_.push_back( new Ship( ShipType, init_pos_x, init_pos_y ) );
        std::cout << "My Ship: type[" << ShipType << "] x[" << init_pos_x << "] y[" << init_pos_y << "]" << std::endl;
		ships_.at(0)->SetName("My Ship");

		blackHole = new Blackhole("blackhole.png",0,0,0);

		if (rakpeer_->Startup(1,30,&SocketDescriptor(), 1))
		{
			rakpeer_->SetOccasionalPing(true);
            return rakpeer_->Connect( DFL_SERVER_IP, DFL_PORTNUMBER, 0, 0 );
		}
	}
	return false;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
	std::string fps = "fps:";
	fps.append(std::to_string(1 / hge_->Timer_GetDelta()));
	fpsbox->mytext_ = fps;
    if( hge_->Input_GetKeyState( HGEK_ESCAPE ) )
        return true;

    float timedelta = hge_->Timer_GetDelta( );

    ships_.at( 0 )->SetAngularVelocity( 0.0f );

	if (ships_.at(0)->getActive())
	{
		if (hge_->Input_GetKeyState(HGEK_LEFT))
		{
			ships_.at(0)->SetAngularVelocity(ships_.at(0)->GetAngularVelocity() - DEFAULT_ANGULAR_VELOCITY);
		}

		if (hge_->Input_GetKeyState(HGEK_RIGHT))
		{
			ships_.at(0)->SetAngularVelocity(ships_.at(0)->GetAngularVelocity() + DEFAULT_ANGULAR_VELOCITY);
		}

		if (hge_->Input_GetKeyState(HGEK_UP))
		{
			ships_.at(0)->Accelerate(-DEFAULT_ACCELERATION, timedelta);
		}

		if (hge_->Input_GetKeyState(HGEK_DOWN))
		{
			ships_.at(0)->Accelerate(DEFAULT_ACCELERATION, timedelta);
		}


		if (hge_->Input_GetKeyState(HGEK_C) && mymissile == NULL && ships_.at(0)->getHaveMissile())
		{
			
				CreateMissile(ships_.at(0)->GetX(), ships_.at(0)->GetY(), ships_.at(0)->GetW(), ships_.at(0)->GetID());
				keydown_enter = true;
				ships_.at(0)->setHaveMissile(false);
		}
		bulletFireRate += timedelta;
		if (hge_->Input_GetKeyState(HGEK_SPACE) && bulletFireRate > 0.5f && activeBullets < 3)
		{
			CreateBullets(ships_.at(0)->GetX(), ships_.at(0)->GetY(), ships_.at(0)->GetW(), ships_.at(0)->GetID());
			bulletFireRate = 0.f;
		}
		
	}

    // update ships
	std::string msgForYouDied = "";
	for (ShipList::iterator ship = ships_.begin();
		ship != ships_.end(); ship++)
	{
		(*ship)->Update(timedelta);
		if (ship == ships_.begin())
		{
			if ((*ship)->getHealth() <= 0)
			{
				(*ship)->getActive() = false;
				msgForYouDied = "You Died!, respawning soon...";
			}
		}
	}
	youDiedText->mytext_ = msgForYouDied;

    // update asteroid
    //if( asteroid )
    //{
    //    if( asteroid->Update( ships_, timedelta ) )
    //    {
    //        //	delete asteroid;
    //        //	asteroid = 0;
    //    }
    //}


	// update missiles
	if( mymissile )
	{
		if (mymissile->Update(ships_, timedelta) || blackHole->Update(mymissile, timedelta, false))
		{
			// have collision
			if (mymissile->GetIsDestroyed()
				|| blackHole->getCollision())
			{
				blackHole->setCollision(false);
				Explosion *ex = FetchBoom();
				ex->setPos(mymissile->GetX(), mymissile->GetY());
			}
			delete mymissile;
			mymissile = 0;
		}
	}

	for (MissileList::iterator missile = missiles_.begin();
		missile != missiles_.end(); missile++)
	{
		if ((*missile)->Update(ships_, timedelta) || blackHole->Update(*missile, timedelta, true))
		{
			// have collision
			if ((*missile)->GetIsDestroyed()
				||blackHole->getCollision())
			{
				blackHole->setCollision(false);
				Explosion *ex = FetchBoom();
				ex->setPos((*missile)->GetX(), (*missile)->GetY());
			}
			delete *missile;
			missiles_.erase(missile);
			break;
		}
	}

	if (!friendlyBulletList.empty())
	{
		activeBullets = 0;
		for (std::vector<Bullets*>::iterator it = friendlyBulletList.begin(); it != friendlyBulletList.end(); ++it)
		{
			if ((*it)->getActive())
			{
				activeBullets += 1;
				if ((*it)->Update(ships_, timedelta) || blackHole->Update((*it), timedelta, false))
				{
					// have collision
					if ((*it)->GetIsDestroyed())
					{
						blackHole->setCollision(false);
						Explosion *ex = FetchBoom();
						ex->setPos((*it)->GetX(), (*it)->GetY());
					}
				}
			}
			else
			{
				//delete (*it);
				//inactive_friendlyBulletList.push_back(it - friendlyBulletList.begin());
			}
		}
	}
	if (!enemyBulletList.empty())
	{
		for (std::vector<Bullets*>::iterator it = enemyBulletList.begin(); it != enemyBulletList.end(); ++it)
		{
			if ((*it)->getActive())
			{
				if ((*it)->Update(ships_, timedelta) || blackHole->Update((*it), timedelta, true))
				{
					// have collision
					if ((*it)->GetIsDestroyed() ||blackHole->getCollision())
					{
						blackHole->setCollision(false);
						Explosion *ex = FetchBoom();
						ex->setPos((*it)->GetX(), (*it)->GetY());
					}
				}
			}
			else
			{
				//delete (*it);
				//inactive_enemyBulletList.push_back(it - enemyBulletList.begin());
			}
		}
	}

	blackHole->Update(timedelta);
	/*if (!inactive_friendlyBulletList.empty())
	{
		for (std::vector<size_t>::reverse_iterator rit = inactive_friendlyBulletList.rbegin(), rend = inactive_friendlyBulletList.rend(); rit != rend; ++rit)
		{
			friendlyBulletList.erase(friendlyBulletList.begin() + (*rit));
		}
		inactive_friendlyBulletList.clear();
	}

	if (!inactive_enemyBulletList.empty())
	{
		for (std::vector<size_t>::reverse_iterator rit = inactive_enemyBulletList.rbegin(), rend = inactive_enemyBulletList.rend(); rit != rend; ++rit)
		{
			enemyBulletList.erase(enemyBulletList.begin() + (*rit));
		}
		inactive_enemyBulletList.clear();
	}*/


	if (!buffList.empty())
	{
		for (std::vector<Buff*>::iterator it = buffList.begin(); it != buffList.end(); ++it)
		{
			if ((*it)->getActive())
			{
				if ((*it)->Update(ships_, timedelta))
				{
					// have collision
					/*if ((*it)->GetIsTouched())
					{
						Explosion *ex = FetchBoom();
						ex->setPos((*it)->GetX(), (*it)->GetY());
					}*/
				}
			}
			else
			{
				//delete (*it);
				//inactive_buffList.push_back(it - buffList.begin());
			}
		}
	}

	/*if (!inactive_buffList.empty())
	{
		for (std::vector<size_t>::reverse_iterator rit = inactive_buffList.rbegin(), rend = inactive_buffList.rend(); rit != rend; ++rit)
		{
			buffList.erase(buffList.begin() + (*rit));
		}
		inactive_buffList.clear();
	}*/
	std::string havemissiles = "Have Missiles? : ";
	if (ships_.at(0)->getHaveMissile())
	{
		havemissiles.append("true");
	}
	else
	{
		havemissiles.append("false");
	}
	std::string readymissiles = "Able to Fire missile? : ";
	if (mymissile == NULL)
	{
		readymissiles.append("true");
	}
	else
	{
		readymissiles.append("false");
	}
	std::string ammoleft = "Ammo for lasergun : ";
	ammoleft.append(std::to_string(3 - activeBullets) + "/3");
	if (activeBullets > 0)
	{
		ammoleft.append(" Recharging");
	}
	
  	haveMissileText->mytext_ = havemissiles;
	missileReadyText->mytext_ = readymissiles;
	ammoLeftText->mytext_ = ammoleft;
	

	if (!rejectedFromServer)
	{
		if (Packet* packet = rakpeer_->Receive())
		{
			RakNet::BitStream bs(packet->data, packet->length, false);

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
			case ID_CONNECTION_REQUEST_ACCEPTED:
				std::cout << "Connected to Server" << std::endl;
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "Lost Connection to Server" << std::endl;
				rakpeer_->DeallocatePacket(packet);
				return true;

			case ID_WELCOME:
			{
							   unsigned int shipcount, id, playerNumber;
							   float x_, y_;
							   int type_;
							   std::string temp;
							   char chartemp[5];

							   bs.Read(id);
							   ships_.at(0)->setID(id);
							   bs.Read(shipcount);

							   for (unsigned int i = 0; i < shipcount; ++i)
							   {
								   bs.Read(id);
								   bs.Read(x_);
								   bs.Read(y_);
								   bs.Read(type_);
								   std::cout << "Welcome Ship pos " << x_ << ", " << y_ << ", type " << type_ << std::endl;
								   Ship* ship = new Ship(type_, x_, y_);
								   temp = "EnemyShip ";
								   temp += _itoa_s(id, chartemp, 10);
								   ship->SetName(temp.c_str());
								   ship->setID(id);

								   ships_.push_back(ship);
							   }
							   bs.Read(playerNumber);
							   float screenwidth = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
							   if (playerNumber == 1)
							   {
								   ships_.at(0)->SetX(screenwidth / 4);
								   ships_.at(0)->SetServerX(screenwidth/4);
							   }
							   else
							   {
								   ships_.at(0)->SetX((screenwidth / 4) * 3);
								   ships_.at(0)->SetServerX((screenwidth / 4) * 3);
							   }

							   SendInitialPosition();
			}
				break;

			case ID_REJECT:
			{
							  rejectBox->mytext_ = "There is already 2 Players in the server! pls join in later!";
							  rejectedFromServer = true;
			}
				break;
			case ID_NEWSHIP:
			{
							   unsigned int id;
							   bs.Read(id);

							   if (id == ships_.at(0)->GetID())
							   {
								   // if it is me
								   break;
							   }
							   else
							   {
								   float x_, y_;
								   int type_;
								   std::string temp;
								   char chartemp[5];

								   bs.Read(x_);
								   bs.Read(y_);
								   bs.Read(type_);
								   std::cout << "New Ship pos" << x_ << " " << y_ << std::endl;
								   Ship* ship = new Ship(type_, x_, y_);
								   temp = "Enemy Ship ";
								   temp += _itoa_s(id, chartemp, 10);
								   ship->SetName(temp.c_str());
								   ship->setID(id);
								   ships_.push_back(ship);
							   }
			}
				break;

			case ID_LOSTSHIP:
			{
								unsigned int shipid;
								bs.Read(shipid);
								for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
								{
									if ((*itr)->GetID() == shipid)
									{
										delete *itr;
										ships_.erase(itr);
										break;
									}
								}
			}
				break;

			case ID_INITIALPOS:
				break;

			case ID_MOVEMENT:
			{
								totalreceived_ += bs.GetNumberOfBytesUsed();

								unsigned int shipid, bulletShipid;
								float server_x, server_y, server_w, bulletx, bullety, bulletw, bulletvx, bulletvy;
								float server_vel_x, server_vel_y, server_vel_angular, time;
								int health;
								bool isActive,haveBuff, activeBullet;
								bs.Read(shipid);
								std::string str = "";
								for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
								{
									if ((*itr)->GetID() == shipid)
									{
										bs.Read(server_x);
										bs.Read(server_y);
										bs.Read(server_w);
										bs.Read(server_vel_x);
										bs.Read(server_vel_y);
										bs.Read(server_vel_angular);
										bs.Read(health);
										bs.Read(time);
										bs.Read(isActive);
										if (health <= 0)
										{
											str.append((*itr)->getName());
											str.append(" has died! Respawning soon! ");
											
										}
										if (!enemybuffList.empty())
										{
											for (std::vector<Buff*>::iterator it = enemybuffList.begin(); it != enemybuffList.end(); ++it)
											{
												bs.Read(haveBuff);
												(*it)->getActive() = haveBuff;

											}
										}

										if (!enemyBulletList.empty())
										{
											for (std::vector<Bullets *>::iterator it = enemyBulletList.begin(); it != enemyBulletList.end(); ++it)
											{
												Bullets *bt = (*it);
												
												bs.Read(activeBullet);
												bs.Read(bulletShipid);
												bs.Read(bulletx);
												bs.Read(bullety);
												bs.Read(bulletw);
												bs.Read(bulletvx);
												bs.Read(bulletvy);
												
												bt->getActive() = activeBullet;
												bt->setID(bulletShipid);
												//bt->initialise(bulletx, bullety, bulletw, bulletShipid);
												bt->SetServerLocation(bulletx, bullety, bulletw);
												bt->SetServerVelocity(bulletvx, bulletvy, 0);
												bt->DoInterpolateUpdate();
											}
										}
										destoryedShipText->mytext_ = str;

										(*itr)->SetServerLocation(server_x, server_y, server_w);
										(*itr)->SetServerVelocity(server_vel_x, server_vel_y, server_vel_angular);
										(*itr)->getHealth() = health;
										(*itr)->setTimeToRespawn(time);
										(*itr)->getActive() = isActive;
										(*itr)->DoInterpolateUpdate();
										break;
									}
									destoryedShipText->mytext_ = str;
								}
								
			}
				break;

			case ID_COLLIDE:
			{
							   unsigned int shipid;
							   float x, y;
							   bs.Read(shipid);

							   if (shipid == ships_.at(0)->GetID())
							   {
								   std::cout << "collided with someone!" << std::endl;
								   bs.Read(x);
								   bs.Read(y);
								   ships_.at(0)->SetX(x);
								   ships_.at(0)->SetY(y);
								   bs.Read(x);
								   bs.Read(y);
								   ships_.at(0)->SetVelocityX(x);
								   ships_.at(0)->SetVelocityY(y);
#ifdef INTERPOLATEMOVEMENT
								   bs.Read(x);
								   bs.Read(y);
								   ships_.at(0)->SetServerVelocityX(x);
								   ships_.at(0)->SetServerVelocityY(y);
#endif	
							   }
			}
				break;


			case ID_NEWMISSILE:
			{
								  float x, y, w;
								  int id;

								  bs.Read(id);
								  bs.Read(x);
								  bs.Read(y);
								  bs.Read(w);

								  missiles_.push_back(new Missile("missile.png", x, y, w, id));
			}
				break;
			case ID_NEWBULLET:
			{
								  float x, y, w;
								  int id;

								  bs.Read(id);
								  bs.Read(x);
								  bs.Read(y);
								  bs.Read(w);

								  Bullets* bt = FetchEnemyBullets();
								  bt->initialise(x, y, w, id);
								  //std::cout << "hi";
			}
				break;
			case ID_UPDATEMISSILE:
			{
									 float x, y, w,vx,vy;
									 int id;
									 char deleted;

									 bs.Read(id);
									 bs.Read(deleted);

									 for (MissileList::iterator itr = missiles_.begin(); itr != missiles_.end(); ++itr)
									 {
										 if ((*itr)->GetOwnerID() == id)
										 {
											 if (deleted)
											 {
												 delete *itr;
												 
												 missiles_.erase(itr);
											 }
											 else
											 {
												 bs.Read(x);
												 bs.Read(y);
												 bs.Read(w);
												 bs.Read(vx);
												 bs.Read(vy);
												
												 (*itr)->SetServerLocation(x, y, w);
												 (*itr)->SetServerVelocity(vx, vy, 0);
												 (*itr)->DoInterpolateUpdate();
											 }
											 break;
										 }
									 }

			}
				break;
			
			case ID_NEWBUFF:
			{
							   
							   int buffType;
							   float x1, x2, y1, y2;
							   bs.Read(x1);
							   bs.Read(y1);
							   bs.Read(x2);
							   bs.Read(y2);
							   bs.Read(buffType);
							   if (x1 == ships_.at(0)->GetX())
							   {
								   Buff* buff1 = FetchBuff();
								   Buff* buff2 = FetchEnemyBuff();
								   buff1->initialise(x1, y1, buffType);
								   buff2->initialise(x2, y2, buffType);
							   }
							   else
							   {
								   Buff* buff1 = FetchEnemyBuff();
								   Buff* buff2 = FetchBuff();
								   buff1->initialise(x1, y1, buffType);
								   buff2->initialise(x2, y2, buffType);
							   }
							  

			}
				break;
			case ID_SPAWNBLACKHOLE:
			{
									  float x, y;
									  bs.Read(x);
									  bs.Read(y);
									  blackHole->getActive() = true;
									  blackHole->initialise(x, y,0);
			}
				break;
			default:
				std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;

			}
			rakpeer_->DeallocatePacket(packet);
		}

		if (RakNet::GetTime() - timer_ > 100)
		{
			timer_ = RakNet::GetTime();

			RakNet::BitStream bs2;
			unsigned char msgid = ID_MOVEMENT;
			bs2.Write(msgid);
			bs2.Write(ships_.at(0)->GetID());
			bs2.Write(ships_.at(0)->GetServerX());
			bs2.Write(ships_.at(0)->GetServerY());
			bs2.Write(ships_.at(0)->GetServerW());
			bs2.Write(ships_.at(0)->GetServerVelocityX());
			bs2.Write(ships_.at(0)->GetServerVelocityY());
			bs2.Write(ships_.at(0)->GetAngularVelocity());
			bs2.Write(ships_.at(0)->getHealth());
			bs2.Write(ships_.at(0)->getTimeToRespawn());
			bs2.Write(ships_.at(0)->getActive());

			if (!buffList.empty())
			{
				for (std::vector<Buff*>::iterator it = buffList.begin(); it != buffList.end(); ++it)
				{
					bs2.Write((*it)->getActive());
					
				}
			}

			for (std::vector<Bullets *>::iterator it = friendlyBulletList.begin(); it != friendlyBulletList.end(); ++it)
			{
				Bullets *bt = (*it);
				
				bs2.Write(bt->getActive());
				bs2.Write(bt->GetOwnerID());
				bs2.Write(bt->GetServerX());
				bs2.Write(bt->GetServerY());
				bs2.Write(bt->GetServerW());
				bs2.Write(bt->GetServerVelocityX());
				bs2.Write(bt->GetServerVelocityY());
					
				
			}

			rakpeer_->Send(&bs2, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			totalsent_ += bs2.GetNumberOfBytesUsed();

			// send missile update to server
			if (mymissile)
			{
				RakNet::BitStream bs3;
				unsigned char msgid2 = ID_UPDATEMISSILE;
				unsigned char deleted = 0;
				
				bs3.Write(msgid2);
				bs3.Write(mymissile->GetOwnerID());
				bs3.Write(deleted);
				bs3.Write(mymissile->GetServerX());
				bs3.Write(mymissile->GetServerY());
				bs3.Write(mymissile->GetServerW());
				bs3.Write(mymissile->GetServerVelocityX());
				bs3.Write(mymissile->GetServerVelocityY());

				rakpeer_->Send(&bs3, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			/*if (!friendlyBulletList.empty())
			{
				RakNet::BitStream bs4;
				unsigned char deleted = 0;
				bool toUpdateBullet = false;
				for (std::vector<Bullets *>::iterator it = friendlyBulletList.begin(); it != friendlyBulletList.end(); ++it)
				{
					Bullets *bt = (*it);
					if (bt->getActive())
					{

						if ((it) == friendlyBulletList.begin())
						{
							unsigned char msgid3 = ID_UPDATEBULLET;
							bs4.Write(msgid3);
						}
						toUpdateBullet = true;
						bs4.Write(bt->GetOwnerID());
						bs4.Write(deleted);
						bs4.Write(bt->GetX());
						bs4.Write(bt->GetY());
						bs4.Write(bt->GetW());
					}
				}
				if (toUpdateBullet)
				{
					rakpeer_->Send(&bs4, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
			}*/
		}
	}
	UpdateBoomList(timedelta);
	return false;
}


/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{
	std::string fps;
	char temp[BUFFERSIZE];

	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);

	//databox->mytext_ = "hi";
	blackHole->Render();

	for (std::vector<Bullets*>::iterator it = friendlyBulletList.begin(); it != friendlyBulletList.end(); ++it)
	{
		(*it)->Render();
	}
	for (std::vector<Bullets*>::iterator it = enemyBulletList.begin(); it != enemyBulletList.end(); ++it)
	{
		(*it)->Render();
	}

    // render astroid
    /*if( asteroid )
    {
        asteroid->Render( );
    }*/

    // render spaceships
	ShipList::iterator itr;
	for (itr = ships_.begin(); itr != ships_.end(); itr++)
	{
		(*itr)->Render();
	}

	RenderBoomList();

	
	// render missiles
	if( mymissile )
	{
		mymissile->Render();
	}
	MissileList::iterator itr2;
	for (itr2 = missiles_.begin(); itr2 != missiles_.end(); itr2++)
	{
		(*itr2)->Render();
	}
	for (std::vector<Buff*>::iterator it = buffList.begin(); it != buffList.end(); ++it)
	{
		(*it)->Render();
	}
	for (std::vector<Buff*>::iterator it = enemybuffList.begin(); it != enemybuffList.end(); ++it)
	{
		(*it)->Render();
	}

	fpsbox->Render();
	databox->Render();
	rejectBox->Render();
	destoryedShipText->Render();
	youDiedText->Render();

	haveMissileText->Render();
	missileReadyText->Render();
	ammoLeftText->Render();

	hge_->Gfx_EndScene();
}


/** 
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/

void Application::Shutdown()
{
	ShipList::iterator itr = ships_.begin();

	while( itr != ships_.end() )
	{
		delete *itr;
		ships_.erase(itr);
		itr = ships_.begin();
		if( itr == ships_.end() ) break;
//		itr++;
	//	if( itr == ships_.end()

	}
/*
	for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); itr++)
	{			
		delete *itr;
		ships_.erase(itr);
		itr = ships_.begin();
	}
*/
	hge_->System_Shutdown();
	hge_->Release();
}

/** 
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
	}
}

bool Application::SendInitialPosition()
{
	RakNet::BitStream bs;
	unsigned char msgid = ID_INITIALPOS;
	bs.Write(msgid);
	bs.Write(ships_.at(0)->GetX());
	bs.Write(ships_.at(0)->GetY());
    bs.Write( ships_.at( 0 )->GetType( ) );
	float screenheight = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	float screenWidth = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	bs.Write(screenWidth);
	bs.Write(screenheight);
	std::cout << "Sending pos " << ships_.at(0)->GetX() << ", " << ships_.at(0)->GetY() << ", type " << ships_.at(0)->GetType() << std::endl;

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

	return true;
}

void Application::CreateMissile(float x, float y, float w, int id)
{
	RakNet::BitStream bs;
	unsigned char msgid;
	unsigned char deleted=0;

	if( id != ships_.at(0)->GetID() )
	{
		// not my ship
		missiles_.push_back( new Missile("missile.png", x, y, w, id ) );
	}
	else
	{		
		if( have_missile )
		{
			//locate existing missile

			// send network command to delete across all clients
				deleted=1;
				msgid = ID_UPDATEMISSILE;
				bs.Write(msgid);
				bs.Write(id);
				bs.Write(deleted);
				bs.Write(x);
				bs.Write(y);
				bs.Write(w);
				
				rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

				have_missile = false;
		}
		
		// add new missile to list
		mymissile = new Missile("missile.png", x, y, w, id );
			
		// send network command to add new missile
		bs.Reset();
		msgid = ID_NEWMISSILE;
		bs.Write(msgid);
		bs.Write(id);
		bs.Write(x);
		bs.Write(y);
		bs.Write(w);
		
		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

		have_missile = true;
	}
}

Explosion* Application::FetchBoom()
{
	for (std::vector<Explosion *>::iterator it = boomList.begin(); it != boomList.end(); ++it)
	{
		Explosion *ex = (Explosion *)*it;
		if (!ex->getActive())
		{
			ex->setActive(true);
			//++m_objectCount;
			return ex;
		}
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		Explosion *ex = new Explosion("boom.png");
		boomList.push_back(ex);
	}
	Explosion *ex = boomList.back();
	ex->setActive(true);
	//++m_objectCount;
	return ex;
}
void Application::RenderBoomList()
{
	for (std::vector<Explosion *>::iterator it = boomList.begin(); it != boomList.end(); ++it)
	{
		Explosion *ex = (Explosion *)*it;
		if (ex->getActive())
		{
			ex->Render();
		}
	}
}
void Application::UpdateBoomList(float dt)
{
	for (std::vector<Explosion *>::iterator it = boomList.begin(); it != boomList.end(); ++it)
	{
		Explosion *ex = (Explosion *)*it;
		if (ex->getActive())
		{
			ex->Update(dt);
		}
	}
}

void Application::CreateBullets(float x, float y, float w, int id)
{
	RakNet::BitStream bs;
	unsigned char msgid;
	unsigned char deleted = 0;

	if (id != ships_.at(0)->GetID())
	{
		// not my ship
		enemyBulletList.push_back(new Bullets("bullet.png", x, y, w, id));
	}
	else
	{
		// add new bullet to list
		Bullets* bt = FetchBullets();
		bt->initialise(x, y, w, id);

		// send network command to add new missile
		bs.Reset();
		msgid = ID_NEWBULLET;
		bs.Write(msgid);
		bs.Write(id);
		bs.Write(x);
		bs.Write(y);
		bs.Write(w);

		rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

		have_missile = true;
	}
}

Bullets* Application::FetchBullets()
{
	for (std::vector<Bullets *>::iterator it = friendlyBulletList.begin(); it != friendlyBulletList.end(); ++it)
	{
		Bullets *bt = (Bullets *)*it;
		if (!bt->getActive())
		{
			bt->getActive() = true;
			bt->SetIsDesapwn(false);
			bt->SetIsDestroyed(false);
			//++m_objectCount;
			return bt;
		}
	}
	for (unsigned i = 0; i < 3; ++i)
	{
		Bullets *bt = new Bullets("bullet.png", 0, 0, 0, 0);
		friendlyBulletList.push_back(bt);
	}
	Bullets *bt = friendlyBulletList.back();
	bt->getActive() = true;
	//++m_objectCount;
	return bt;
}

Bullets* Application::FetchEnemyBullets()
{
	for (std::vector<Bullets *>::iterator it = enemyBulletList.begin(); it != enemyBulletList.end(); ++it)
	{
		Bullets *bt = (Bullets *)*it;
		if (!bt->getActive())
		{
			bt->getActive() = true;
			bt->SetIsDesapwn(false);
			bt->SetIsDestroyed(false);
			//++m_objectCount;
			return bt;
		}
	}
	for (unsigned i = 0; i < 3; ++i)
	{
		Bullets *bt = new Bullets("bullet.png", 0, 0, 0, 0);
		enemyBulletList.push_back(bt);
	}
	Bullets *bt = enemyBulletList.back();
	bt->getActive() = true;
	//++m_objectCount;
	return bt;
}


Buff* Application::FetchBuff()
{
	for (std::vector<Buff *>::iterator it = buffList.begin(); it != buffList.end(); ++it)
	{
		Buff *bt = (Buff *)*it;
		if (!bt->getActive())
		{
			bt->getActive() = true;
			//++m_objectCount;
			return bt;
		}
	}
	Buff *bt = new Buff(0,0,1);
	buffList.push_back(bt);
	//Buff *bt = buffList.back();
	bt->getActive() = true;
	//++m_objectCount;
	return bt;
}

Buff* Application::FetchEnemyBuff()
{
	for (std::vector<Buff *>::iterator it = enemybuffList.begin(); it != enemybuffList.end(); ++it)
	{
		Buff *bt = (Buff *)*it;
		if (!bt->getActive())
		{
			bt->getActive() = true;
			//++m_objectCount;
			return bt;
		}
	}
	Buff *bt = new Buff(0, 0, 1);
	enemybuffList.push_back(bt);
	//Buff *bt = buffList.back();
	bt->getActive() = true;
	//++m_objectCount;
	return bt;
}