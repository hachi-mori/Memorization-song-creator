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
	if (Button(Rect{ 0, 200, 350, 150 }, font, U"OP", true))
	{
		changeScene(U"Scene1");
	}

	if (Button(Rect{ 0, 400, 350, 150 }, font, U"語句入力", true))
	{
		changeScene(U"Scene2");
	}

	if (Button(Rect{ 0, 600, 350, 150 }, font, U"曲選択", true))
	{
		changeScene(U"Scene3");
	}

	Rect{ 0, 800, 350, 150 }.draw();

	font(U"動画再生").draw(40, Vec2{ 100, 845 }, ColorF{ 0.3, 0.7, 1.0 });


	// リストボックスを描画し、選択が変更された場合にtrueを返す
	if (SimpleGUI::ListBox(listBoxState, Vec2{ 450, 250 }, 350, 600))
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
	Rect{ 900, 315, 800, 450 }.draw();
	font(U"ファイル選択")
		.draw(30, Rect{ 450, 200, 480, 200 }, Palette::Black);
	font(U"動画")
		.draw(100, Rect{ 1170, 485, 550, 300 }, Palette::Black);

	font(U"動画再生")
		.draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);
}
