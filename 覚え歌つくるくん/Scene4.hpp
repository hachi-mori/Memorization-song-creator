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

	mutable TextAreaEditState textAreaEditState;

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色

	mutable String text;

	const Rect DeleteButton{ 680, 820, 100, 80 };
	const Rect StopButton{ 450, 820, 150, 80 };
	const Rect PlayButton{ 450, 820, 150, 80 };
	const Rect RePlayButton{ 400, 920, 450, 80 };

	mutable Audio audio{ U"" };
	bool playing;
};
