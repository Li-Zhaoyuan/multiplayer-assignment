#ifndef _BUFF_H_
#define _BUFF_H_

#include <memory>
#include <vector>

#include "hge.h"
#include "hgerect.h"

class hgeSprite;
class hgeRect;
class Ship;

class Buff
{
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	HTEXTURE tex2_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite2_; //!< The sprite used to display the ship
	float x_; //!< The x-ordinate of the ship
	float y_; //!< The y-ordinate of the ship
	
	hgeRect collidebox;
	int bufftype;
	bool isTouched;
	bool active;

public:
	float angular_velocity;
	Buff(float x, float y, int bufftype);
	~Buff();
	bool Update(std::vector<Ship*> &shiplist, float timedelta);
	void Render();
	bool HasCollided(Ship &ship);

	void UpdateLoc(float x, float y)
	{
		x_ = x;
		y_ = y;
	}

	float GetX() const
	{
		return x_;
	}

	float GetY() const
	{
		return y_;
	}

	bool GetIsTouched()
	{
		return isTouched;
	}

	void SetIsTouched(bool toggle)
	{
		isTouched = toggle;
	}

	bool &getActive()
	{
		return active;
	}

	void initialise(float x, float y, int buffType);
};

#endif