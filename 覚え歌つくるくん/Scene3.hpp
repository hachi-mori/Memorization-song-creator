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

	s3d::Optional<uint64> previousSelectedIndex1;
	s3d::Optional<uint64> previousSelectedIndex2;
	s3d::Optional<uint64> previousSelectedIndex3;

	int Difference;	//仮の類似度（正しくは類似してない度）

	void InitializeLists();

	const Rect Scene1Button{ 0, 900, 200, 100 };
	const Rect Scene2Button{ 0, 230, 350, 150 };
	const Rect Scene4Button{ 0, 630, 350, 150 };
	const Rect SaveButton{ 1570, 950, 300, 80 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色

	// 非同期タスク
	AsyncTask<bool> task;

	//円グラフ
	Array<double> ToRatios(const Array<double>& values)
	{
		const double sum = values.sumF();

		Array<double> ratios;

		for (const auto& value : values)
		{
			ratios << (value / sum);

		}
		return ratios;
	}

	Array<double> CumulativeSum(const Array<double>& values)
	{
		Array<double> sums = { 0.0 };

		for (const auto& value : values)
		{
			sums << (sums.back() + value);
		}

		return sums;
	}

	// ラベル
	const Array<String> labels = { U"一致率", U""};

	// 数値
	Array<double> values = { 60, 100 - 60 };
	int a = 80;
	int b;
	const Texture emoji1{ U"😄"_emoji };

	const Texture emoji2{ U"😐"_emoji };

	const Texture emoji3{ U"😟"_emoji };


	// 円グラフで占める割合
	const Array<double> ratios = ToRatios(values);

	// 円グラフの開始位置（割合）
	const Array<double> starts = CumulativeSum(ratios);

	const Circle circle{1300,500, 160.0 };
};
