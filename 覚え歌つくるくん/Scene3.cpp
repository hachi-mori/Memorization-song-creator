#include "stdafx.h"

Scene3::Scene3(const InitData& init)
	: IScene{ init },
	texture{ U"Character/シルエット.png" },
	textureRect{ 1540, 390, 280, 460 },
	selectListBox3{ false },
	previousSelectedIndex1{ s3d::none },
	previousSelectedIndex2{ s3d::none },
	previousSelectedIndex3{ s3d::none },
	Difference{-1},
	isloading{ false }, saved{ false }
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
		changeScene(U"Scene1", 0.5s);
	}
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2", 0.5s);
	}
	if (Scene4Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene4", 0.5s);
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
			saved = true;
			//Print(U"音声合成が成功しました。");
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

		accumulate += Scene::DeltaTime();

		if (interval <= accumulate)
		{
			accumulate -= interval;

			state = (state + 1) % 4;
		}
	}

	if (listBoxState1.selectedItemIndex != previousSelectedIndex1)
	{
		listBoxState2.selectedItemIndex = none;
		previousSelectedIndex2 = none;
		Difference = -1;

		// 選択が変更されたときの処理
		previousSelectedIndex1 = listBoxState1.selectedItemIndex;
		String selectedlyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
		const FilePath lyricsFilePath = U"lyrics/{}.csv"_fmt(selectedlyricsName);

		// JSONファイルを更新する処理
		for (size_t i = 0; i < OriginalScoresfileNames.size(); ++i)
		{
			String selectedScoreName = OriginalScoresfileNames[i];
			const FilePath scoreFilePath = U"OriginalScores/{}.json"_fmt(selectedScoreName);
			const FilePath createscoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedlyricsName, selectedScoreName);

			// JSONファイルを更新
			UpdateJSONFromCSV(lyricsFilePath, scoreFilePath, createscoreFilePath);
		}

		// `differenceScorePairs`をリセット
		differenceScorePairs.clear();

		// すべてのListBox2の要素に対してDifferenceを取得して、ペアを配列に格納する
		for (size_t i = 0; i < OriginalScoresfileNames.size(); ++i)
		{
			String selectedScoreName = OriginalScoresfileNames[i];  // インデックスを使用して全要素を取得
			const FilePath createScoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedlyricsName, selectedScoreName);

			JSON json = JSON::Load(createScoreFilePath);

			// 値を文字列として取得
			String typeValue = json[U"__type"].getString();

			// "Difference:" プレフィックスを取り除く
			String differenceStr = typeValue.substr(11); // プレフィックスの長さは10

			// 数値に変換
			int differenceValue = Parse<int>(differenceStr);

			// DifferenceとOriginalScoresfileNamesのペアを配列に追加
			differenceScorePairs.push_back({ differenceValue, selectedScoreName });
		}

		// ペアをDifferenceの値でソート
		std::sort(differenceScorePairs.begin(), differenceScorePairs.end(), [](const std::pair<int, String>& a, const std::pair<int, String>& b) {
			return a.first < b.first;  // Differenceの値が小さい順にソート
		});

		// ソートされた結果でOriginalScoresfileNamesを再構築
		OriginalScoresfileNames.clear();
		for (const auto& pair : differenceScorePairs)
		{
			OriginalScoresfileNames.push_back(pair.second);  // ソートされたスコア名を再度格納
		}

		// 新しいリストでListBoxState2を再作成
		listBoxState2 = ListBoxState(OriginalScoresfileNames);
	}


	if (listBoxState2.selectedItemIndex != previousSelectedIndex2)
	{
		if (listBoxState1.selectedItemIndex) {
			previousSelectedIndex2 = listBoxState2.selectedItemIndex;
			String selectedLyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
			String selectedScoreName = OriginalScoresfileNames[listBoxState2.selectedItemIndex.value()];
			const FilePath createScoreFilePath = U"CreatedScores/{}-{}.json"_fmt(selectedLyricsName, selectedScoreName);

			JSON json = JSON::Load(createScoreFilePath);

			//Print << (U"CreatedScores/" + createScoreFilePath);

			// 値を文字列として取得
			String typeValue = json[U"__type"].getString();

			// "Difference:" プレフィックスを取り除く
			String differenceStr = typeValue.substr(11); // プレフィックスの長さは10

			// 数値に変換
			Difference = Parse<int>(differenceStr);
			//Print << U"Difference value: " << Difference;
		}
		else {
			listBoxState2.selectedItemIndex = s3d::none;
			System::MessageBoxOK(U"", U"「かし」を先にえらんでください");
		}
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

	//絵文字
	if (listBoxState2.selectedItemIndex == 0|| listBoxState2.selectedItemIndex == 1) {
		b = 1;
	}
	else if (listBoxState2.selectedItemIndex == 2 || listBoxState2.selectedItemIndex == 3) {
		b = 2;
	}
	else if (listBoxState2.selectedItemIndex == 4 || listBoxState2.selectedItemIndex == 5){
		b = 3;
	}
	else if (Difference == -1){
		b = -1;
	}

	//カーソル
	if (Scene1Button.mouseOver()|| Scene2Button.mouseOver()|| Scene4Button.mouseOver()|| SaveButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (saved)
	{
		// 透明度を1.0に設定
		MessageOpacity = 1.0;
		saved = false;  // 一度だけ実行するためにフラグをリセット
	}
	// 透明度が0より大きい場合、徐々に透明度を減少させる
	if (MessageOpacity > 0.0)
	{
		MessageOpacity -= Scene::DeltaTime() * 0.5;  // 減少速度を調整（0.5は速度の調整係数）
		if (MessageOpacity < 0.0) {
			MessageOpacity = 0.0;  // 透明度が負の値にならないようにする
		}
	}
}


void Scene3::draw() const
{
	//リストボックスを描画
	SimpleGUI::ListBox(listBoxState1, Vec2{ 400, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState2, Vec2{ 810, 250 }, 300, 600);
	SimpleGUI::ListBox(listBoxState3, Vec2{ 1520, 250 }, 300, 100);
	if (b == 1) {
		emoji1.scaled(1.5).drawAt(1220, 350);
		emoji4.scaled(1.5).drawAt(1220, 550);
		emoji4.scaled(1.5).drawAt(1220, 750);
		FontAsset(U"MainFont")(U"すごく\nいいね！").drawAt(Vec2{1420, 350}, Palette::Black);
	}
	else if (b == 2) {
		emoji4.scaled(1.5).drawAt(1220, 350);
		emoji2.scaled(1.5).drawAt(1220, 550);
		emoji4.scaled(1.5).drawAt(1220, 750);
		FontAsset(U"MainFont")(U"これも\nいいね！").drawAt(Vec2{ 1420, 550 }, Palette::Black);
	}
	else if (b == 3) {
		emoji4.scaled(1.5).drawAt(1220, 350);
		emoji4.scaled(1.5).drawAt(1220, 550);
		emoji3.scaled(1.5).drawAt(1220, 750);
		FontAsset(U"MainFont")(U" ありだね！").drawAt(Vec2{ 1420, 750 }, Palette::Black);
	}
	else if (b == -1){
		emoji4.scaled(1.5).drawAt(1220, 350);
		emoji4.scaled(1.5).drawAt(1220, 550);
		emoji4.scaled(1.5).drawAt(1220, 750);
	}
	
	// 曲設定
	Rect{ 0, 430, 350, 150 }.draw();
	FontAsset(U"MainFont")(U"つくる").draw(48, Vec2{ 110, 475 }, buttonColor);

	FontAsset(U"MainFont")(U"つくる").draw(70, Vec2{ 20, 20 }, Palette::White);

	FontAsset(U"MainFont")(U"かし").draw(30, Vec2{ 400, 200 }, Palette::Black);

	FontAsset(U"MainFont")(U"きょく").draw(30, Vec2{ 810, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"おすすめ度").draw(30, Vec2{ 1210, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"キャラクター").draw(30, Vec2{ 1520, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"→").draw(60, Vec2{ 723, 500 }, Palette::Black);

	// Charactertexture
	textureRect.draw(ColorF{ 0.0, 0.0, 0.0, 0.0 });
	texture.draw(1520, 380);

	//Scene1Button
	Scene1Button.draw(Palette::Gray);
	FontAsset(U"MainFont")(U"おわりにする").drawAt(30, Vec2{ Scene1Button.center() }, Palette::White);

	// Scene2Button
	Scene2Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"かしをかく").drawAt(Scene2Button.center(), Palette::White);

	//Scene4Button
	Scene4Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"ながす").drawAt(Vec2{ Scene4Button.center() }, Palette::White);

	//SaveButton
	SaveButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"つくる").drawAt(Vec2{ SaveButton.center() }, Palette::White);

	FontAsset(U"MainFont")(U"作れました").draw(54, Arg::bottomRight = Vec2{ 1400, 1026 }, ColorF{ 0.0, 0.0, 0.0, MessageOpacity });

	// loding
	if (isloading) {
		loadingRect.draw(ColorF{ 0.0, 0.5 });
		if (interval <= accumulate)
		{
			accumulate -= interval;

			state = (state + 1) % 4;
		}
		if (1 <= state)
		{
			note.draw(860, 440);
		}
		if (2 <= state)
		{
			note.draw(800, 440);
		}
		if (3 <= state)
		{
			note.draw(740, 440);
		}
		texture.resized(texture.width() * 0.6, texture.height() * 0.6).draw(900, 400);
		FontAsset(U"MainFont")(U"学習ソングを作っています・・・").drawAt(Vec2{ 1920 / 2,1080 / 2 + 180 }, Palette::White);
	}
}
