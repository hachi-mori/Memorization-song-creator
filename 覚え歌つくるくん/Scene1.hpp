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
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	// ボタンの矩形と色
	const Rect Scene2Button{ 0, 400, 350, 150 };
	const Rect Scene3Button{ 0, 600, 350, 150 };
	const Rect Scene4Button{ 0, 800, 350, 150 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色
};
