#include <math.h>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "missile.h"


Missile::Missile(char* filename, float x, float y, float w, int shipid ) :
	angular_velocity(0),
	isDestroyed(false),
	isDespawned(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 20));
	sprite_->SetHotSpot(20,10);
	x_ = x;
	y_ = y;
	w_ = w;
	ownerid = shipid;

	velocity_x_ = 200.0f * cosf(w_);
	velocity_y_ = 200.0f * sinf(w_); 

	x_ += velocity_x_ * 0.5f;
	y_ += velocity_y_ * 0.5f;
	w_ = atan2(velocity_y_, velocity_x_);
}

Missile::~Missile()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

bool Missile::Update(std::vector<Ship*> &shiplist, float timedelta)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	float pi = 3.141592654f*2;
	float oldx, oldy;

	w_ += angular_velocity * timedelta;
	if (w_ > pi)
		w_ -= pi;

	if (w_ < 0.0f)
		w_ += pi;

	oldx = x_;
	oldy = y_;
	

	for (std::vector<Ship*>::iterator thisship = shiplist.begin();
		thisship != shiplist.end(); thisship++)
	{
		if (HasCollided((*(*thisship))) && (*thisship)->getActive() && ownerid != (*thisship)->GetID())
		{
			// if both are stuck
			(*thisship)->getHealth() -= 50;
			isDestroyed = true;
			return true;
		}
		else if (ownerid != (*thisship)->GetID() && (*thisship)->getActive())//homing, since there is only 2 ships everytime in the server.
		{
			float newVelX, newVelY, length;
			newVelX = (*thisship)->GetX() - x_;
			newVelY = (*thisship)->GetY() - y_;
			length = sqrt(newVelX * newVelX + newVelY * newVelY);
			velocity_x_ = (newVelX / length) * 100;
			velocity_y_ = (newVelY / length) * 100;
			w_ = atan2(velocity_y_, velocity_x_);
		}
	}
	x_ += velocity_x_ * timedelta;
	y_ += velocity_y_ * timedelta;
	
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
		return true;
	}
		

	return false;
}

void Missile::Render()
{
	sprite_->RenderEx(x_, y_, w_);
}

bool Missile::HasCollided( Ship &ship )
{
	sprite_->GetBoundingBox( x_, y_, &collidebox);

	return collidebox.Intersect( ship.GetBoundingBox() );
}