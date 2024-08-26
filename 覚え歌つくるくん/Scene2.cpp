#include "Scene2.hpp"

const Array<double> Scene2::minColumnWidths = { 50, 200, 200, 200, 200 };
//const Array<String> Scene2::columnNames = { U"", U"", U"", U"", U"" };
const Array<String> Scene2::columnNames = { U"番号", U"語句 (必須)", U"読み (必須)", U"付加 1 (任意)", U"付加 2 (任意)" };

Scene2::Scene2(const InitData& init)
	: IScene{ init },
	table{ minColumnWidths, {
		.cellHeight = 45,
		.variableWidth = true,
	} }
{
	InitializeTable();
	// 初期化
	listBoxNeedsUpdate = true; // リストボックスの更新が必要なフラグを初期化
}

Scene2::~Scene2()
{
}

void Scene2::InitializeTable()
{
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

void Scene2::update()
{
	// リストボックスの状態を更新
	if (listBoxNeedsUpdate)
	{
		Array<String> lyricsfileNames;
		for (const auto& path : FileSystem::DirectoryContents(U"lyrics"))
		{
			lyricsfileNames << FileSystem::BaseName(path);
		}

		listBoxState = ListBoxState(lyricsfileNames);
		listBoxNeedsUpdate = false; // 更新完了
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
		TextWriter writer(U"lyrics/test.csv");
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
	}

	if (CreateButton.mouseOver() && MouseL.down())
	{
		AddNewRow();
	}

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

void Scene2::draw() const
{
	SimpleGUI::ListBox(listBoxState, Vec2{ 450, 200 }, 480, 600);

	// 語句入力
	Rect{ 0, 400, 350, 150 }.draw();
	font(U"語句入力").draw(40, Vec2{ 100, 445 }, ColorF{ 0.3, 0.7, 1.0 });

	tableScrollBar.draw();
	font(U"ファイル選択").draw(30, Rect{ 450, 150, 480, 200 }, Palette::Black);
	font(U"語句入力")	.draw(70, Rect{ 20, 20, 480, 200 }, Palette::White);

	//Scene1Button
	Scene1Button.draw(buttonColor);
	font(U"OP").drawAt(Vec2{ Scene1Button.center() }, Palette::White);

	//Scene3Button
	Scene3Button.draw(buttonColor);
	font(U"曲選択").drawAt(Vec2{ Scene3Button.center() }, Palette::White);

	//Scene4Button
	Scene4Button.draw(buttonColor);
	font(U"動画再生").drawAt(Vec2{ Scene4Button.center() }, Palette::White);

	//SaveButton
	SaveButton.draw(buttonColor);
	font(U"保存").drawAt(Vec2{ SaveButton.center() }, Palette::White);

	//CreateButton
	CreateButton.draw(buttonColor);
	font(U"新規作成").drawAt(Vec2{ CreateButton.center() }, Palette::White);

	/*
	font(U"語句(必須)")
		.draw(30, Rect{ 1050, 100, 480, 200 }, Palette::Black);
	font(U"読み(必須)")
		.draw(30, Rect{ 1250, 100, 480, 200 }, Palette::Black);
	font(U"付加１(任意)")
		.draw(30, Rect{ 1450, 100, 480, 200 }, Palette::Black);
	font(U"付加２(任意)")
		.draw(30, Rect{ 1650, 100, 480, 200 }, Palette::Black);
	*/
}
