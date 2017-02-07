#include <math.h>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "blackhole.h"
#include "missile.h"
#include "bullets.h"


Blackhole::Blackhole(char* filename, float x, float y, float w) :
angular_velocity(0),
scaling(0.1f),
active(false),
collision(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_->SetHotSpot(32, 32);
	x_ = x;
	y_ = y;
	w_ = w;

}

Blackhole::~Blackhole()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}
bool Blackhole::Update(float timedelta)
{
	if (active)
	{
		w_ += 5 * timedelta;
		if (scaling < 1.f)
		{
			scaling += timedelta;
		}
	}
	else
	{
		scaling = 0.1f;
	}
	return true;
}
bool Blackhole::Update(Bullets* &bullet, float timedelta, bool isEnemy)
{
	if (active)
	{
		if (!isEnemy)
		{
			/*float newVelX, newVelY, length;
					newVelX = (*thisship)->GetX() - x_;
					newVelY = (*thisship)->GetY() - y_;
					length = sqrt(newVelX * newVelX + newVelY * newVelY);
					velocity_x_ = (newVelX / length) * 100;
					velocity_y_ = (newVelY / length) * 100;*/
			float newVelX, newVelY, length;
			newVelX = x_ -(bullet)->GetX();
			newVelY = y_ - (bullet)->GetY();
			length = sqrt(newVelX * newVelX + newVelY * newVelY);
			if (length < 1000)
			{
				bullet->GetServerVelocityX() += (newVelX / length) * 100 * timedelta;
				bullet->GetServerVelocityY() += (newVelY / length) * 100 * timedelta;
			}
		}
		if (HasCollided((*(bullet))) && (bullet)->getActive())//make sure the ship collided is active
		{
			// if both are stuck	
			bullet->SetIsDestroyed(true);
			bullet->getActive() = false;
			collision = true;
			return true;


		}
	}
	return false;
}

bool Blackhole::Update(Missile* &missile, float timedelta, bool isEnemy)
{
	if (active)
	{
		if (!isEnemy)
		{
			float newVelX, newVelY, length;
			newVelX = x_ - (missile)->GetX();
			newVelY = y_ - (missile)->GetY();
			length = sqrt(newVelX * newVelX + newVelY * newVelY);
			if (length < 1500)
			{
				missile->GetServerVelocityX() += (newVelX / length) * 100 * timedelta;
				missile->GetServerVelocityY() += (newVelY / length) * 100 * timedelta;
			}
		}
		if (HasCollided((*(missile))) && (missile)->getActive())//make sure the ship collided is active
		{
			// if both are stuck
			missile->SetIsDestroyed(true);
			missile->getActive() = false;
			collision = true;
			return true;


		}
	}
	return false;
}

void Blackhole::Render()
{
	if (active)
		sprite_->RenderEx(x_, y_, w_, scaling, scaling);
}

bool Blackhole::HasCollided(Bullets &bullet)
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return collidebox.Intersect(bullet.GetBoundingBox());
}

bool Blackhole::HasCollided(Missile &missile)
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return collidebox.Intersect(missile.GetBoundingBox());
}