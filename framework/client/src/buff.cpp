#include <math.h>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "buff.h"


Buff::Buff(float x, float y, int bufftype) :
angular_velocity(0),
isTouched(false),
active(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	
	tex_ = hge->Texture_Load("health.png");
	
	tex2_ = hge->Texture_Load("homingmissile.png");
	
	this->bufftype = bufftype;
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_->SetHotSpot(32, 32);
	sprite2_.reset(new hgeSprite(tex2_, 0, 0, 64, 64));
	sprite2_->SetHotSpot(32, 32);
	x_ = x;
	y_ = y;

}

void Buff::initialise(float x, float y, int buffType)
{
	x_ = x;
	y_ = y;
	isTouched = false;
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
	if (bufftype == 1)
	{
		tex_ = hge->Texture_Load("health.png");
	}
	else
	{
		tex_ = hge->Texture_Load("homingmissile.png");
	}
	bufftype = buffType;
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 64, 64));
	sprite_->SetHotSpot(32, 32);
}

Buff::~Buff()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Texture_Free(tex2_);
	hge->Release();
}

bool Buff::Update(std::vector<Ship*> &shiplist, float timedelta)
{
	
	for (std::vector<Ship*>::iterator thisship = shiplist.begin();
		thisship != shiplist.end(); thisship++)
	{
		if (HasCollided((*(*thisship))) && (*thisship)->getActive())
		{
			// if both are stuck
			if (thisship == shiplist.begin())
			{
				if (bufftype == 1 && thisship == shiplist.begin())
					(*thisship)->getHealth() += 50;
				else if (bufftype == 0 && thisship == shiplist.begin())
				{
					(*thisship)->setHaveMissile(true);
				}
			}
			isTouched = true;
			active = false;
			return true;
		}
	}


	return false;
}

void Buff::Render()
{
	if (active)
	{
		if (bufftype == 1)
		{
			sprite_->RenderEx(x_, y_, 0);
		}
		else
		{
			sprite2_->RenderEx(x_, y_, 0);
		}
		
	}
		
}

bool Buff::HasCollided(Ship &ship)
{
	sprite_->GetBoundingBox(x_, y_, &collidebox);

	return collidebox.Intersect(ship.GetBoundingBox());
}