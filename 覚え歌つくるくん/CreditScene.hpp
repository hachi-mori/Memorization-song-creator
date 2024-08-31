#pragma once
#include "stdafx.h"

using App = SceneManager<String>;

class CreditScene : public App::Scene
{
public:
	// コンストラクタとデストラクタ
	CreditScene(const InitData& init);
	~CreditScene();

	// 更新処理
	void update() override;

	// 描画処理
	void draw() const override;

private:
	// メンバ変数
	const Texture texture{ U"zunkyo.png" };
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	// ボタンの矩形と色
	const Rect Scene1Button{ 1580, 930, 300, 100 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色
};
