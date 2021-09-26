/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "Brick.h"
#include "SpriteCodex.h"

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	ball(Vec2(300.0f, 300.0f), Vec2(-1.0f, -1.0f)),
	walls(0.0f, float(gfx.ScreenWidth), 0.0f, float(gfx.ScreenHeight)),
	soundPad(L"Sounds\\arkpad.wav"),
	soundBrick(L"sounds\\arkbrick.wav"),	
	pad(Vec2(400.0f, 500.0f), 50.0f, 10.0f)
{
	const Color colors[4] = { Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan };
	const Vec2 topLeft(40.0f, 40.0f);
	int i = 0;
	for (int y = 0; y < nBricksDown; y++) {
		const Color c = colors[y];
		for (int x = 0; x < nBricksAcross; x++) {
			bricks[i] = Brick(RectF(
				topLeft + Vec2(x*brickWidth, y * brickHeight),
				brickWidth, brickHeight), c);
			i++;
		}
	}
}

void Game::Go()
{
	gfx.BeginFrame();	
	float elapsedTime = ft.Mark(); //the time passed between frames
	while (elapsedTime > 0.0f) {
		const float dt = std::min(0.0025f, elapsedTime); //2.5 milliseconds
		UpdateModel(dt);
		elapsedTime -= dt;
	}
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel(float dt)
{
	if (gameIsOver) {
		return;
	}
	pad.Update(wnd.kbd, dt);
	pad.IsWallCollision(walls);

	ball.Update(dt);
	
	bool collisionHappened = false;
	float currColDistSq;
	int currCollisionIndex; //index for the brick of the current collision
	for (int i = 0; i < nBricks; i++) {
		if (bricks[i].CheckBallCollision(ball))
		{
			const float newColDistSq = (ball.GetPosition() - bricks[i].GetCenter()).GetLengthSq();
			if (collisionHappened) //if it's not the first collision happened
			{
				if (newColDistSq < currColDistSq) {
					currColDistSq = newColDistSq;
					currCollisionIndex = i;
					
				}
			}
			else
			{
				currColDistSq = newColDistSq;
				currCollisionIndex = i;
				collisionHappened = true;
			}
		}
	}
	if (collisionHappened) {
		pad.ResetCooldown();
		bricks[currCollisionIndex].ExecuteBallCollision(ball);
		soundBrick.Play();
	}

	if (pad.IsBallCollision(ball)) {
		soundPad.Play();
	}
	const int ballWallCollResult = ball.IsWallCollision(walls);
	if (ballWallCollResult == 1) {
		pad.ResetCooldown();
		soundPad.Play();
	}
	else if (ballWallCollResult == 2) {
		gameIsOver = true;
	}
}

void Game::ComposeFrame()
{
	if (!gameIsOver) {
		ball.Draw(gfx);

	}
	else {
		SpriteCodex::DrawGameOver(Vec2(400.0f, 300.0f), gfx);
	}
	for (const Brick& b : bricks) {
		b.Draw(gfx);
		pad.Draw(gfx);

	}

}
