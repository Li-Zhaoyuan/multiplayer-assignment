#ifndef _Timebomb_H_
#define _Timebomb_H_

#include <memory>
#include <vector>

#include "hge.h"
#include "hgerect.h"
#include "textbox.h"
#include "explosion.h"

class hgeSprite;
class hgeRect;
class Bullets;
class Ship;

class Timebomb
{
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	TextBox* timeLeftText;
	float x_; //!< The x-ordinate of the ship
	float y_; //!< The y-ordinate of the ship
	float w_; //!< The angular position of the ship

	hgeRect collidebox;

	bool active;
	bool collision;
	bool doExplosion;
	float scaling;
	float timeleft;

public:
	float angular_velocity;
	Timebomb(char* filename, float x, float y, float w);
	~Timebomb();
	bool Update(Ship* &ship, float timedelta, bool isLocal);
	void Render();
	bool HasCollided(Ship &ship);

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
	void setTimeLeft(float time)
	{
		timeleft = time;
	}
	float getTimeLeft()
	{
		return timeleft;
	}

	bool getDoExplosion()
	{
		return doExplosion;
	}

	void setDoExplosion(bool toggle)
	{
		doExplosion = toggle;
	}
};

#endif