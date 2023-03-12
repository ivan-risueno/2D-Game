#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4	// Saltar mas rapido o no
#define JUMP_HEIGHT 64
#define FALL_STEP 3			// Saltar mas rapido o no


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_RIGHT, JUMP_LEFT, FALL_RIGHT, FALL_LEFT
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	bJumping = false;
	spritesheet.loadFromFile("images/AlucardSheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(0.0625, 0.0625), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(8);

	sprite->setAnimationSpeed(STAND_LEFT, 6);
	for (int i = 1; i <= 6; ++i) {
		sprite->addKeyframe(STAND_LEFT, glm::vec2(1 - i * 0.0625f, 0.f));
	}

	sprite->setAnimationSpeed(STAND_RIGHT, 6);
	for (int i = 0; i < 6; ++i) {
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(i * 0.0625f, 0.f));
	}

	sprite->setAnimationSpeed(MOVE_LEFT, 24);
	for (int i = 32; i >= 0; --i) {
		int x = i;
		int y = i < 16 ? 3 : 4;
		y = 4;	// Quitar si encontramos la solucion
		if (i == 16) x = i + 16;
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(x * 0.0625f, y * 0.0625f));
	}

	sprite->setAnimationSpeed(MOVE_RIGHT, 24);
	for (int i = 0; i < 32; ++i) {
		int x = i;
		int y = i < 16 ? 1 : 2;
		y = 2;	// Quitar si encontramos la solucion
		if (i == 16) x = i - 16;
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(i * 0.0625f, y * 0.0625f));
	}

	sprite->setAnimationSpeed(JUMP_LEFT, 8);
	for (int i = 1; i <= 4; ++i) {
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(1 - i * 0.0625f, 5 * 0.0625f));
	}

	sprite->setAnimationSpeed(FALL_LEFT, 18);
	for (int i = 1; i <= 9; ++i) {
		sprite->addKeyframe(FALL_LEFT, glm::vec2(1 - i * 0.0625f, 7 * 0.0625f));
	}
	for (int i = 0; i < 50; ++i) {
		sprite->addKeyframe(FALL_LEFT, glm::vec2(1 - 8 * 0.0625f, 7 * 0.0625f));
	}	// Cutre pero funciona

	sprite->setAnimationSpeed(JUMP_RIGHT, 8);
	for (int i = 0; i < 4; ++i) {
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(i * 0.0625f, 5 * 0.0625f));
	}

	sprite->setAnimationSpeed(FALL_RIGHT, 18);
	for (int i = 0; i < 9; ++i) {
		sprite->addKeyframe(FALL_RIGHT, glm::vec2(i * 0.0625f, 6 * 0.0625f));
	}
	for (int i = 0; i < 50; ++i) {
		sprite->addKeyframe(FALL_RIGHT, glm::vec2(8 * 0.0625f, 6 * 0.0625f));
	}	// Cutre pero funciona

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(64, 64)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if (map->collisionMoveRight(posPlayer, glm::ivec2(64, 64)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle > 90) {
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 64), &posPlayer.y);
				if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT)
					sprite->changeAnimation(FALL_LEFT);
				else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT)
					sprite->changeAnimation(FALL_RIGHT);
			}
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 64), &posPlayer.y))
		{
			if (sprite->animation() == FALL_LEFT)
				sprite->changeAnimation(STAND_LEFT);
			else if (sprite->animation() == FALL_RIGHT)
				sprite->changeAnimation(STAND_RIGHT);
			if (Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
				if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT)
					sprite->changeAnimation(JUMP_LEFT);
				else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT)
					sprite->changeAnimation(JUMP_RIGHT);
			}
		}
		else {
			if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT || sprite->animation() == JUMP_LEFT)
				sprite->changeAnimation(FALL_LEFT);
			else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT || sprite->animation() == JUMP_RIGHT)
				sprite->changeAnimation(FALL_RIGHT);
		}
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




