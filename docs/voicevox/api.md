# VOICEVOX API 関数仕様

このドキュメントでは、`VoiceVoxSynthesizer.cpp` に実装されている各関数の詳細仕様を記載する。

## 関数一覧

| 関数名 | 役割 | 戻り値 |
|--------|------|--------|
| `GetSpeakers` | VOICEVOX話者一覧を取得 | `Array<Speaker>` |
| `SynthesizeFromJSONFile` | JSONクエリから音声データを生成 | `bool` |
| `SynthesizeVoiceFromScore` | 楽譜から音声を生成（2段階処理） | `bool` |

---

## 1. GetSpeakers

### 概要
VOICEVOX APIから利用可能な話者（キャラクター）とそのスタイル一覧を取得する。

### シグネチャ
```cpp
Array<Speaker> GetSpeakers(const Duration timeout = SecondsF{ 50.0 })
```

### 引数
| 引数名 | 型 | デフォルト値 | 説明 |
|--------|---|----|------|
| `timeout` | `Duration` | `50.0秒` | タイムアウト時間 |

### 戻り値
| 型 | 説明 |
|------|------|
| `Array<Speaker>` | 話者情報の配列。取得失敗時は空配列 |

### 処理フロー
1. `http://localhost:50021/speakers` に非同期GET リクエスト
2. タイムアウト監視（1msポーリング）
3. レスポンスが200番台か確認
4. JSON解析してSpeaker配列に変換
5. 結果を返す（失敗時は空配列）

### エラー処理
| エラー種別 | 戻り値 | 備考 |
|-----------|--------|------|
| タイムアウト | 空配列 `{}` | タスクをキャンセル |
| HTTPエラー | 空配列 `{}` | ステータスコードが200番台以外 |
| JSON解析エラー | 空配列 `{}` | JSONパースエラー |

### 使用例
```cpp
Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers();
if (speakers.isEmpty()) {
    Print << U"VOICEVOX接続失敗";
} else {
    for (const auto& speaker : speakers) {
        Print << speaker.name;
    }
}
```

### 備考
- Scene3の `InitializeLists()` で呼び出される
- 取得した話者のうち、ID=2, 3, 8 のみをフィルタリングして使用
- VOICEVOX未起動の場合は空配列が返る（エラーメッセージなし）

---

## 2. SynthesizeFromJSONFile

### 概要
JSONクエリファイルをPOSTして音声データを取得し、ファイルに保存する。  
汎用的な関数で、`SynthesizeVoiceFromScore` から2回呼び出される。

### シグネチャ
```cpp
bool SynthesizeFromJSONFile(
    const FilePath jsonFilePath,
    const FilePath savePath,
    const URL& synthesisURL,
    const Duration timeout = SecondsF{ 120.0 }
)
```

### 引数
| 引数名 | 型 | デフォルト値 | 説明 |
|--------|---|----|------|
| `jsonFilePath` | `FilePath` | - | 送信するJSONファイルのパス |
| `savePath` | `FilePath` | - | 受信データの保存先パス |
| `synthesisURL` | `URL` | - | POSTリクエスト先のURL |
| `timeout` | `Duration` | `120.0秒` | タイムアウト時間 |

### 戻り値
| 型 | 値 | 説明 |
|------|-----|------|
| `bool` | `true` | 成功（ファイル保存完了） |
| `bool` | `false` | 失敗（タイムアウト、HTTPエラー、JSON読み込み失敗） |

### 処理フロー
1. JSONファイルを読み込む（`JSON::Load`）
2. JSONをUTF-8にフォーマット（`formatUTF8Minimum`）
3. HTTPヘッダー設定（`Content-Type: application/json`）
4. 非同期POSTリクエスト（`SimpleHTTP::PostAsync`）
5. タイムアウト監視（1msポーリング）
6. レスポンスが200番台か確認
7. 成功メッセージを表示（`Print`）
8. 結果を返す

### エラー処理
| エラー種別 | 戻り値 | 副作用 | ログ出力 |
|-----------|--------|--------|---------|
| JSON読み込み失敗 | `false` | なし | `"Failed to load JSON file."` |
| タイムアウト | `false` | 不完全なファイルを削除 | `"Request timed out."` |
| HTTPエラー | `false` | 不完全なファイルを削除 | `"Request failed."` |
| 成功 | `true` | ファイル保存 | `"File saved to: {savePath}"` |

### タイムアウト時の処理
```cpp
if (timeout <= stopwatch)
{
    task.cancel();
    if (FileSystem::IsFile(savePath)) {
        FileSystem::Remove(savePath); // 不完全なファイルを削除
    }
    Print(U"Request timed out.");
    return false;
}
```

### 使用例
```cpp
FilePath queryPath = U"SingQuery.json";
FilePath outputPath = U"output.wav";
URL url = U"http://localhost:50021/frame_synthesis?speaker=3003";

bool success = VOICEVOX::SynthesizeFromJSONFile(queryPath, outputPath, url);
if (success) {
    Print << U"音声合成成功";
}
```

### 備考
- **汎用関数**: 任意のJSONクエリとエンドポイントに対応
- **ファイル削除**: エラー時は不完全なWAVファイルを削除（クリーンアップ処理）
- **同期処理**: 完了まで待機（非同期タスクを使用するが、ブロッキング）

---

## 3. SynthesizeVoiceFromScore

### 概要
楽譜（ScoreQuery JSON）から音声WAVを生成する。  
2段階処理（sing_frame_audio_query → frame_synthesis）を内部で実行。

### シグネチャ
```cpp
bool SynthesizeVoiceFromScore(
    const FilePath scoreFilePath,
    const FilePath outputAudioPath,
    const int32 speakerID = 3003,
    const Duration timeout = SecondsF{ 300.0 }
)
```

### 引数
| 引数名 | 型 | デフォルト値 | 説明 |
|--------|---|----|------|
| `scoreFilePath` | `FilePath` | - | 入力楽譜JSONのパス（CreatedScores/） |
| `outputAudioPath` | `FilePath` | - | 出力音声WAVのパス（Voice/） |
| `speakerID` | `int32` | `3003` | 歌声合成用の話者ID（3000番台） |
| `timeout` | `Duration` | `300.0秒` | タイムアウト時間（各段階に適用） |

### 戻り値
| 型 | 値 | 説明 |
|------|-----|------|
| `bool` | `true` | 成功（音声WAV生成完了） |
| `bool` | `false` | 失敗（どちらかの段階でエラー） |

### 処理フロー
```
[入力] scoreFilePath (楽譜JSON)
    ↓
[段階1] SynthesizeFromJSONFile()
    エンドポイント: sing_frame_audio_query?speaker=6000
    出力: SingQuery.json（一時ファイル）
    ↓ 失敗時は即座にfalseを返す
[段階2] SynthesizeFromJSONFile()
    入力: SingQuery.json
    エンドポイント: frame_synthesis?speaker={speakerID}
    出力: outputAudioPath (音声WAV)
    ↓
[出力] 成功メッセージ表示、trueを返す
```

### 内部処理詳細
1. **SingQuery.json のパス設定**（固定ファイル名）
2. **段階1**: ScoreQuery → SingQuery
   - URL: `http://localhost:50021/sing_frame_audio_query?speaker=6000`
   - 失敗時: `Print(U"Failed to create SingQuery.")` → `return false`
3. **段階2**: SingQuery → 音声WAV
   - URL: `http://localhost:50021/frame_synthesis?speaker={speakerID}`
   - 失敗時: `Print(U"Failed to synthesize audio.")` → `return false`
4. **成功時**: `Print(U"Voice synthesis succeeded.")` → `ClearPrint()` → `return true`

### エラー処理
| エラー箇所 | ログ出力 | 戻り値 |
|-----------|---------|--------|
| 段階1失敗 | `"Failed to create SingQuery."` | `false` |
| 段階2失敗 | `"Failed to synthesize audio."` | `false` |
| 成功 | `"Voice synthesis succeeded."` | `true` |

### 使用例
```cpp
FilePath scorePath = U"App/CreatedScores/花-ちょうちょ.json";
FilePath outputPath = U"App/Voice/花-ちょうちょ-ずんだもん.wav";
int32 speakerID = 3003; // ずんだもん（歌声）

bool success = VOICEVOX::SynthesizeVoiceFromScore(scorePath, outputPath, speakerID);
if (success) {
    Print << U"生成完了";
} else {
    Print << U"生成失敗";
}
```

### 話者IDの対応表（コードから推測）
| キャラクター名 | 読み上げID | 歌声ID (読み上げID + 3000) |
|--------------|-----------|--------------------------|
| 四国めたん | 2 | 3002 |
| ずんだもん | 3 | 3003 |
| 春日部つむぎ | 8 | 3008 |

### 備考
- **固定speaker=6000**: SingQuery生成時は共通の話者ID（歌声合成用の基準）
- **可変speakerID**: frame_synthesis時にキャラクター固有のIDを使用
- **一時ファイル**: `SingQuery.json` はルートディレクトリに生成され、削除されない
- **タイムアウト**: 各段階で同じタイムアウト値を使用（合計では最大 `timeout × 2`）
- **Scene3での呼び出し**: `Async` でラップされ、非同期実行される

---

## 共通仕様

### タイムアウト監視
すべての関数で共通のタイムアウト監視方法を使用:
```cpp
Stopwatch stopwatch{ StartImmediately::Yes };

while (not task.isReady())
{
    if (timeout <= stopwatch)
    {
        task.cancel();
        return {}; // or false
    }
    System::Sleep(1ms);
}
```

### HTTP通信
- **非同期タスク**: `SimpleHTTP::GetAsync` / `PostAsync` を使用
- **ヘッダー**: POSTでは `Content-Type: application/json` を設定
- **ステータスチェック**: `task.getResponse().isOK()` で200番台を確認

### ログ出力
- **成功**: `Print(U"...")`
- **失敗**: `Print(U"Failed to ...")`
- **ClearPrint**: 成功時にコンソールをクリア（Scene3のUI更新のため）

---

## 今後の拡張方向性（想定）

### 関数追加の余地
- `GetVersion()` - VOICEVOXバージョン取得
- `CancelSynthesis()` - 合成処理のキャンセル
- `GetSpeakerInfo(int32 speakerID)` - 特定話者の詳細情報取得

### パラメータ拡張
- リトライ回数・間隔の設定
- HTTPタイムアウトとポーリング間隔の分離
- ログレベル指定（DEBUG, INFO, ERROR）

### エラー詳細化
- HTTPステータスコードの取得
- VOICEVOX APIエラーメッセージの解析
- エラー種別の列挙型定義
