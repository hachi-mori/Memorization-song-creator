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
	if (Button(Rect{ 0, 800, 350, 150 }, font, U"動画再生", true))
	{
		changeScene(U"Scene4");
	}

	if (Button(Rect{ 0, 400, 350, 150 }, font, U"語句入力", true))
	{
		changeScene(U"Scene2");
	}

	if (Button(Rect{ 0, 600, 350, 150 }, font, U"曲選択", true))
	{
		changeScene(U"Scene3");
	}

	Rect{ 0, 200, 350, 150 }.draw();

	font(U"OP").draw(40, Vec2{ 150, 245 }, ColorF{ 0.3, 0.7, 1.0 });

	font(U"オープニング")
		.draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);
	texture.draw(760, 130);
}


