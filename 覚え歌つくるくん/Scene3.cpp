#include "Scene3.hpp"

Scene3::Scene3(const InitData& init)
	: IScene{ init },
	texture{ U"Character/シルエット.png" },
	textureRect{ 1540, 390, 280, 460 },
	selectListBox3{ false },
	previousSelectedIndex{ s3d::none }
{
	// Initialize the list of files and speakers
	InitializeLists();
}

void Scene3::InitializeLists()
{
	// Load lyrics file names
	for (const auto& path : FileSystem::DirectoryContents(U"lyrics"))
	{
		lyricsfileNames << FileSystem::BaseName(path);
	}
	listBoxState1 = ListBoxState(lyricsfileNames);

	// Load OriginalScores file names
	for (const auto& path : FileSystem::DirectoryContents(U"OriginalScores"))
	{
		OriginalScoresfileNames << FileSystem::BaseName(path);
	}
	listBoxState2 = ListBoxState(OriginalScoresfileNames);

	// Load character speaker information
	for (const auto& speaker : VOICEVOX::GetSpeakers())
	{
		for (const auto& style : speaker.styles)
		{
			if (style.id == 2 || style.id == 3 || style.id == 8) {
				speakers << U"{}"_fmt(speaker.name);
				speakerIDs << style.id;
			}
		}
	}
	listBoxState3 = ListBoxState{ speakers };
}

bool Scene3::Button(const Rect& rect, const String& text, bool enabled)
{
	if (enabled && rect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (enabled)
	{
		rect.draw(ColorF{ 0.3, 0.7, 1.0 });
		font(text).drawAt(rect.center(), Palette::White);
	}
	else
	{
		rect.draw(ColorF{ 0.5 });
		font(text).drawAt(rect.center(), ColorF{ 0.7 });
	}

	return (enabled && rect.leftClicked());
}

void Scene3::update()
{
	if (Scene1Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene1");
	}
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2");
	}
	if (Scene4Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene4");
	}

	if (SaveButton.mouseOver() && MouseL.down())
	{
		Optional<int32> selectedSpeakerID;
		if (listBoxState3.selectedItemIndex)
		{
			selectedSpeakerID = speakerIDs[*listBoxState3.selectedItemIndex] + 3000;
		}

		String selectedlyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
		String selectedScoreName = OriginalScoresfileNames[listBoxState2.selectedItemIndex.value()];
		String selectedCharacterName = speakers[listBoxState3.selectedItemIndex.value()];

		const FilePath lyricsFilePath = U"lyrics/{}.csv"_fmt(selectedlyricsName);
		const FilePath scoreFilePath = U"OriginalScores/{}.json"_fmt(selectedScoreName);
		const FilePath outputAudioFilePath = U"Voice/{}-{}-{}.wav"_fmt(selectedlyricsName, selectedScoreName, selectedCharacterName);
		const FilePath createscoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedlyricsName, selectedScoreName);

		// JSONファイルを更新
		UpdateJSONFromCSV(lyricsFilePath, scoreFilePath, createscoreFilePath);

		if (VOICEVOX::SynthesizeVoiceFromScore(createscoreFilePath, outputAudioFilePath, *selectedSpeakerID))
		{
			Print(U"音声合成が成功しました。");
			const Audio audio{ outputAudioFilePath };
			audio.play();
		}
		else
		{
			Print(U"音声合成に失敗しました。");
		}
	}

	if (listBoxState3.selectedItemIndex != previousSelectedIndex)
	{
		selectListBox3 = true;
		previousSelectedIndex = listBoxState3.selectedItemIndex;
			Character = speakerIDs[*listBoxState3.selectedItemIndex];
			switch (Character) {
			case 2:
				texture = Texture{ U"Character/四国めたん_1.png" };
				break;
			case 3:
				texture = Texture{ U"Character/ずんだもん_1.png" };
				break;
			case 8:
				texture = Texture{ U"Character/春日部つむぎ_1.png" };
				break;
			default:
				texture = Texture{ U"Character/シルエット.png" };
				break;
			}

	}
	if (selectListBox3 && textureRect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}
	if (selectListBox3 && textureRect.leftClicked())
	{
		Character = speakerIDs[*listBoxState3.selectedItemIndex];
		switch (Character) {
		case 2:
			texture = Texture{ U"Character/四国めたん_2.png" };
			audio1.play();
			break;
		case 3:
			texture = Texture{ U"Character/ずんだもん_2.png" };
			audio2.play();
			break;
		case 8:
			texture = Texture{ U"Character/春日部つむぎ_2.png" };
			audio3.play();
			break;
		default:
			texture = Texture{ U"Character/シルエット.png" };
			break;
		}
	}

	font(U"曲設定").draw(70, Vec2{ 20, 20 }, Palette::White);
	font(U"ファイル選択").draw(30, Vec2{ 500, 200 }, Palette::Black);
	font(U"曲選択").draw(30, Vec2{ 1010, 200 }, Palette::Black);
	font(U"キャラクター選択").draw(30, Vec2{ 1520, 200 }, Palette::Black);
	textureRect.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 });
	texture.draw(1520, 380);
}

void Scene3::draw() const
{
	//リストボックスを描画
	SimpleGUI::ListBox(listBoxState1, Vec2{ 500, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState2, Vec2{ 1010, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState3, Vec2{ 1520, 250 }, 300, 100);

	// 曲設定
	Rect{ 0, 600, 350, 150 }.draw();
	font(U"曲設定").draw(40, Vec2{ 110, 645 }, ColorF{ 0.3, 0.7, 1.0 });

	font(U"曲設定").draw(70, Vec2{ 20, 20 }, Palette::White);

	font(U"ファイル選択").draw(30, Vec2{ 500, 200 }, Palette::Black);
	font(U"曲選択").draw(30, Vec2{ 1010, 200 }, Palette::Black);
	font(U"キャラクター選択").draw(30, Vec2{ 1520, 200 }, Palette::Black);

	// Charactertexture
	textureRect.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 });
	texture.draw(1520, 380);

	//Scene1Button
	Scene1Button.draw(buttonColor);
	font(U"OP").drawAt(Vec2{ Scene1Button.center() }, Palette::White);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	font(U"語句入力").drawAt(Scene2Button.center(), Palette::White);

	//Scene4Button
	Scene4Button.draw(buttonColor);
	font(U"動画再生").drawAt(Vec2{ Scene4Button.center() }, Palette::White);

	//SaveButton
	SaveButton.draw(buttonColor);
	font(U"保存").drawAt(Vec2{ SaveButton.center() }, Palette::White);
}
