#ifndef _BLACKHOLE_H_
#define _BLACKHOLE_H_

#include <memory>
#include <vector>

#include "hge.h"
#include "hgerect.h"

class hgeSprite;
class hgeRect;
class Bullets;
class Missile;

class Blackhole
{
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	float x_; //!< The x-ordinate of the ship
	float y_; //!< The y-ordinate of the ship
	float w_; //!< The angular position of the ship

	hgeRect collidebox;

	bool active;
	bool collision;
	float scaling;

public:
	float angular_velocity;
	Blackhole(char* filename, float x, float y, float w);
	~Blackhole();
	bool Update(Bullets* &bulletList, float timedelta, bool isEnemy);
	bool Update(Missile* &missile, float timedelta, bool isEnemy);
	bool Update(float timedelta);
	void Render();
	bool HasCollided(Bullets &bullet);
	bool HasCollided(Missile &missile);

	void UpdateLoc(float x, float y, float w)
	{
		x_ = x;
		y_ = y;
		w_ = w;
	}


	float GetX() const
	{
		return x_;
	}

	float GetY() const
	{
		return y_;
	}

	float GetW() const
	{
		return w_;
	}


	bool &getActive()
	{
		return active;
	}

	void initialise(float x, float y, float w)
	{
		x_ = x;
		y_ = y;
		w_ = w;
	}
	void setCollision(bool toggle)
	{
		collision = toggle;
	}

	bool getCollision()
	{
		return collision;
	}
};

#endif