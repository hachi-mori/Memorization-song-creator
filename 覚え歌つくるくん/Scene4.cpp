#include "stdafx.h"

const Array<double> Scene4::minColumnWidths = { 50, 200, 200, 200, 200 };
const Array<String> Scene4::columnNames = { U"番号", U"ことば", U"よみ*", U"つけたし1", U"つけたし2" };

Scene4::Scene4(const InitData& init)
	: IScene{ init }, previousSelectedIndex{ s3d::none },
	table{ minColumnWidths, {
		.cellHeight = 45,
		.variableWidth = true,
	} }, text{ U"" }, audio{ U"" }, audio2{ U"" }, playing{ false }
{
	InitializeTable();
	UpdateListBoxState();
	// 初期化
	listBoxNeedsUpdate = true; // リストボックスの更新が必要なフラグを初期化
}

Scene4::~Scene4()
{
}

void Scene4::InitializeTable()
{
	table.clear();
	table.push_back_row(columnNames, Array<int32>(CellCountX + 1, 0));
	table.setRowBackgroundColor(0, ColorF{ 0.9 });
	tableScrollBar.moveTo(0);  // スクロール位置を一番上にリセット
	for (int32 i = 1; i < (InitialCellCountY + 1); ++i)
	{
		AddNewRow();
	}
}

void Scene4::AddNewRow()
{
	size_t rowIdx = table.rows();
	Array<String> row(CellCountX + 1);
	row[0] = U"{}"_fmt(rowIdx);

	Array<int32> rowAlignments(CellCountX + 1, 1);
	rowAlignments[0] = 0;

	table.push_back_row(row, rowAlignments);
	table.setBackgroundColor(rowIdx, 0, ColorF{ 0.9 });
}

void Scene4::UpdateListBoxState()
{
	Array<String> lyricsfileNames;
	for (const auto& path : FileSystem::DirectoryContents(U"Voice"))
	{
		lyricsfileNames << FileSystem::BaseName(path);
	}

	listBoxState = ListBoxState(lyricsfileNames);
}

void Scene4::LoadCSVToTable(const String& filePath)
{
	// テーブルを初期化してから、CSVファイルの内容を読み込む
	InitializeTable();  // 既存のテーブルをクリア

	TextReader reader(filePath);
	String line;
	int32 rowIdx = 0;
	// 行ごとに読み込む
	while (reader.readLine(line))
	{
		Array<String> cells = line.split(U',');
		for (int32 colIdx = 0; colIdx < cells.size(); ++colIdx)
		{
			if (rowIdx + 1 > 15 && colIdx + 1 == 1)
			{
				AddNewRow();
			}
			table.setText(Point{ colIdx + 1, rowIdx + 1 }, cells[colIdx]);
		}
		++rowIdx;
	}
}

void Scene4::update()
{
	if (listBoxState.selectedItemIndex && listBoxState.selectedItemIndex != previousSelectedIndex)
	{
		// 選択されたアイテムの名前を取得
		selectedVoiceFile = listBoxState.items[listBoxState.selectedItemIndex.value()];

		// ハイフンで文字列を分割する
		Array<String> parts = selectedVoiceFile.split(U'-');

		// 歌詞を取得
		String firstElement = parts[0];
		String secondElement = parts[1];
		TextReader reader(U"lyrics/" + firstElement + U".csv");

		// te の内容を選択された要素の名前に変更
		te.text = firstElement;
		previousSelectedIndex = listBoxState.selectedItemIndex;
		// スプレッドシートに内容を表示
		LoadCSVToTable(U"lyrics/" + firstElement + U".csv");

		audio = Audio{ Audio::Stream,  U"Voice/" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U".wav"};
		audio.stop();
		audio2 = Audio{ Audio::Stream,  U"Instruments/" + secondElement + U".wav"};
		audio2.stop();
		playing = { false };
	}

	// ボタンの処理
	if (Scene1Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene1", 0.5s);
	}
	if (Scene2Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene2", 0.5s);
	}
	if (Scene3Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene3", 0.5s);
	}

	// ファイルの再生
	if (PlayButton.mouseOver() && MouseL.down())
	{
		if (listBoxState.selectedItemIndex) {
			// オーディオを新しいファイルで初期化
			if (!playing) {
				audio.play();
				audio2.play();
				audio2.setVolume(0.5);
				playing = { true };
			}
			else {
				audio.pause();
				audio2.pause();
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
			audio2.stop();
			audio.play();
			audio2.play();
			audio2.setVolume(0.5);
			playing = { true };
		}
		else {
			System::MessageBoxOK(U"", U"おぼえうたをえらんでください");
		}
	}

	// ファイルの削除
	if (DeleteButton.mouseOver() && MouseL.down() && listBoxState.selectedItemIndex)
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"", U"「" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U"」\nこのファイルをけしますか？");
		audio.stop();
		audio2.stop();
		playing = { false };

		// OK が選ばれたら
		if (result == MessageBoxResult::OK)
		{
			FileSystem::Remove(U"Voice/" + listBoxState.items[listBoxState.selectedItemIndex.value()] + U".wav");
			UpdateListBoxState();
			InitializeTable();
			te.text = U"";
			previousSelectedIndex = { s3d::none };
		}
	}

	if (listBoxState.selectedItemIndex) {
		//スプレッドシートの座標
		const Rect tableViewport = Scene::Rect().stretched(-150).movedBy(810, 0);

		tableScrollBar.updateLayout(tableViewport);
		tableScrollBar.updateConstraints(0, table.height(), tableViewport.h);
		if (tableViewport.mouseOver())
		{
			tableScrollBar.scroll(Mouse::Wheel() * 100);
		}
		tableScrollBar.update(tableViewport.mouseOver() ? Cursor::PosF() : MakeOptional<Vec2>());

		{
			ScopedViewport2D vp{ tableViewport };

			Transformer2D tf{
				Mat3x2::Translate(0, -tableScrollBar.value()),
				Mat3x2::Translate(tableViewport.pos.movedBy(0, -tableScrollBar.value()))
			};

			table.draw();
		}
	}

	//カーソル
	if (Scene1Button.mouseOver() || Scene2Button.mouseOver() || Scene3Button.mouseOver() || DeleteButton.mouseOver() || PlayButton.mouseOver() || RePlayButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (audio.isPlaying() && !audio2.isPlaying()) {
		audio2.play();
		audio2.setVolume(0.5);
	}
	if (!audio.isPlaying() && !audio2.isPlaying()) {
		playing = false;
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
	}
	else {
		System::Sleep(10);
		StopButton.draw(buttonColor);
		FontAsset(U"MainFont")(U"とめる").drawAt(Vec2{ StopButton.center() }, Palette::White);
	}

	//RePlayButton
	RePlayButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"さいしょからながす").drawAt(Vec2{ RePlayButton.center() }, Palette::White);


	tableScrollBar.draw();

	if (listBoxState.selectedItemIndex)FontAsset(U"MainFont")(te.text +U"の覚え歌").drawAt(44, Vec2{1389, 116.7}, Palette::Black);
}
