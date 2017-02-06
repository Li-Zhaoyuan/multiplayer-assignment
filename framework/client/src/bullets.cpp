#include <math.h>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "bullets.h"


Bullets::Bullets(char* filename, float x, float y, float w, int shipid) :
angular_velocity(0),
isDestroyed(false),
isDespawned(false),
active(false)
, server_w_(0)
, client_w_(0)
, server_velx_(0)
, server_vely_(0)
, ratio_(1)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 20));
	sprite_->SetHotSpot(20, 10);
	x_ = server_x_ = client_x_ = x;
	y_ = server_y_ = client_y_ = y;
	w_ = w;
	ownerid = shipid;

	velocity_x_ = 200.0f * cosf(w_);
	velocity_y_ = 200.0f * sinf(w_);
	server_velx_ = velocity_x_;
	server_vely_ = velocity_y_;

	x_ += velocity_x_ * 0.5f;
	y_ += velocity_y_ * 0.5f;
	w_ = atan2(velocity_y_, velocity_x_);
}

Bullets::~Bullets()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

bool Bullets::Update(std::vector<Ship*> &shiplist, float timedelta)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	/*float pi = 3.141592654f * 2;
	float oldx, oldy;

	w_ += angular_velocity * timedelta;
	if (w_ > pi)
		w_ -= pi;

	if (w_ < 0.0f)
		w_ += pi;

	oldx = x_;
	oldy = y_;*/
	server_x_ += server_velx_ * timedelta;
	server_y_ += server_vely_ * timedelta;
	client_x_ += velocity_x_ * timedelta;
	client_y_ += velocity_y_ * timedelta;
	x_ = ratio_ * server_x_ + (1 - ratio_) * client_x_;
	y_ = ratio_ * server_y_ + (1 - ratio_) * client_y_;
	if (ratio_ < 1)
	{
		ratio_ += timedelta * 4;
		if (ratio_ > 1)
			ratio_ = 1;
	}

	w_ = atan2(server_vely_, server_velx_);

	for (std::vector<Ship*>::iterator thisship = shiplist.begin();
		thisship != shiplist.end(); thisship++)
	{
		if (HasCollided((*(*thisship))) && (*thisship)->getActive() && ownerid != (*thisship)->GetID())//make sure the ship collided is active
		{
			// if both are stuck
			(*thisship)->getHealth() -= 40;
			isDestroyed = true;
			active = false;
			return true;

			
		}
	}


	float screenwidth = static_cast<float>(hge->System_GetState(HGE_SCREENWIDTH));
	float screenheight = static_cast<float>(hge->System_GetState(HGE_SCREENHEIGHT));
	float spritewidth = sprite_->GetWidth();
	float spriteheight = sprite_->GetHeight();
	/*if (x_ < -spritewidth/2)
	x_ += screenwidth + spritewidth;
	else if (x_ > screenwidth + spritewidth/2)
	x_ -= screenwidth + spritewidth;

	if (y_ < -spriteheight/2)
	y_ += screenheight + spriteheight;
	else if (y_ > screenheight + spriteheight/2)
	y_ -= screenheight + spriteheight;*/
	if ((x_ < -spritewidth / 2) ||
		(x_ > screenwidth + spritewidth / 2) ||
		(y_ < -spriteheight / 2) ||
		(y_ > screenheight + spriteheight / 2))
	{
		isDespawned = true;
		active = false;
		return true;
	}


	return false;
}

void Bullets::Render()
{
	if (active)
	sprite_->RenderEx(x_, y_, w_);
}

bool Bullets::HasCollided(Ship &ship)
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return collidebox.Intersect(ship.GetBoundingBox());
}

hgeRect* Bullets::GetBoundingBox()
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return &collidebox;
}