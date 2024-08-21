#include "stdafx.h"

Scene4::Scene4(const InitData& init)
	: IScene{ init }, audio1{ U"" }
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
	if (Button(Rect{ 870, 950, 300, 80 }, font, U"OPに戻る", true))
	{
		changeScene(U"Scene1");
	}

	if (Button(Rect{ 1220, 950, 300, 80 }, font, U"語句入力に戻る", true))
	{
		changeScene(U"Scene2");
	}

	if (Button(Rect{ 1570, 950, 300, 80 }, font, U"曲選択に戻る", true))
	{
		changeScene(U"Scene3");
	}

	// リストボックスを描画し、選択が変更された場合にtrueを返す
	if (SimpleGUI::ListBox(listBoxState, Vec2{ 80, 250 }, 500, 600))
	{
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
	Rect{ 700, 200, 1000, 600 }.draw();

	font(U"動画")
		.draw(100, Rect{ 1100, 435, 480, 200 }, Palette::Black);

	font(U"動画再生")
		.draw(50, Rect{ 20, 20, 480, 200 }, Palette::Black);
}
