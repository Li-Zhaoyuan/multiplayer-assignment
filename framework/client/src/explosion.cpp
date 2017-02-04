#include <math.h>
#include <iostream>

#include "hge.h"
#include "hgeSprite.h"

#include "ship.h"
#include "Explosion.h"

Explosion::Explosion(char* filename) :
x_(0),
y_(0),
w_(0),
scale_(0.01f),
active(false)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 40));
	sprite_->SetHotSpot(20, 20);

}

Explosion::~Explosion()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

bool Explosion::Update(float timedelta)
{
	if (scale_ < 2.f)
	{
		scale_ += 5 * timedelta;
	}
	else
	{
		active = false;
	}
	if (!active)
	{
		scale_ = 0.01f;
	}
	return true;
}

void Explosion::Render()
{
	sprite_->RenderEx(x_, y_, w_, scale_, scale_);
}
void Explosion::setActive(bool active)
{
	this->active = active;
}
bool Explosion::getActive()
{
	return active;
}
void Explosion::setPos(float x, float y)
{
	x_ = x;
	y_ = y;
}