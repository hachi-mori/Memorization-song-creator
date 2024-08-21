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
	if (Button(Rect{ 335, 950, 300, 80 }, font, U"既存曲の曲設定", true))
	{
		changeScene(U"Scene3");
	}

	if (Button(Rect{ 785, 950, 300, 80 }, font, U"動画再生", true))
	{
		changeScene(U"Scene4");
	}

	if (Button(Rect{ 1235, 950, 300, 80 }, font, U"語句入力", true))
	{
		changeScene(U"Scene2");
	}
}

void Scene1::draw() const
{
	texture.draw(560, 70);
}
