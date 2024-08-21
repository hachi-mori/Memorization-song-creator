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
	ListBoxState listBoxState1;
	ListBoxState listBoxState2;
	ListBoxState listBoxState3;

	Array<String> lyricsfileNames;
	Array<String> OriginalScoresfileNames;
	Array<String> speakers;
	Array<int32> speakerIDs;

	void InitializeLists();
	bool Button(const Rect& rect, const String& text, bool enabled);
};
