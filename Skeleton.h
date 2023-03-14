#ifndef _SKELETON_INCLUDE
#define _SKELETON_INCLUDE


#include "Sprite.h"
#include "TileMap.h"




class Skeleton : public Enemy
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, bool right, int stpr, int stpl );
	void update(int deltaTime);

private:
	int stepsRight;
	int stepsLeft;

};


#endif // _SKELETON_INCLUDE