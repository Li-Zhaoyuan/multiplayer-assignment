#ifndef _BULLETS_H_
#define _BULLETS_H_

#include <memory>
#include <vector>

#include "hge.h"
#include "hgerect.h"

class hgeSprite;
class hgeRect;
class Ship;

class Bullets
{
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship
	float x_; //!< The x-ordinate of the ship
	float y_; //!< The y-ordinate of the ship
	float w_; //!< The angular position of the ship
	float velocity_x_; //!< The resolved velocity of the ship along the x-axis
	float velocity_y_; //!< The resolved velocity of the ship along the y-axis
	hgeRect collidebox;
	int ownerid;

	bool isDestroyed;
	bool isDespawned;
	bool active;

public:
	float angular_velocity;
	Bullets(char* filename, float x, float y, float w, int shipid);
	~Bullets();
	bool Update(std::vector<Ship*> &shiplist, float timedelta);
	void Render();
	bool HasCollided(Ship &ship);

	void UpdateLoc(float x, float y, float w)
	{
		x_ = x;
		y_ = y;
		w_ = w;
	}

	int GetOwnerID()
	{
		return ownerid;
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

	bool GetIsDestroyed()
	{
		return isDestroyed;
	}

	bool GetIsDespawned()
	{
		return isDespawned;
	}

	void SetIsDestroyed(bool toggle)
	{
		isDestroyed = toggle;
	}

	void SetIsDesapwn(bool toggle)
	{
		isDespawned = toggle;
	}

	bool &getActive()
	{
		return active;
	}

	void initialise(float x, float y, float w, int id)
	{
		x_ = x;
		y_ = y;
		w_ = w;
		ownerid = id;
		velocity_x_ = 200.0f * cosf(w_);
		velocity_y_ = 200.0f * sinf(w_);
	}
};

#endif