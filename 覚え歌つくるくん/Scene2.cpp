#include "Scene2.hpp"

const Array<double> Scene2::minColumnWidths = { 50, 200, 200, 200, 200 };
const Array<String> Scene2::columnNames = { U"番号", U"ことば*", U"よみ*", U"つけたし1", U"つけたし2" };

Scene2::Scene2(const InitData& init)
	: IScene{ init }, previousSelectedIndex{ s3d::none },
	table{ minColumnWidths, {
		.cellHeight = 45,
		.variableWidth = true,
	}}, saved{ false }
{
	InitializeTable();
	UpdateListBoxState();
	// 初期化
	listBoxNeedsUpdate = true; // リストボックスの更新が必要なフラグを初期化
}

Scene2::~Scene2()
{
}

void Scene2::InitializeTable()
{
	table.clear();
	table.push_back_row(columnNames, Array<int32>(CellCountX + 1, 0));
	table.setRowBackgroundColor(0, ColorF{ 0.9 });

	for (int32 i = 1; i < (InitialCellCountY + 1); ++i)
	{
		AddNewRow();
	}
}

void Scene2::AddNewRow()
{
	size_t rowIdx = table.rows();
	Array<String> row(CellCountX + 1);
	row[0] = U"{}"_fmt(rowIdx);

	Array<int32> rowAlignments(CellCountX + 1, 1);
	rowAlignments[0] = 0;

	table.push_back_row(row, rowAlignments);
	table.setBackgroundColor(rowIdx, 0, ColorF{ 0.9 });
}

void Scene2::UpdateListBoxState()
{
	Array<String> lyricsfileNames;
	for (const auto& path : FileSystem::DirectoryContents(U"lyrics"))
	{
		lyricsfileNames << FileSystem::BaseName(path);
	}

	listBoxState = ListBoxState(lyricsfileNames);
}

void Scene2::LoadCSVToTable(const String& filePath)
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

void Scene2::update()
{
	if (listBoxState.selectedItemIndex && listBoxState.selectedItemIndex != previousSelectedIndex)
	{
		// te の内容を選択された要素の名前に変更
		te.text = listBoxState.items[*listBoxState.selectedItemIndex];
		previousSelectedIndex = listBoxState.selectedItemIndex;
		// スプレッドシートに内容を表示
		LoadCSVToTable(U"lyrics/" + listBoxState.items[*listBoxState.selectedItemIndex] + U".csv");
	}

	// ボタンの処理
	if (Scene1Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene1");
	}

	if (Scene3Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene3");
	}

	if (Scene4Button.mouseOver() && MouseL.down())
	{
		changeScene(U"Scene4");
	}

	if (SaveButton.mouseOver() && MouseL.down())
	{
		if (listBoxState.selectedItemIndex) {
			if (te.text) {
				TextWriter writer(U"lyrics/" + te.text + U".csv");
				if (!writer)
				{
					throw Error{ U"Failed to open test.csv" };
				}

				for (int32 a = 1; a < table.rows(); ++a)
				{
					for (int32 b = 1; b < CellCountX + 1; ++b)
					{
						writer.write(table.getItem(Point{ b, a }).text);
						writer.write(b < CellCountX ? U"," : U"\n");
					}
				}
				writer.close();
				UpdateListBoxState();

				// リストボックスで保存したファイル名を選択している状態にする
				for (size_t i = 0; i < listBoxState.items.size(); ++i)
				{
					if (listBoxState.items[i] == te.text)
					{
						listBoxState.selectedItemIndex = static_cast<int32>(i);
						break;
					}
				}
				SaveMassage = te.text;
				saved = true;
			}
			else {
				System::MessageBoxOK(U"", U"タイトルをかいてからほぞんしてください");
			}
		}
		else {
			System::MessageBoxOK(U"", U"ファイルをあたらしくつくってからほぞんしてください");
		}
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

	if (CreateButton.mouseOver() && MouseL.down())
	{
		String baseName = U"新しいファイル";
		String fileName;
		int index = 1;

		// 「新しいファイル」から始めて、存在するファイルがあれば番号を追加
		do
		{
			fileName = baseName + (index == 1 ? U"" : U"{}"_fmt(index));
			index++;
		} while (FileSystem::Exists(U"lyrics/" + fileName + U".csv"));

		// 新しいファイル名でファイルを作成
		TextWriter writer(U"lyrics/" + fileName + U".csv");
		writer.close();

		UpdateListBoxState();
		InitializeTable();

		// リストボックスで新しいファイル名を選択している状態にする
		for (size_t i = 0; i < listBoxState.items.size(); ++i)
		{
			if (listBoxState.items[i] == fileName)
			{
				listBoxState.selectedItemIndex = static_cast<int32>(i);
				break;
			}
		}
		te.text = fileName;
	}

	// ファイルの削除
	if (DeleteButton.mouseOver() && MouseL.down())
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"",U"「" + listBoxState.items[*listBoxState.selectedItemIndex] + U"」\nこのファイルを削除しますか？");

		// OK が選ばれたら
		if (result == MessageBoxResult::OK)
		{
			FileSystem::Remove(U"lyrics/" + listBoxState.items[*listBoxState.selectedItemIndex] + U".csv");
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

		if (nextActiveIndex)
		{
			activeIndex = *nextActiveIndex;
			textEditState = TextEditState{ table.getItem(*activeIndex).text };
			textEditState.cursorPos = textEditState.text.length();
			textEditState.active = true;
			nextActiveIndex.reset();
		}

		{
			ScopedViewport2D vp{ tableViewport };

			Transformer2D tf{
				Mat3x2::Translate(0, -tableScrollBar.value()),
				Mat3x2::Translate(tableViewport.pos.movedBy(0, -tableScrollBar.value()))
			};

			if (MouseL.down())
			{
				const auto newActiveIndex = table.cellIndex({ 0, 0 }, Cursor::Pos());

				if (newActiveIndex != activeIndex)
				{
					activeIndex = newActiveIndex;

					if (activeIndex)
					{
						textEditState = TextEditState{ table.getItem(*activeIndex).text };
						textEditState.cursorPos = textEditState.text.length();
						textEditState.active = true;
						MouseL.clearInput();
						// テキストボックスを非アクティブにする
						te.active = false;
					}
				}
			}

			table.draw();

			if (activeIndex && (activeIndex->x != 0 && activeIndex->y != 0))
			{
				const RectF cellRegion = table.cellRegion({ 0, 0 }, *activeIndex);

				if (SimpleGUI::TextBox(textEditState, cellRegion.pos, cellRegion.w))
				{
					table.setText(*activeIndex, textEditState.text);
				}

				if (textEditState.enterKey)
				{
					nextActiveIndex = Point{ activeIndex->x, (activeIndex->y + 1) };

					if (nextActiveIndex->y == table.rows())
					{
						AddNewRow();
					}
				}

				if ((1 < activeIndex->y) && KeyUp.down())
				{
					nextActiveIndex = Point{ activeIndex->x, (activeIndex->y - 1) };
				}

				if ((activeIndex->y < table.rows()) && KeyDown.down())
				{
					nextActiveIndex = Point{ activeIndex->x, (activeIndex->y + 1) };

					if (nextActiveIndex->y == table.rows())
					{
						AddNewRow();
					}
				}

				if ((1 < activeIndex->x) && KeyLeft.down())
				{
					nextActiveIndex = Point{ (activeIndex->x - 1), activeIndex->y };
				}

				if ((activeIndex->x < CellCountX) && KeyRight.down())
				{
					nextActiveIndex = Point{ (activeIndex->x + 1), activeIndex->y };
				}

				if (KeyDelete.down())
				{
					textEditState.clear();
					table.setText(*activeIndex, U"");
				}
			}
		}
	}	
}

void Scene2::draw() const
{
	SimpleGUI::ListBox(listBoxState, Vec2{ 450, 200 }, 480, 600);

	// 語句入力
	Rect{ 0, 230, 350, 150 }.draw();
	FontAsset(U"MainFont")(U"かく").draw(48, Vec2{ 135, 275 }, buttonColor);

	tableScrollBar.draw();
	FontAsset(U"MainFont")(U"かしのリスト").draw(30, Rect{ 450, 150, 480, 200 }, Palette::Black);
	FontAsset(U"MainFont")(U"かく")	.draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);

	//Scene1Button
	Scene1Button.draw(Palette::Gray);
	FontAsset(U"MainFont")(U"おわりにする").drawAt(30,Vec2{ Scene1Button.center() }, Palette::White);

	//Scene3Button
	Scene3Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"つくる").drawAt(Vec2{ Scene3Button.center() }, Palette::White);

	//Scene4Button
	Scene4Button.draw(buttonColor);
	FontAsset(U"MainFont")(U"ながす").drawAt(Vec2{ Scene4Button.center() }, Palette::White);

	//SaveButton
	SaveButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"ほぞん").drawAt(Vec2{ SaveButton.center() }, Palette::White);

	//CreateButton
	CreateButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"あたらしくつくる").drawAt(Vec2{ CreateButton.center() }, Palette::White);

	//DeleteButton
	DeleteButton.draw(buttonColor);
	FontAsset(U"MainFont")(U"けす").drawAt(Vec2{ DeleteButton.center() }, Palette::White);

	// TextBox
	FontAsset(U"MainFont")(U"タイトル").drawAt(25, Vec2{ 1220, 80 }, Palette::Black);
	SimpleGUI::TextBox(te, Vec2{ 1024, 100}, 400);

	FontAsset(U"MainFont")(SaveMassage + U"をほぞんしました").drawAt(34, Vec2{1235, 985}, ColorF{0.0, 0.0, 0.0, MessageOpacity});
}
