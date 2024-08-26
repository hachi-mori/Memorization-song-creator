#include "stdafx.h"

Scene1::Scene1(const InitData& init)
	: IScene{ init }
{
}

Scene1::~Scene1()
{
}

void Scene1::update()
{
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2");
	}

	if (Scene3Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene3");
	}

	if (Scene4Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene4");
	}
}

void Scene1::draw() const {
	// OP
	Rect{ 0, 200, 350, 150 }.draw();
	font(U"OP").draw(40, Vec2{ 150, 245 }, ColorF{ buttonColor });

	font(U"オープニング").draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);

	//logo
	texture.draw(760, 130);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	font(U"語句入力").drawAt(Scene2Button.center(), Palette::White);

	// Scene3Button
	Scene3Button.draw(buttonColor);
	font(U"曲選択").drawAt(Scene3Button.center(), Palette::White);

	// Scene4Button
	Scene4Button.draw(buttonColor);
	font(U"動画再生").drawAt(Scene4Button.center(), Palette::White);
}

void Scene1::draw() const
{
}
