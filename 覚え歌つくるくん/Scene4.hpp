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

	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	ListBoxState listBoxState;
	Array<String> VoicefileNames;
	String selectedVoiceFile;
	Audio audio1{};

};
