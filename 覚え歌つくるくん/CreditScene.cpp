#include "stdafx.h"

CreditScene::CreditScene(const InitData& init)
	: IScene{ init }
{
}

CreditScene::~CreditScene()
{
}

void CreditScene::update()
{
	if (Scene1Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene1");
	}
}

void CreditScene::draw() const {

	font(U"クレジット").draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);

	font(U"制作：仙台高専広瀬キャンパスプログラミング部").drawAt(Vec2{ 960, 150 }, Palette::Black);
	font(U"開発：C++／Siv3D").drawAt(Vec2{ 960, 250 }, Palette::Black);
	font(U"イラスト　　　　　　　　　　　　　　　　").drawAt(Vec2{ 960, 340 }, Palette::Black);
	font(U"・いろいろな人のシルエット（いらすとや）").drawAt(50, Vec2{ 960, 410 }, Palette::Black);
	font(U"・四国めたん立ち絵素材（坂本アヒル）　　").drawAt(50, Vec2{ 960, 480 }, Palette::Black);
	font(U"・ずんだもん立ち絵素材（坂本アヒル）　　").drawAt(50, Vec2{ 960, 550 }, Palette::Black);
	font(U"・春日部つむぎ立ち絵素材（坂本アヒル）　").drawAt(50, Vec2{ 960, 620 }, Palette::Black);
	font(U"歌声合成　　　　　　　　　　　　　　　　").drawAt(Vec2{ 960, 740 }, Palette::Black);
	font(U"・四国めたん（VOICEVOX）　　　　　　　").drawAt(50, Vec2{ 960, 810 }, Palette::Black);
	font(U"・ずんだもん（VOICEVOX）　　　　　　　").drawAt(50, Vec2{ 960, 880 }, Palette::Black);
	font(U"・春日部つむぎ（VOICEVOX）　　　　　　").drawAt(50, Vec2{ 960, 950 }, Palette::Black);


	//zunkyo
	texture.draw(1180, 810);

	// Scene4Button
	Scene1Button.draw(buttonColor);
	font(U"OPに戻る").drawAt(Scene1Button.center(), Palette::White);
}
