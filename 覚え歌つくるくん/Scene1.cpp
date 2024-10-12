#include "stdafx.h"
#include "Scene1.hpp"

Scene1::Scene1(const InitData& init)
	: IScene{ init }
	, ScreenName{ U"ウィンドウ" }
	, scale{ 0 }
	, scale2{ 1 }
	, rotationAngle{ 0 }  // 初期回転角度を 0 に設定
	, audioplay{true}
{
}

Scene1::~Scene1()
{
}

void Scene1::update()
{
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2", 0.5s);
	}

	if (CreditButton.mouseOver() && MouseL.down())
	{
		changeScene(U"CreditScene", 0.5s);
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
	if (Scene2Button.mouseOver()
		|| CreditButton.mouseOver()
		|| ExitButton.mouseOver()
		|| FullscreenButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (Scene::Time() > 6 && textureRect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
		scale = 1.1;
		if (MouseL.down())
		{
			changeScene(U"Scene2", 0.5s);
		}
	}
	else if (Scene::Time() > 6) {
		scale = 1;
	}

	// Scene::Time() を使用して経過時間を取得
	double t = Scene::Time();

	if (t <= 6.0)
	{
		// 回転速度（1秒間に1回転）
		const double rotationsPerSecond = 2.0;
		// 回転角度を更新（ラジアン単位）
		rotationAngle = t * 2.0 * Math::Pi * rotationsPerSecond;
	}
	else
	{
		// 回転を停止
		rotationAngle = 0.0;
	}
	if (scale <= 1.0)
	{
		scale += Scene::DeltaTime() * 0.3;  // 速度を調整（0.3は速度の調整係数）
		if (scale > 1)scale = 1;
	}

	if (audioplay&&Scene::Time()<5) {
		audioplay = false;
		audio.play();
	}
}

void Scene1::draw() const
{
	// texture2 を描画
	texture2.resized(texture2.width() * scale2, texture2.height() * scale2)
		.drawAt(Scene::Center().x, Scene::Center().y - 60);
	if (Scene::Time() >= 1.5)
	{
		// texture を回転させて描画
		texture.resized(texture.width() * scale, texture.height() * scale)
			.rotated(rotationAngle)
			.drawAt(Scene::Center().x, Scene::Center().y - 60);
	}

	// ボタンとテキストの描画
	Scene2Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"タップではじめる").drawAt(Scene2Button.center(), Palette::White);

	CreditButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"クレジット").drawAt(CreditButton.center(), Palette::White);

	ExitButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"おわる").drawAt(ExitButton.center(), Palette::White);

	FullscreenButton.draw(buttonColor);
	FontAsset(U"MainFont")(ScreenName).drawAt(FullscreenButton.center(), Palette::White);
}
