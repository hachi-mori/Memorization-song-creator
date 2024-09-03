#pragma once
#include "stdafx.h"

class Scene3 : public App::Scene
{
public:
	Scene3(const InitData& init);

	void update() override;
	void draw() const override;

private:
	mutable ListBoxState listBoxState1;
	mutable ListBoxState listBoxState2;
	mutable ListBoxState listBoxState3;

	Array<String> lyricsfileNames;
	Array<String> OriginalScoresfileNames;
	Array<String> speakers;
	Array<int32> speakerIDs;

	bool isloading;
	Rect loadingRect{ 0, 0, 1920, 1080 };
	Texture loadingtexture{ U"loading.png"};
	// 回転速度（ラジアン / 秒）
	const double angularVelocity = 360_deg;
	// 回転角度
	double angle = 0_deg;

	Texture texture{};
	Rect textureRect{ 1540, 390, 280, 460 };
	int Character;
	bool selectListBox3;
	Audio audio1{ U"Character/四国めたん_ららららら.wav" };
	Audio audio2{ U"Character/ずんだもん_ららららら.wav" };
	Audio audio3{ U"Character/春日部つむぎ_ららららら.wav" };

	s3d::Optional<uint64> previousSelectedIndex;

	void InitializeLists();
	bool Button(const Rect& rect, const String& text, bool enabled);

	const Rect Scene1Button{ 0, 900, 200, 100 };
	const Rect Scene2Button{ 0, 230, 350, 150 };
	const Rect Scene4Button{ 0, 630, 350, 150 };
	const Rect SaveButton{ 1570, 950, 300, 80 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色

	// 非同期タスク
	AsyncTask<bool> task;
};
