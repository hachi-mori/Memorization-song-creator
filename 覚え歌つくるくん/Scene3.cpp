#include "stdafx.h"

Scene3::Scene3(const InitData& init)
	: IScene{ init },
	texture{ U"Character/シルエット.png" },
	textureRect{ 1540, 390, 280, 460 },
	selectListBox3{ false },
	previousSelectedIndex1{ s3d::none },
	previousSelectedIndex2{ s3d::none },
	previousSelectedIndex3{ s3d::none },
	Difference{0},
	isloading{ false }
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

	if (SaveButton.mouseOver() && MouseL.down() && !task.isValid())
	{
		if (listBoxState1.selectedItemIndex && listBoxState2.selectedItemIndex && listBoxState3.selectedItemIndex) {
			isloading = true;

			// 非同期タスクを開始
			task = Async([this]()
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

				// 音声合成
				return VOICEVOX::SynthesizeVoiceFromScore(createscoreFilePath, outputAudioFilePath, *selectedSpeakerID);
			});
		}
		else {
			System::MessageBoxOK(U"", U"「かし」と「きょく」と「キャラクター」をえらんでからほぞんしてください");
		}
	}

	// 非同期タスクが完了したかをチェック
	if (task.isReady())
	{
		// 結果を取得
		bool synthesisSuccess = task.get();
		if (synthesisSuccess)
		{
			Print(U"音声合成が成功しました。");
		}
		else
		{
			Print(U"音声合成に失敗しました。");
		}
		isloading = false;
	}

	if (isloading)
	{
		angle += (Scene::DeltaTime() * angularVelocity);
	}

	if (listBoxState1.selectedItemIndex != previousSelectedIndex1)
	{
		previousSelectedIndex1 = listBoxState1.selectedItemIndex;
		String selectedlyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
		const FilePath lyricsFilePath = U"lyrics/{}.csv"_fmt(selectedlyricsName);

		for (size_t i = 0; i < OriginalScoresfileNames.size(); ++i)
		{
			String selectedScoreName = OriginalScoresfileNames[i];
			const FilePath scoreFilePath = U"OriginalScores/{}.json"_fmt(selectedScoreName);
			const FilePath createscoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedlyricsName, selectedScoreName);

			// JSONファイルを更新
			UpdateJSONFromCSV(lyricsFilePath, scoreFilePath, createscoreFilePath);
		}
	}

	if (listBoxState2.selectedItemIndex != previousSelectedIndex2)
	{
		previousSelectedIndex2 = listBoxState2.selectedItemIndex;
		String selectedLyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
		String selectedScoreName = OriginalScoresfileNames[listBoxState2.selectedItemIndex.value()];
		const FilePath createScoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedLyricsName, selectedScoreName);

		JSON json = JSON::Load(createScoreFilePath);

		Print << (U"CreatedScores/" + createScoreFilePath);

		// 値を文字列として取得
		String typeValue = json[U"__type"].getString();

		// "Difference:" プレフィックスを取り除く
		String differenceStr = typeValue.substr(11); // プレフィックスの長さは10

		// 数値に変換
		Difference = Parse<int>(differenceStr);
		Print << U"Difference value: " << Difference;
	}

	if (listBoxState3.selectedItemIndex != previousSelectedIndex3)
	{
		selectListBox3 = true;
		previousSelectedIndex3 = listBoxState3.selectedItemIndex;
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
	// 円グラフを描画する
	for (size_t i = 0; i < values.size(); ++i)
	{
		const double startAngle = (starts[i] * 360_deg);
		const double angle = (ratios[i] * 360_deg);
		circle.drawPie(startAngle, angle, HSV{ (120 + 70 * i), 0.5, 0.95 });
	}

	// 境界線を描画する
	for (size_t i = 0; i < values.size(); ++i)
	{
		const double startAngle = (starts[i] * 360_deg);
		Line{ circle.center, Arg::angle = startAngle, circle.r }.draw(3);
	}

	// ラベルを描画する
	for (size_t i = 0; i < values.size(); ++i)
	{
		const double startAngle = (starts[i] * 360_deg);
		const double angle = (ratios[i] * 360_deg);
		const double midAngle = (startAngle + angle / 2.0);

		// 割合に応じてラベルの位置を調整する
		const Vec2 pos = OffsetCircular{ circle.center, ((ratios[i] < 0.1) ? 220.0 : (ratios[i] < 0.4) ? 120.0 : 90.0), midAngle };

		FontAsset(U"MainFont")(labels[i]).draw(24, Arg::bottomCenter = pos, ColorF{0.11});
		FontAsset(U"MainFont")(U"{:.1f}%"_fmt(ratios[i] * 100.0)).draw(18, Arg::topCenter = pos, ColorF{0.11});
	}
}

void Scene3::draw() const
{
	//リストボックスを描画
	SimpleGUI::ListBox(listBoxState1, Vec2{ 400, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState2, Vec2{ 810, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState3, Vec2{ 1520, 250 }, 300, 100);
	
	// 曲設定
	Rect{ 0, 430, 350, 150 }.draw();
	FontAsset(U"MainFont")(U"つくる").draw(48, Vec2{ 110, 475 }, buttonColor);

	FontAsset(U"MainFont")(U"つくる").draw(70, Vec2{ 20, 20 }, Palette::White);

	FontAsset(U"MainFont")(U"かし").draw(30, Vec2{ 400, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"きょく").draw(30, Vec2{ 810, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"一致率").draw(30, Vec2{ 1210, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"60％").draw(100, Vec2{ 1220, 700 }, Palette::Black);
	FontAsset(U"MainFont")(U"キャラクター").draw(30, Vec2{ 1520, 200 }, Palette::Black);

	// Charactertexture
	textureRect.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 });
	texture.draw(1520, 380);

	//Scene1Button
	Scene1Button.draw(Palette::Gray);
	FontAsset(U"MainFont")(U"おわりにする").drawAt(30, Vec2{ Scene1Button.center() }, Palette::White);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"かく").drawAt(Scene2Button.center(), Palette::White);

	//Scene4Button
	Scene4Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"ながす").drawAt(Vec2{ Scene4Button.center() }, Palette::White);

	//SaveButton
	SaveButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"つくる").drawAt(Vec2{ SaveButton.center() }, Palette::White);

	// loding
	if (isloading) {
		loadingRect.draw(ColorF{ 0.0, 0.5 });
		loadingtexture.rotated(angle).draw(860, 440);
	}
}
