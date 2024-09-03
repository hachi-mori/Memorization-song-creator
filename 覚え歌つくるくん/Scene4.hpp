#pragma once
#include "stdafx.h"

using App = SceneManager<String>;

class Scene4 : public App::Scene
{
public:
	Scene4(const InitData& init);

	void update() override;
	void draw() const override;

private:

	mutable ListBoxState listBoxState;
	Array<String> VoicefileNames;
	String selectedVoiceFile;
	Audio audio1{};
	s3d::Optional<uint64> previousSelectedIndex;

	const Rect Scene1Button{ 0, 900, 200, 100 };
	const Rect Scene2Button{ 0, 230, 350, 150 };
	const Rect Scene3Button{ 0, 430, 350, 150 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色
};
