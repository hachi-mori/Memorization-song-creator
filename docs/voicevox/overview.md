# VOICEVOX API ライブラリ概要

## 概要

このドキュメントでは、VOICEVOX APIを利用した音声合成機能の概要を記載する。

## VOICEVOX APIとは

- **目的**: テキスト読み上げおよび歌声合成
- **提供元**: VOICEVOX（ローカル実行）
- **通信方式**: HTTP REST API
- **実行環境**: ローカルホスト（`http://localhost:50021`）

## プロジェクト内での役割

- **担当ファイル**: `VoiceVoxSynthesizer.cpp` / `VoiceVoxSynthesizer.hpp`
- **名前空間**: `s3d::VOICEVOX`
- **主な機能**:
  1. VOICEVOX話者一覧の取得
  2. 楽譜（ScoreQuery）から歌声の生成
  3. エラー処理（タイムアウト、HTTPエラー）

## 前提条件

### VOICEVOXの起動
- アプリケーション実行前に **VOICEVOXエンジンが起動している必要がある**
- デフォルトポート: `50021`
- 起動確認: ブラウザで `http://localhost:50021/docs` にアクセス

### 必要なエンドポイント
| エンドポイント | 用途 | 使用箇所 |
|--------------|------|----------|
| `GET /speakers` | 話者一覧取得 | Scene3初期化時 |
| `POST /sing_frame_audio_query` | 楽譜→歌唱クエリ変換 | 音声生成（1段階目） |
| `POST /frame_synthesis` | 歌唱クエリ→音声合成 | 音声生成（2段階目） |

## アーキテクチャ

### レイヤー構成
```
[Scene3]
    ↓ 非同期タスク（Async）
[VOICEVOX::SynthesizeVoiceFromScore]
    ↓ 内部呼び出し
[VOICEVOX::SynthesizeFromJSONFile] × 2回
    ↓ HTTP通信
[VOICEVOX API（localhost:50021）]
```

### 同期 vs 非同期
- **VoiceVoxSynthesizerの関数**: すべて **同期処理**（完了まで待機）
  - `AsyncHTTPTask` を使用するが、完了まで `while` ループで待機
  - タイムアウト監視あり
- **Scene3からの呼び出し**: **非同期タスク**（`Async`）で実行
  - UIブロックを避けるため
  - 完了後に `task.get()` で結果を取得

## 音声合成の2段階処理

### 段階1: 楽譜 → 歌唱クエリ（SingQuery）
```
[入力] ScoreQuery JSON（楽譜）
    ↓ POST /sing_frame_audio_query?speaker=6000
[出力] SingQuery.json（歌唱パラメータ）
```
- **speaker**: 固定値 `6000`（歌声合成用の共通話者ID）
- **タイムアウト**: デフォルト120秒

### 段階2: 歌唱クエリ → 音声合成
```
[入力] SingQuery.json
    ↓ POST /frame_synthesis?speaker={speakerID}
[出力] 音声WAV
```
- **speaker**: キャラクター固有のID（3000番台）
  - 例: 四国めたん=3002, ずんだもん=3003, 春日部つむぎ=3008
- **タイムアウト**: デフォルト300秒

## タイムアウト設計

### タイムアウト値
| 処理 | デフォルト | 設定理由 |
|------|-----------|----------|
| 話者一覧取得 | 50秒 | ネットワーク遅延を考慮 |
| SingQuery生成 | 120秒 | 楽譜解析の処理時間 |
| 音声合成 | 300秒 | 長い楽曲の合成時間を考慮 |

### タイムアウト監視方法
```cpp
Stopwatch stopwatch{ StartImmediately::Yes };

while (not task.isReady())
{
    if (timeout <= stopwatch)
    {
        task.cancel();
        return false; // タイムアウト
    }
    System::Sleep(1ms); // ポーリング
}
```
- `Stopwatch` で経過時間を監視
- 1ms間隔でポーリング
- タイムアウト時は `task.cancel()` を呼び出し

## エラー処理の設計

### エラー種別
1. **タイムアウトエラー** - 処理時間超過
2. **HTTPエラー** - ステータスコードが200番台以外
3. **JSON解析エラー** - JSONファイル読み込み失敗
4. **接続エラー** - VOICEVOXが起動していない（HTTPエラーに含まれる）

### エラー時の挙動
| エラー種別 | 戻り値 | ファイル処理 | ログ |
|-----------|--------|------------|------|
| タイムアウト | `false` | 生成途中のWAVを削除 | `Print(U"Request timed out.")` |
| HTTPエラー | `false` | 生成途中のWAVを削除 | `Print(U"Request failed.")` |
| JSON不正 | `false` | なし | `Print(U"Failed to load JSON file.")` |

### エラー表示（Scene3）
- `VOICEVOX::SynthesizeVoiceFromScore()` が `false` を返した場合
- メッセージボックスで通知（実装内容はコードから確認可能）

## 主要データ構造

### Speaker構造体
```cpp
struct Speaker {
    String name;  // 話者名（例: "四国めたん"）
    
    struct Style {
        String name;  // スタイル名（例: "ノーマル"）
        int32 id;     // 話者ID（例: 2）
    };
    
    Array<Style> styles;  // スタイル一覧
};
```

### 話者IDの管理
- **読み上げ用ID**: 0～999番台（例: 2, 3, 8）
- **歌声合成用ID**: ID + 3000（例: 3002, 3003, 3008）
- Scene3では `speakerID + 3000` で変換

## 実装上の注意点

### Siv3DのHTTP機能
- **SimpleHTTP::GetAsync / PostAsync** を使用
- **非同期タスク（AsyncHTTPTask）** を返すが、完了まで待機
- **ヘッダー指定**: `Content-Type: application/json`

### UIブロック回避
- VOICEVOX関数は同期処理のため、直接呼び出すとUIがフリーズ
- **Scene3では `Async` でラップ** してバックグラウンド実行
- ローディング画面（`isloading`）で処理中を表示

### 一時ファイル
- **SingQuery.json**: 音声合成の中間ファイル
- 処理後も残る（再利用されない）
- 削除処理は実装されていない（今後の改善点と思われる）

### リトライ処理
- **現状: リトライなし**
- 失敗時は即座に `false` を返す
- 再試行はユーザーが手動で実行

## 使用例

### 話者一覧の取得
```cpp
Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers();
for (const auto& speaker : speakers) {
    Print << speaker.name;
    for (const auto& style : speaker.styles) {
        Print << U"  " << style.name << U" (ID: " << style.id << U")";
    }
}
```

### 音声合成
```cpp
FilePath scorePath = U"App/CreatedScores/花-ちょうちょ.json";
FilePath outputPath = U"App/Voice/花-ちょうちょ-ずんだもん.wav";
int32 speakerID = 3003; // ずんだもん（歌声）

bool success = VOICEVOX::SynthesizeVoiceFromScore(scorePath, outputPath, speakerID);
if (success) {
    Print << U"音声生成成功";
} else {
    Print << U"音声生成失敗";
}
```

## 今後の改善方向性（想定）

### 現状の課題
1. エラーメッセージが最小限（デバッグしにくい）
2. リトライ処理がない
3. 一時ファイル（SingQuery.json）が削除されない
4. HTTPステータスコードの詳細が取得できない

### 改善案（設計変更時の参考）
- エラー種別の詳細化（HTTPステータスコード、接続失敗、パース失敗など）
- リトライ回数・間隔の設定
- ログレベルの導入（DEBUG, INFO, ERROR）
- 一時ファイルの自動削除
- ユーザー向けエラーメッセージの多言語化

詳細は `docs/voicevox/api.md` および `docs/voicevox/errors.md` を参照。
