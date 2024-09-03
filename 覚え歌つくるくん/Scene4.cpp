#include "stdafx.h"

Scene4::Scene4(const InitData& init)
	: IScene{ init }, audio1{ U"" },previousSelectedIndex{ s3d::none }
{
	// Load Voice file names
	for (const auto& path : FileSystem::DirectoryContents(U"Voice"))
	{
		VoicefileNames << FileSystem::BaseName(path);
	}
	listBoxState = ListBoxState(VoicefileNames);
}

void Scene4::update()
{
	// ボタンの処理
	if (Scene1Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene1");
	}
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2");
	}
	if (Scene3Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene3");
	}

	// リストボックスの選択が変更された場合にtrueを返す
	if (listBoxState.selectedItemIndex != previousSelectedIndex)
	{
		previousSelectedIndex = listBoxState.selectedItemIndex;
		// 選択が変更された場合の処理
		if (listBoxState.selectedItemIndex.has_value())
		{
			// 選択されたアイテムの名前を取得
			selectedVoiceFile = listBoxState.items[listBoxState.selectedItemIndex.value()];
			selectedVoiceFile = U"Voice/" + selectedVoiceFile + U".wav";
			// ここに選択されたときの処理を追加
			audio1 = Audio{ selectedVoiceFile }; // オーディオを新しいファイルで初期化
			audio1.play();
		}
	}
}

void Scene4::draw() const
{
	//動画再生
	Rect{ 0, 630, 350, 150 }.draw();
	FontAsset(U"MainFont")(U"ながす").draw(48, Vec2{ 100, 675 }, buttonColor);

	Rect{ 900, 315, 800, 450 }.draw();
	FontAsset(U"MainFont")(U"おぼえうた").draw(30, Rect{ 450, 200, 480, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"動画").draw(100, Rect{ 1180, 480, 550, 300 }, Palette::Black);
	FontAsset(U"MainFont")(U"ながす").draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);

	//Scene1Button
	Scene1Button.draw(Palette::Gray);
	FontAsset(U"MainFont")(U"おわりにする").drawAt(30, Vec2{ Scene1Button.center() }, Palette::White);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"かく").drawAt(Scene2Button.center(), Palette::White);

	// Scene3Button
	Scene3Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"つくる").drawAt(Scene3Button.center(), Palette::White);

	SimpleGUI::ListBox(listBoxState, Vec2{ 450, 250 }, 350, 600);
}
