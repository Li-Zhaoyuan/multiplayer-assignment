#include <math.h>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "timebomb.h"
#include "missile.h"
#include "bullets.h"


Timebomb::Timebomb(char* filename, float x, float y, float w) :
angular_velocity(0),
scaling(0.1f),
timeLeftText(0),
timeleft(0.f),
active(false),
collision(false),
doExplosion(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 128, 128));
	sprite_->SetHotSpot(64, 64);
	x_ = x;
	y_ = y;
	w_ = w;

	timeLeftText = new TextBox("font1.fnt");
	timeLeftText->SetPos(x_, y_);
	timeLeftText->mytext_ = std::to_string(timeleft);
	
}

Timebomb::~Timebomb()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
	delete timeLeftText;
	timeLeftText = 0;
}

bool Timebomb::Update(Ship* &ship, float timedelta, bool isLocal)
{
	if (active && isLocal)
	{
		timeleft -= timedelta;
		timeLeftText->SetPos(x_, y_);
		if (timeleft > 0)
		{
			timeLeftText->mytext_ = std::to_string(timeleft);
		}
		else
		{
			//timeleft = 10.f;
			active = false;
			doExplosion = true;
		}
		
		if (HasCollided((*(ship))) && (ship)->getActive() && timeleft < 0)//make sure the ship collided is active
		{
			// if both are stuck	
			ship->getHealth() -= 50;
			collision = true;
			
			return true;


		}
	}
	else if (active && !isLocal)
	{
		timeleft -= timedelta;
		timeLeftText->SetPos(x_, y_);
		if (timeleft > 0)
		{
			timeLeftText->mytext_ = std::to_string(timeleft);
		}
		else
		{
			//timeleft = 10.f;
			active = false;
			doExplosion = true;
		}
	}
	return false;
}



void Timebomb::Render()
{
	if (active)
	{
		sprite_->RenderEx(x_ + 20, y_, w_);
		timeLeftText->Render();
	}
}

bool Timebomb::HasCollided(Ship &ship)
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return collidebox.Intersect(ship.GetBoundingBox());
}

