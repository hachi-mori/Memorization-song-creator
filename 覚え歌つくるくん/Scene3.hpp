#pragma once
#include <Siv3D.hpp>
#include "VoiceVoxSynthesizer.hpp"

class Scene3 : public App::Scene
{
public:
	Scene3(const InitData& init);

	void update() override;
	void draw() const override;

private:
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	mutable ListBoxState listBoxState1;
	mutable ListBoxState listBoxState2;
	mutable ListBoxState listBoxState3;

	Array<String> lyricsfileNames;
	Array<String> OriginalScoresfileNames;
	Array<String> speakers;
	Array<int32> speakerIDs;

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

	const Rect Scene1Button{ 0, 200, 350, 150 };
	const Rect Scene2Button{ 0, 400, 350, 150 };
	const Rect Scene4Button{ 0, 800, 350, 150 };
	const Rect SaveButton{ 1570, 950, 300, 80 };

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色
};
