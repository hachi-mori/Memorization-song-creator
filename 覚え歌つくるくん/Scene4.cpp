#include "stdafx.h"

Scene4::Scene4(const InitData& init)
	: IScene{ init }, previousSelectedIndex{ s3d::none }, text{ U"" },audio{U""},playing{false}
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
			
			// ハイフンで文字列を分割する
			Array<String> parts = selectedVoiceFile.split(U'-');

			// 歌詞を取得
			String firstElement = parts[0];
			TextReader reader(U"lyrics/" + firstElement + U".csv");
			text = reader.readAll();

			audio = Audio{ Audio::Stream,  U"Voice/" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U".wav",Loop::Yes };
		}
	}

	// ファイルの再生
	if (PlayButton.mouseOver() && MouseL.down())
	{
		if(listBoxState.selectedItemIndex){
			// オーディオを新しいファイルで初期化
			if (!playing) {
				audio.play();
				playing = { true };
			}
			else {
				audio.pause();
				playing = { false };
			}
		}
		else {
			System::MessageBoxOK(U"", U"おぼえうたをえらんでください");
		}		
	}

	if (RePlayButton.mouseOver() && MouseL.down())
	{
		if (listBoxState.selectedItemIndex) {
			audio.stop();
			audio.play();
		}
		else {
			System::MessageBoxOK(U"", U"おぼえうたをえらんでください");
		}
	}

	// ファイルの削除
	if (DeleteButton.mouseOver() && MouseL.down() && listBoxState.selectedItemIndex)
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"", U"「" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U"」\nこのファイルをけしますか？");

		// OK が選ばれたら
		if (result == MessageBoxResult::OK)
		{
			FileSystem::Remove(U"Voice/" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U".wav");
			previousSelectedIndex = { s3d::none };
		}
	}

	//カーソル
	if (Scene1Button.mouseOver()|| Scene2Button.mouseOver()|| Scene3Button.mouseOver()|| DeleteButton.mouseOver()|| PlayButton.mouseOver()|| RePlayButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}
}

void Scene4::draw() const
{
	//動画再生
	Rect{ 0, 630, 350, 150 }.draw();
	FontAsset(U"MainFont")(U"ながす").draw(48, Vec2{ 100, 675 }, buttonColor);

	//Rect{ 900, 315, 800, 450 }.draw();
	FontAsset(U"MainFont")(U"おぼえうた").draw(30, Rect{ 450, 150, 480, 200 }, Palette::Black);
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

	SimpleGUI::ListBox(listBoxState, Vec2{ 385, 200 }, 480, 600);
	FontAsset(U"MainFont")(text).draw(30, Vec2{ 1000, 220 }, Palette::Black);

	//DeleteButton
	DeleteButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"けす").drawAt(Vec2{ DeleteButton.center() }, Palette::White);

	if (!playing) {
		//PlayButton
		PlayButton.draw(buttonColor);
		FontAsset(U"MainFont")(U"ながす").drawAt(Vec2{ PlayButton.center() }, Palette::White);
	}else{
		System::Sleep(10);
		StopButton.draw(buttonColor);
		FontAsset(U"MainFont")(U"とめる").drawAt(Vec2{ StopButton.center() }, Palette::White);
	}

	//RePlayButton
	RePlayButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"さいしょからながす").drawAt(Vec2{ RePlayButton.center()}, Palette::White);
}
