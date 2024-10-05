#include "stdafx.h"
#include <Siv3D.hpp>  // Siv3Dのヘッダーをインクルード

Scene1::Scene1(const InitData& init)
	: IScene{ init }
	, ScreenName{ U"ウィンドウ" },scale{1}, scale2{ 1 }
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
	// ボタンの上にあるマウスカーソルを手のアイコンにする
	if (Scene2Button.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	if (CreditButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	if (ExitButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	if (FullscreenButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	// アニメーション：1秒周期で大きさが変化
	scale = 1 + Periodic::Jump0_1(1s) * 0.1;
	scale2 = 1 + Periodic::Sine0_1(4s) * 0.04;
}

void Scene1::draw() const {
	// 拡大率を反映してテクスチャを描画
	texture2.resized(texture2.width() * scale2, texture2.height() * scale2).drawAt(1920 / 2, 1080 / 2 - 60);
	texture.resized(texture.width() * scale, texture.height() * scale).drawAt(1920 / 2, 1080 / 2 - 60);

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
