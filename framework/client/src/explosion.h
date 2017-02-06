#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include <hge.h>
#include <hgerect.h>
#include <memory>
#include <vector>

class hgeSprite;
class hgeRect;
class Ship;

class Explosion
{
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the asteroid
	float x_; //!< The x-ordinate of the asteroid
	float y_; //!< The y-ordinate of the asteroid
	float w_; //!< The angular position of the asteroid
	float scale_;
	float maxScale;
	bool active;
	//hgeRect collidebox; //!< For use in collision detection

public:
	
	Explosion(char* filename);
	~Explosion();
	bool Update(float timedelta);
	void Render();
	void setActive(bool active);
	bool getActive();
	void setPos(float x, float y);
	void setMaxScale(float number)
	{
		maxScale = number;
	}
	//bool HasCollided(Ship &ship);

};

#endif