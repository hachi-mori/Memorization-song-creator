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

	if (CreditButton.mouseOver() && MouseL.down())
	{
		changeScene(U"CreditScene");
	}
}

void Scene1::draw() const {
	//logo
	texture.draw(560,50);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"タップではじめる").drawAt(Scene2Button.center(), Palette::White);

	// CreditButton
	CreditButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"クレジット").drawAt(CreditButton.center(), Palette::White);
}
