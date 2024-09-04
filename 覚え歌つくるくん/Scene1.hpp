#pragma once
#include "stdafx.h"

using App = SceneManager<String>;

class Scene1 : public App::Scene
{
public:
	// コンストラクタとデストラクタ
	Scene1(const InitData& init);
	~Scene1();

	// 更新処理
	void update() override;

	// 描画処理
	void draw() const override;

private:
	// メンバ変数
	const Texture texture{ U"logo4.PNG" };

	// ボタンの矩形と色
	const Rect Scene2Button{ 760, 880, 400, 100 };
	const Rect CreditButton{ 1580, 930, 300, 100 };
	const Rect ExitButton{ 1580, 630, 200, 100 };
	const Rect FullscreenButton{ 1580, 780, 300, 100 };
	//const Rect WindowButton{ 1580, 830, 300, 100 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色
};
