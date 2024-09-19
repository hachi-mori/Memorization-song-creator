#include "stdafx.h"

Scene1::Scene1(const InitData& init)
	: IScene{ init }
	, ScreenName{ U"ウィンドウ" }
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

	if (ExitButton.mouseOver() && MouseL.down())
	{
		System::Exit();
	}
	
	if (Window::GetState().fullscreen)
	{
		if (FullscreenButton.mouseOver() && MouseL.down())
		{
				// ウィンドウモードにする
				Window::SetFullscreen(false);
				ScreenName = U"フルスクリーン";
		}
	}
	else
	{
		if (FullscreenButton.mouseOver() && MouseL.down())
		{
				// フルスクリーンモードにする
				Window::SetFullscreen(true);
				ScreenName = U"ウィンドウ";
		}

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

	ExitButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"おわる").drawAt(ExitButton.center(), Palette::White);

	FullscreenButton.draw(buttonColor);
	FontAsset(U"MainFont")(ScreenName).drawAt(FullscreenButton.center(), Palette::White);
}

	

