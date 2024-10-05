#pragma once
#include "stdafx.h"

using App = SceneManager<String>;

class Scene4 : public App::Scene
{
public:
	// コンストラクタとデストラクタ
	Scene4(const InitData& init);
	~Scene4();

	// 更新処理
	void update() override;

	// 描画処理
	void draw() const override;

private:
	// テーブルの初期設定
	void InitializeTable();

	// テーブルの行追加
	void AddNewRow();

	// リストボックスの更新
	void UpdateListBoxState();

	// CSVファイルの内容をスプレッドシートに表示
	void LoadCSVToTable(const String& filePath);

	// テーブルオブジェクト
	SimpleTable table;

	// スクロールバー
	SasaGUI::ScrollBar tableScrollBar{ SasaGUI::Orientation::Vertical };

	// リストボックスの状態
	mutable ListBoxState listBoxState;
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

	// テキストボックス
	mutable TextEditState te;
	s3d::Optional<uint64> previousSelectedIndex;

	// 保存メッセージ
	double MessageOpacity;

	const ColorF buttonColor{ 0.3, 0.7, 1.0 };  // ボタンの共通色

	Array<String> VoicefileNames;
	String selectedVoiceFile;
	Audio audio1{};

	const Rect Scene1Button{ 0, 900, 200, 100 };
	const Rect Scene2Button{ 0, 230, 350, 150 };
	const Rect Scene3Button{ 0, 430, 350, 150 };

	mutable TextAreaEditState textAreaEditState;

	mutable String text;

	const Rect DeleteButton{ 680, 820, 100, 80 };
	const Rect StopButton{ 450, 820, 150, 80 };
	const Rect PlayButton{ 450, 820, 150, 80 };
	const Rect RePlayButton{ 400, 920, 450, 80 };

	mutable Audio audio{ U"" };
	bool playing;
};
