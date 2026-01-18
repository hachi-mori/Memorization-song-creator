# プロジェクト構成図

## ルートディレクトリ構成

```
Memorization-song-creator/
├── README.md                          # プロジェクトの概要・入口
├── 覚え歌つくるくん.sln                  # Visual Studioソリューションファイル
├── XmlToJsonConverter.cpp             # 楽譜XML→JSON変換ツール（外部ツール）
├── start.bat                          # 起動用バッチファイル
├── docs/                              # 設計ドキュメント（本フォルダ）
│   ├── architecture/                  # アーキテクチャ関連
│   ├── design/                        # 設計関連
│   ├── voicevox/                      # VOICEVOX API関連
│   └── dev/                           # 開発ガイドライン
└── 覚え歌つくるくん/                     # メインプロジェクト
    ├── *.cpp, *.hpp                   # ソースコード
    ├── *.vcxproj                      # Visual Studioプロジェクトファイル
    └── App/                           # 実行時データ・リソース
```

## 覚え歌つくるくん/（メインプロジェクト）

### ソースコードファイル

```
覚え歌つくるくん/
├── Main.cpp                           # アプリケーションエントリポイント
├── stdafx.h / stdafx.cpp              # プリコンパイル済みヘッダー
│
├── Scene1.cpp / Scene1.hpp            # トップ画面（タイトル・メニュー）
├── Scene2.cpp / Scene2.hpp            # 語句入力画面（スプレッドシート編集）
├── Scene3.cpp / Scene3.hpp            # 覚え歌生成画面（楽曲選択・音声合成）
├── Scene4.cpp / Scene4.hpp            # 再生画面（生成済み音声の再生）
├── CreditScene.cpp / CreditScene.hpp  # クレジット画面
│
├── VoiceVoxSynthesizer.cpp / .hpp     # VOICEVOX API通信・音声合成
├── LyricsProcessor.cpp / .hpp         # 歌詞処理（音符への割り当て）
├── JSONHandler.cpp / .hpp             # JSON操作（楽譜データ読み書き）
├── StringUtils.cpp / .hpp             # 文字列処理（モーラ分割など）
│
├── Note.hpp                           # 音符データ構造体
├── Button.cpp / Button.hpp            # ボタンUI部品
└── ScrollBar.cpp / ScrollBar.hpp      # スクロールバー・スプレッドシートUI
```

## App/（実行時リソース・データ）

```
App/
├── OriginalScores/                    # 元楽譜（JSON形式、Git管理）
│   ├── ちょうちょ.json
│   ├── むすんでひらいて.json
│   ├── 大きな古時計.json
│   ├── 幸せなら手をたたこう.json
│   └── 雪.json
│
├── Instruments/                       # 伴奏音源（.wav形式）
│   └── （各楽曲名.wav）
│
├── Character/                         # キャラクター画像・サンプル音声
│   ├── シルエット.png
│   ├── 四国めたん_ららららら.wav
│   ├── ずんだもん_ららららら.wav
│   └── 春日部つむぎ_ららららら.wav
│
├── lyrics/                            # ユーザー入力語句（CSV形式、実行時生成）
│   └── （語句1.csv, 語句2.csv, ...）
│
├── CreatedScores/                     # 生成された覚え歌楽譜（JSON、実行時生成、Git無視）
│   └── （語句1-楽曲名.json, ...）
│
├── Voice/                             # 生成された音声ファイル（.wav、実行時生成、Git無視）
│   └── （語句1-楽曲名-キャラ名.wav, ...）
│
├── font/                              # フォントファイル
│   └── rounded-mplus-20150529/
│
├── engine/                            # Siv3Dエンジンリソース（Git無視）
├── dll/                               # 依存DLL（Git無視）
├── example/                           # Siv3Dサンプルリソース（Git無視）
└── AS_DEBUG/                          # デバッグ出力（Git無視）
```

## ファイル管理方針

### Git管理対象
- ソースコード（.cpp, .hpp）
- 元楽譜（OriginalScores/*.json）
- 伴奏音源（Instruments/*.wav）
- キャラクター素材（Character/）
- プロジェクトファイル（.sln, .vcxproj）
- ドキュメント（docs/）

### Git無視（実行時生成・一時ファイル）
- ビルド成果物（.exe, .app, bin/, obj/）
- 実行時生成データ
  - `App/lyrics/` - ユーザー入力CSV
  - `App/CreatedScores/` - 生成楽譜JSON
  - `App/Voice/` - 生成音声WAV
- Siv3Dランタイム（engine/, dll/, example/）
- デバッグ出力（AS_DEBUG/）
- Visual Studio一時ファイル（.vs/）

## 主要ディレクトリの役割

| ディレクトリ | 役割 | 更新タイミング |
|------------|------|--------------|
| `覚え歌つくるくん/` | ソースコード一式 | 機能追加・修正時 |
| `App/OriginalScores/` | 元楽譜データベース | 新曲追加時 |
| `App/Instruments/` | 伴奏音源 | 新曲追加時 |
| `App/Character/` | キャラクター素材 | キャラ追加時 |
| `App/lyrics/` | ユーザー入力データ | 実行時（自動） |
| `App/CreatedScores/` | 生成楽譜 | 実行時（自動） |
| `App/Voice/` | 生成音声 | 実行時（自動） |
| `docs/` | 設計ドキュメント | 設計変更時 |

## 特記事項

- **XmlToJsonConverter.cpp**: 楽譜をXML形式からJSON形式に変換するための補助ツール。メインアプリケーションとは独立して実行される。
- **App/OriginalScores/**: このフォルダ内のJSONファイルが増えると、Scene3の楽曲選択リストに自動的に追加される。
- **App/lyrics/**: Scene2で作成・保存されたCSVファイルが格納される。ファイル名がScene3の語句選択リストに表示される。
- **App/Voice/**: ファイル名形式は「語句名-楽曲名-キャラクター名.wav」。Scene4の再生リストに表示される。
