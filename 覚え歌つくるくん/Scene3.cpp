#include "Scene3.hpp"

Scene3::Scene3(const InitData& init)
	: IScene{ init }
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
			speakers << U"{}（{}）"_fmt(speaker.name, style.name);
			speakerIDs << style.id;
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
	if (Button(Rect{ 50, 950, 300, 80 }, U"OPに戻る", true))
	{
		changeScene(U"Scene1");
	}

	if (Button(Rect{ 400, 950, 300, 80 }, U"語句入力に戻る", true))
	{
		changeScene(U"Scene2");
	}

	if (Button(Rect{ 1570, 950, 300, 80 }, U"動画再生", true))
	{
		changeScene(U"Scene4");
	}

	if (Button(Rect{ 1220, 950, 300, 80 }, U"保存", true))
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

		if (s3d::VOICEVOX::SynthesizeVoiceFromScore(createscoreFilePath, outputAudioFilePath, *selectedSpeakerID))
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

	SimpleGUI::ListBox(listBoxState1, Vec2{ 100, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState2, Vec2{ 470, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState3, Vec2{ 1520, 250 }, 300, 600);
}

void Scene3::draw() const
{
	font(U"曲設定").draw(50, Vec2{ 20, 20 }, Palette::Black);
	font(U"ファイル選択").draw(30, Vec2{ 100, 200 }, Palette::Black);
	font(U"曲選択").draw(30, Vec2{ 470, 200 }, Palette::Black);
	font(U"キャラクター選択").draw(30, Vec2{ 1520, 200 }, Palette::Black);
}
