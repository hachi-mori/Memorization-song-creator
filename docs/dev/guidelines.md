# 開発ガイドライン

このドキュメントでは、プロジェクトの開発ルールとベストプラクティスを記載する。

## 命名規則

### ファイル名
| 種類 | 規則 | 例 |
|------|------|-----|
| Sceneクラス | `Scene{番号}.cpp` / `.hpp` | `Scene1.cpp`, `Scene2.hpp` |
| 機能別クラス | `{機能名}{種類}.cpp` / `.hpp` | `VoiceVoxSynthesizer.cpp`, `JSONHandler.hpp` |
| UI部品 | `{部品名}.cpp` / `.hpp` | `Button.cpp`, `ScrollBar.hpp` |
| データ構造 | `{名前}.hpp` | `Note.hpp` |
| ユーティリティ | `{機能名}Utils.cpp` / `.hpp` | `StringUtils.cpp` |

### クラス名
- **PascalCase** を使用
- 例: `VoiceVoxSynthesizer`, `JSONHandler`, `Scene1`

### 関数名
- **PascalCase** を使用（Siv3D標準に準拠）
- 例: `GetSpeakers()`, `ProcessLyrics()`, `SynthesizeVoiceFromScore()`

### 変数名
- **camelCase** を使用
- 例: `speakerID`, `listBoxState`, `outputPath`
- メンバ変数にはプレフィックスなし（`m_` などを使用しない）

### 定数名
- **PascalCase** を使用
- 例: `CellCountX`, `InitialCellCountY`

### 名前空間
- 小文字または大文字を使用
- 例: `s3d::VOICEVOX`, `SasaGUI`

---

## コーディングスタイル

### インデント
- **タブ** を使用（スペースではない）
- Visual Studioのデフォルト設定に準拠

### 波括弧
- **同じ行** に開く（K&Rスタイル）
```cpp
if (condition) {
    // 処理
}
```

### コメント
- **日本語** を使用
- 必要最小限に留める
- ファイルヘッダーコメント:
```cpp
// ファイル名.cpp
#include "stdafx.h"
```

### 文字列リテラル
- Siv3Dの `U"..."` 形式を使用
```cpp
String message = U"こんにちは";
```

---

## 依存関係のルール

### レイヤー構造
```
[Scene層] - UIとユーザー操作
    ↓
[サービス層] - ビジネスロジック（JSONHandler, LyricsProcessor）
    ↓
[通信層] - 外部API通信（VoiceVoxSynthesizer）
    ↓
[ユーティリティ層] - 汎用処理（StringUtils）
```

### 禁止事項
- **Sceneから直接HTTP通信を行わない** → VoiceVoxSynthesizerを経由
- **Sceneに複雑なビジネスロジックを書かない** → 別モジュールに分離
- **循環依存を作らない** → 上位層は下位層のみを参照

### 推奨事項
- **新しい機能は専用ファイルに分離** → Main.cppやScene.cppに集約しない
- **データ処理はJSONHandler/LyricsProcessorに集約** → Scene内で完結させない
- **UI部品は再利用可能に設計** → Button, ScrollBarのように独立させる

---

## ファイル追加時の手順

### 1. 新しいSceneを追加
1. `Scene{番号}.cpp` と `Scene{番号}.hpp` を作成
2. `Main.cpp` に登録:
```cpp
manager.add<Scene5>(U"Scene5");
```
3. 既存Sceneに遷移ボタンを追加
4. `docs/design/scenes.md` を更新

### 2. 新しいデータ処理を追加
1. `JSONHandler.cpp` または `LyricsProcessor.cpp` に関数を追加
2. ヘッダーファイル（.hpp）に宣言を追加
3. `docs/architecture/modules.md` を更新

### 3. 新しいVOICEVOX機能を追加
1. `VoiceVoxSynthesizer.cpp` に関数を追加
2. `VoiceVoxSynthesizer.hpp` に宣言を追加
3. `docs/voicevox/api.md` に関数仕様を追加

### 4. 新しいUI部品を追加
1. `{部品名}.cpp` と `{部品名}.hpp` を作成
2. 必要なSceneから呼び出し
3. `docs/architecture/modules.md` を更新

---

## ドキュメント更新ルール

### 変更時に更新すべきドキュメント

| 変更内容 | 更新ドキュメント |
|---------|----------------|
| ファイル追加/削除 | `docs/architecture/tree.md`, `docs/architecture/modules.md` |
| Scene追加/変更 | `docs/design/scenes.md` |
| データ形式変更 | `docs/design/data.md` |
| VOICEVOX関数追加/変更 | `docs/voicevox/api.md`, `docs/voicevox/examples.md` |
| エラー処理変更 | `docs/voicevox/errors.md` |
| 開発ルール変更 | `docs/dev/guidelines.md` |

### ドキュメント更新の優先度
1. **必須**: 関数シグネチャ変更、データ形式変更
2. **推奨**: 新機能追加、ファイル追加
3. **任意**: 小さなバグ修正、コメント追加

### ドキュメント作成方針
- **現状を記述** → 将来の設計提案は書かない
- **コードから読み取れる事実のみ** → 推測は明記
- **簡潔に** → 1行で済むものは1行で

---

## ビルド・実行環境

### 前提条件
- **Visual Studio 2019 以降**
- **Siv3D** ライブラリ（プロジェクトに含まれる）
- **VOICEVOX** エンジン（ローカル起動、ポート50021）

### ビルド手順
1. `覚え歌つくるくん.sln` を Visual Studio で開く
2. ビルド構成を「Release」または「Debug」に設定
3. ビルド実行（Ctrl+Shift+B）

### 実行手順
1. VOICEVOXエンジンを起動（`http://localhost:50021`）
2. ビルドした実行ファイルを起動
   - または `start.bat` を実行

### 実行時の注意
- **App/フォルダ** が実行ファイルと同じディレクトリにあること
- **VOICEVOX** が起動していること

---

## テスト・デバッグ

### デバッグ出力
- `Print()` を使用してコンソールに出力
- `ClearPrint()` でコンソールをクリア

### エラー確認
- コンソール（Print出力）を確認
- Visual Studioのデバッガーを使用

### VOICEVOX接続確認
```cpp
Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers(SecondsF{ 5.0 });
if (speakers.isEmpty()) {
    Print << U"VOICEVOX接続失敗";
}
```

---

## Git管理

### コミット対象
- ソースコード（.cpp, .hpp）
- プロジェクトファイル（.sln, .vcxproj）
- ドキュメント（docs/）
- 元楽譜（App/OriginalScores/）
- 伴奏・素材（App/Instruments/, App/Character/）

### コミット対象外（.gitignore）
- ビルド成果物（.exe, bin/, obj/）
- 実行時生成データ（App/lyrics/, App/CreatedScores/, App/Voice/）
- Visual Studio一時ファイル（.vs/）
- Siv3Dランタイム（App/engine/, App/dll/, App/example/）

### コミットメッセージ
- 日本語または英語
- 簡潔に（1行で）
- 例: 「Scene3に一致率表示機能を追加」

---

## リリース・デプロイ

### リリース成果物
1. 実行ファイル（.exe）
2. App/フォルダ一式
   - OriginalScores/
   - Instruments/
   - Character/
   - font/
   - engine/
   - dll/
3. README.md

### リリース手順
1. Release構成でビルド
2. 実行ファイルをApp/フォルダにコピー
3. 不要なファイルを削除（lyrics/, CreatedScores/, Voice/）
4. ZIPで圧縮
5. GitHubリリースページにアップロード

---

## 新機能追加のチェックリスト

### Scene追加時
- [ ] Scene{番号}.cpp / .hpp を作成
- [ ] Main.cpp にシーン登録
- [ ] 既存Sceneに遷移ボタンを追加
- [ ] `docs/design/scenes.md` を更新
- [ ] 動作確認（遷移、UI表示）

### VOICEVOX機能追加時
- [ ] VoiceVoxSynthesizer.cpp に関数実装
- [ ] VoiceVoxSynthesizer.hpp に宣言追加
- [ ] エラー処理を実装
- [ ] `docs/voicevox/api.md` を更新
- [ ] `docs/voicevox/examples.md` に使用例を追加
- [ ] VOICEVOX起動状態で動作確認

### データ処理追加時
- [ ] JSONHandler/LyricsProcessor に関数追加
- [ ] `docs/architecture/modules.md` を更新
- [ ] `docs/design/data.md` を更新（データ形式変更時）
- [ ] 動作確認（入力→処理→出力）

---

## よくある質問（FAQ）

### Q1: 新しい楽曲を追加したい
**A**: 以下の手順で追加:
1. XML楽譜を用意
2. `XmlToJsonConverter.cpp` でJSON変換
3. 生成されたJSONを `App/OriginalScores/` に配置
4. 伴奏WAVを `App/Instruments/` に配置（同じファイル名）
5. アプリを再起動（自動的にリストに追加される）

### Q2: 新しいキャラクターを追加したい
**A**: 
- VOICEVOXエンジン側でキャラクターを追加
- アプリは自動的に話者一覧を取得するため、コード変更不要
- Scene3の話者フィルター（ID=2,3,8）を変更する場合は `Scene3.cpp` を修正

### Q3: エラーメッセージを変更したい
**A**:
1. `VoiceVoxSynthesizer.cpp` の `Print()` メッセージを変更
2. Scene3のメッセージボックス文言を変更
3. `docs/voicevox/errors.md` を更新

### Q4: タイムアウト時間を変更したい
**A**:
1. 関数呼び出し時に `timeout` 引数を指定
2. またはデフォルト値を変更（`VoiceVoxSynthesizer.hpp`）

---

## トラブルシューティング

### ビルドエラー
- **Siv3Dライブラリが見つからない** → プロジェクト設定を確認
- **include パスエラー** → stdafx.h を最初にincludeしているか確認

### 実行時エラー
- **ウィンドウが表示されない** → App/フォルダの配置を確認
- **フォントが表示されない** → App/font/ の存在を確認
- **音声生成失敗** → VOICEVOX起動を確認

### VOICEVOX関連
- **接続できない** → `http://localhost:50021/docs` にアクセスできるか確認
- **タイムアウト** → タイムアウト時間を延長
- **話者が表示されない** → VOICEVOX話者データをインストール

---

## まとめ

- **ファイル命名**: PascalCase、機能別に分離
- **依存ルール**: Scene → サービス → 通信 → ユーティリティ
- **ドキュメント更新**: ファイル追加・関数変更時は該当ドキュメントを更新
- **Git管理**: 生成データはコミット対象外
- **リリース**: Release構成でビルド、Appフォルダと一緒に配布

新機能追加時は、このガイドラインに従い、該当するドキュメントを更新すること。
