# モジュール / ファイル責務一覧

## 概要

このドキュメントでは、各ソースファイル（.cpp/.hpp）の役割と責務を記載する。

## ファイル一覧と責務

### エントリポイント・初期化

#### Main.cpp
- **役割**: アプリケーションのエントリポイント
- **主な処理**:
  - ウィンドウ設定（タイトル、サイズ、フルスクリーン）
  - シーン登録（Scene1～4, CreditScene）
  - フォント登録（MainFont）
  - メインループ（SceneManager.update()）
- **依存**: SceneManager, 各Sceneクラス
- **公開API**: `void Main()`

#### stdafx.h / stdafx.cpp
- **役割**: プリコンパイル済みヘッダー
- **内容**: Siv3D標準ヘッダー、プロジェクト共通includeの集約
- **備考**: すべての.cppファイルから最初にincludeされる

---

### Scene（画面）関連

各Sceneは `SceneManager<String>` によって管理され、文字列キー（"Scene1", "Scene2"等）で遷移する。

#### Scene1.cpp / Scene1.hpp
- **役割**: トップ画面（タイトル・メニュー）
- **主な機能**:
  - タイトルロゴ表示
  - Scene2（語句入力）への遷移ボタン
  - クレジット画面への遷移ボタン
  - フルスクリーン切り替えボタン
  - 終了ボタン
  - BGM（ジングル）再生
- **状態管理**: ロゴ拡大アニメーション、回転角度、音声再生状態
- **遷移先**: Scene2, CreditScene
- **ビジネスロジック**: なし（UI表示と遷移のみ）

#### Scene2.cpp / Scene2.hpp
- **役割**: 語句入力画面（スプレッドシート編集）
- **主な機能**:
  - スプレッドシート形式での語句入力（4列：番号、単語、読み、文字数）
  - 行の追加・削除
  - CSVファイルへの保存・読み込み
  - 他シーンへの遷移（Scene1, Scene3, Scene4）
- **データ処理**:
  - スプレッドシート → CSV形式への変換・保存（`App/lyrics/*.csv`）
  - CSV読み込み → スプレッドシート表示
- **依存**: SimpleTable（Siv3D）, ScrollBar, ListBox
- **主要メソッド**:
  - `InitializeTable()` - テーブル初期化
  - `AddNewRow()` - 行追加
  - `LoadCSVToTable()` - CSV読み込み
  - `UpdateListBoxState()` - リストボックス更新
- **備考**: 入力内容の妥当性チェック（文字数の一致など）を行い、invalidRowsに記録

#### Scene3.cpp / Scene3.hpp
- **役割**: 覚え歌生成画面（楽曲選択・音声合成）
- **主な機能**:
  - 3つのリストボックス選択
    1. 語句（lyrics/フォルダ内のCSV）
    2. 楽曲（OriginalScores/フォルダ内のJSON）
    3. キャラクター（VOICEVOX話者）
  - 覚え歌の生成（非同期処理）
  - 一致率の円グラフ表示
  - キャラクタープレビュー（画像・サンプル音声）
- **データ処理フロー**:
  1. CSVから語句を読み込み
  2. 元楽譜JSONを読み込み
  3. `UpdateJSONFromCSV()` で歌詞を楽譜に割り当て
  4. 生成楽譜を `App/CreatedScores/` に保存
  5. `VOICEVOX::SynthesizeVoiceFromScore()` で音声生成
  6. 音声を `App/Voice/` に保存
- **非同期処理**: `AsyncTask<bool> task` を使用（UI非ブロック）
- **依存**: VOICEVOX::*, JSONHandler, LyricsProcessor
- **主要メソッド**:
  - `InitializeLists()` - ファイル一覧・話者一覧の取得
  - `ToRatios()`, `CumulativeSum()` - 円グラフ計算
- **備考**: 類似度（Difference）の計算と表示がある

#### Scene4.cpp / Scene4.hpp
- **役割**: 再生画面（生成済み音声の再生）
- **主な機能**:
  - 生成済み音声ファイル一覧表示（`App/Voice/` 内）
  - 音声再生・停止
  - 音声ファイル削除
  - 歌詞テーブル表示（対応する楽譜から取得）
  - 音量調整スライダー
  - 伴奏の同時再生オプション
- **データ処理**:
  - `App/Voice/` 内のファイル一覧取得
  - 選択された音声ファイルの再生
  - 対応する楽譜（CreatedScores/）の読み込み・表示
- **依存**: Audio（Siv3D）, ListBox, SimpleTable
- **主要メソッド**:
  - `InitializeTable()` - 歌詞テーブル初期化
  - `LoadCSVToTable()` - 歌詞表示（実際はJSONから取得と思われる）
  - `UpdateListBoxState()` - リストボックス更新
- **備考**: 伴奏（Instruments/）との同期再生機能がある

#### CreditScene.cpp / CreditScene.hpp
- **役割**: クレジット画面
- **主な機能**:
  - 製作者情報・ライセンス情報の表示
  - Scene1への戻るボタン
- **備考**: 詳細はコードから確認可能（実装内容は軽微と推測）

---

### VOICEVOX API関連

#### VoiceVoxSynthesizer.cpp / VoiceVoxSynthesizer.hpp
- **役割**: VOICEVOX API通信・音声合成処理
- **名前空間**: `s3d::VOICEVOX`
- **主要構造体**:
  - `Speaker` - 話者情報
    - `name` - 話者名
    - `styles` - スタイル配列（`Style::name`, `Style::id`）
- **公開関数**:
  1. `Array<Speaker> GetSpeakers(Duration timeout = 50s)`
     - **役割**: VOICEVOX APIから話者一覧を取得
     - **エンドポイント**: `http://localhost:50021/speakers`
     - **処理**: 非同期HTTP GET → JSON解析 → Speaker配列
     - **エラー処理**: タイムアウト or HTTPエラー時は空配列を返す
  2. `bool SynthesizeFromJSONFile(FilePath jsonPath, FilePath savePath, URL synthesisURL, Duration timeout = 120s)`
     - **役割**: JSONクエリをPOSTして音声データを取得・保存
     - **処理**: JSON読み込み → HTTP POST（JSON） → WAVファイル保存
     - **エラー処理**: タイムアウト・HTTPエラー・JSON不正時にfalseを返し、失敗時は保存ファイルを削除
  3. `bool SynthesizeVoiceFromScore(FilePath scorePath, FilePath outputPath, int32 speakerID = 3003, Duration timeout = 300s)`
     - **役割**: 楽譜（ScoreQuery）から音声合成までの2段階処理
     - **処理フロー**:
       1. ScoreQuery → `sing_frame_audio_query` → SingQuery.json生成
       2. SingQuery.json → `frame_synthesis` → 音声.wav生成
     - **エンドポイント**:
       - `http://localhost:50021/sing_frame_audio_query?speaker=6000`
       - `http://localhost:50021/frame_synthesis?speaker={speakerID}`
     - **エラー処理**: 各段階で失敗時はfalse、成功時はtrueを返す
- **依存**: SimpleHTTP（Siv3D）, JSON（Siv3D）
- **備考**: 
  - すべて同期処理（AsyncHTTPTaskを使用するが、完了まで待機）
  - タイムアウトはStopwatch + System::Sleep(1ms)でポーリング
  - UIブロックを避けるため、Scene3では非同期タスク（Async）内で呼び出される

---

### データ処理関連

#### JSONHandler.cpp / JSONHandler.hpp
- **役割**: 楽譜JSON操作・歌詞の楽譜への割り当て
- **公開関数**:
  1. `void UpdateJSONFromCSV(FilePath csvPath, FilePath jsonPath, FilePath outputPath)`
     - **役割**: CSVの語句を元楽譜JSONに割り当て、生成楽譜を出力
     - **処理フロー**:
       1. CSV読み込み → モーラ分割（`SplitToMoraWithoutLongVowel`）
       2. JSON楽譜読み込み → フレーズ抽出（`DeterminePhrasesFromJSON`）
       3. 歌詞を音符に割り当て（`ProcessLyrics`）
       4. 伸ばし棒を母音に変換（`ReplaceLongVowelMarks`）
       5. 類似度（Difference）をJSONに追加
       6. JSONに歌詞を書き込み、出力
     - **出力**: `App/CreatedScores/*.json`
  2. `Array<Array<Note>> DeterminePhrasesFromJSON(const JSON& json)`
     - **役割**: JSON楽譜からフレーズ構造（Note配列の配列）を抽出
     - **処理**: JSON内の音符情報をNote構造体に変換
- **依存**: LyricsProcessor, StringUtils, Note
- **備考**: 類似度（Difference）の計算結果をJSONの`__type`フィールドに格納

#### LyricsProcessor.cpp / LyricsProcessor.hpp
- **役割**: 歌詞の音符への割り当てロジック
- **公開関数**:
  1. `int ProcessLyrics(JSON json, Array<Array<String>> originalLyricList, Array<Array<Note>>& phrases)`
     - **役割**: 語句のモーラリストを楽譜のフレーズに割り当て
     - **処理**:
       - フレーズごとにモーラを音符に割り当て
       - モーラ数と音符数が異なる場合の調整（`AdjustMoraAndNotes`）
       - フレーズ不足時は楽譜を繰り返しコピー（最大10回）
     - **戻り値**: 類似度（差分、低いほど一致）
  2. `void HandleMoreMoraThanNotes(JSON json, Array<String>& moraList, Array<Note>& notes)`
     - **役割**: モーラ数 > 音符数の場合の調整
  3. `void HandleMoreNotesThanMora(Array<String> moraList, Array<Note>& notes)`
     - **役割**: 音符数 > モーラ数の場合の調整
  4. `bool AdjustMoraAndNotes(JSON json, Array<String>& moraList, Array<Note>& notes)`
     - **役割**: モーラと音符の数を一致させる処理の統括
- **依存**: Note, StringUtils
- **備考**: フレーズ不足時の楽譜コピーロジックがある（覚え歌の長さに応じて自動拡張）

#### StringUtils.cpp / StringUtils.hpp
- **役割**: 文字列処理（主にモーラ分割・母音処理）
- **公開関数**:
  1. `Array<String> SplitToMoraWithoutLongVowel(const String& input)`
     - **役割**: 日本語文字列をモーラ（音節）単位に分割
     - **処理**: ひらがな・カタカナ→モーラ分割、伸ばし棒は「ー」として保持
     - **戻り値**: モーラの配列（例: "かあさん" → ["か", "あ", "さ", "ん"]）
  2. `void ReplaceLongVowelMarks(Array<Array<Note>>& phrases)`
     - **役割**: 歌詞中の伸ばし棒「ー」を直前のモーラの母音に変換
     - **処理**: 例: "カー" → "カア"（VOICEVOXは伸ばし棒を認識しないため）
  3. `String GetVowelFromKana(const String& kana)`
     - **役割**: かな文字から母音を抽出
     - **処理**: "か" → "a", "き" → "i", など
- **依存**: なし（純粋な文字列処理）
- **備考**: 日本語音声合成に必要な前処理を担当

---

### UI部品関連

#### Button.cpp / Button.hpp
- **役割**: 汎用ボタンUI部品
- **公開関数**: `bool Button(const Rect& rect, const Font& font, const String& text, bool enabled)`
- **機能**: 矩形領域にテキストを描画し、クリック判定を返す
- **備考**: 各Sceneから利用される共通UI部品

#### ScrollBar.cpp / ScrollBar.hpp
- **役割**: スクロールバー・スプレッドシート表示
- **名前空間**: `SasaGUI`
- **主要クラス**:
  - `ScrollBar` - 縦/横スクロールバーの描画・操作
  - `Delay` - 遅延処理ヘルパー
  - `Repeat` - リピート処理ヘルパー
- **機能**:
  - スクロール位置管理
  - マウス操作（ドラッグ、クリック）
  - スムーススクロールアニメーション
  - フェードイン/アウト効果
- **依存**: Siv3D GUI関連
- **備考**: Scene2, Scene4のスプレッドシート表示で使用

---

### データ構造

#### Note.hpp
- **役割**: 音符データの構造体定義
- **構造体**: `class Note`
  - `String lyric` - 歌詞（モーラ単位）
  - `size_t frame_length` - フレーム長（音の長さ）
  - `std::optional<size_t> key` - 音のキー（MIDIノート番号、休符の場合はnullopt）
  - `String notelen` - 音符の長さ表記（"G4", "E4", "R"など）
- **用途**: JSONからの楽譜読み込み、歌詞割り当て、音声合成で使用
- **備考**: VOICEVOXのSingQuery形式に対応

---

## 依存関係の方針

### レイヤー構造（推測）

```
[Scene層]
  ↓ 呼び出し
[サービス層] JSONHandler, LyricsProcessor
  ↓ 呼び出し
[通信層] VoiceVoxSynthesizer
  ↓ 呼び出し
[ユーティリティ層] StringUtils, Note
```

### ルール（コードから読み取れる範囲）
- **Sceneは直接HTTP通信を行わない** → VoiceVoxSynthesizerを経由
- **Sceneはデータ処理を直接行わない** → JSONHandler, LyricsProcessorを経由
- **VoiceVoxSynthesizerは純粋な通信層** → ビジネスロジックを持たない
- **StringUtilsは依存なし** → どの層からも呼び出し可能

## 典型的な呼び出しフロー

### 覚え歌生成（Scene3）
```
Scene3::update()
  → Async（非同期タスク起動）
    → UpdateJSONFromCSV()           // JSONHandler
      → SplitToMoraWithoutLongVowel() // StringUtils
      → DeterminePhrasesFromJSON()    // JSONHandler
      → ProcessLyrics()               // LyricsProcessor
        → AdjustMoraAndNotes()        // LyricsProcessor
      → ReplaceLongVowelMarks()       // StringUtils
      → JSON保存（CreatedScores/）
    → VOICEVOX::SynthesizeVoiceFromScore() // VoiceVoxSynthesizer
      → SynthesizeFromJSONFile() (1回目: sing_frame_audio_query)
        → SimpleHTTP::PostAsync()
      → SynthesizeFromJSONFile() (2回目: frame_synthesis)
        → SimpleHTTP::PostAsync()
      → WAV保存（Voice/）
```

### 語句保存（Scene2）
```
Scene2::update()
  → SimpleTable → CSV保存（lyrics/）
```

### 音声再生（Scene4）
```
Scene4::update()
  → Audio::play()
  → JSON読み込み（CreatedScores/）→ SimpleTable表示
```

## 更新時の注意点

- **新しいSceneを追加する場合**: Main.cppにシーン登録を追加
- **VOICEVOX APIの仕様変更**: VoiceVoxSynthesizer.cppのエンドポイントURLを修正
- **楽譜形式の変更**: Note.hpp, JSONHandler.cpp, LyricsProcessor.cppを修正
- **新しいデータ処理の追加**: JSONHandler or LyricsProcessorに関数を追加（Sceneに直接書かない）
