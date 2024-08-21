#pragma once
#include "stdafx.h"

using App = SceneManager<String>;

class Scene2 : public App::Scene
{
public:
	// コンストラクタとデストラクタ
	Scene2(const InitData& init);
	~Scene2();

	// 更新処理
	void update() override;

	// 描画処理
	void draw() const override;

private:
	// フォントの生成
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	// テーブルの初期設定
	void InitializeTable();

	// テーブルの行追加
	void AddNewRow();

	// テーブルオブジェクト
	SimpleTable table;

	// スクロールバー
	SasaGUI::ScrollBar tableScrollBar{ SasaGUI::Orientation::Vertical };

	// リストボックスの状態
	ListBoxState listBoxState;
	bool listBoxNeedsUpdate = true;

	// 編集状態
	Optional<Point> activeIndex;
	Optional<Point> nextActiveIndex;
	TextEditState textEditState;

	// セルの最小幅と見出し
	static const int32 CellCountX = 4;
	static const int32 InitialCellCountY = 15;
	static const Array<double> minColumnWidths;
	static const Array<String> columnNames;
};
