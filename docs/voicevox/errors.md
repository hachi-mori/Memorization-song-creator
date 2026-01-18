# VOICEVOX APIエラー処理設計

このドキュメントでは、VOICEVOX API関連のエラー処理の設計と実装状況を記載する。

## エラー分類

### 1. 接続エラー
**状況**: VOICEVOXエンジンが起動していない、またはネットワーク問題

**検出方法**:
- HTTP GETリクエストが失敗
- `task.getResponse().isOK()` が `false`

**現在の処理**:
- `GetSpeakers`: 空配列 `{}` を返す
- `SynthesizeFromJSONFile`: `false` を返し、`Print(U"Request failed.")`

**影響範囲**:
- Scene3初期化時: 話者リストが空になる
- 音声生成時: 生成失敗

**ユーザー向け表示**:
- Scene3: メッセージボックス（詳細はコードから確認）
- 具体的なエラー内容は表示されない

---

### 2. タイムアウトエラー
**状況**: API応答が遅い、処理時間が設定値を超過

**検出方法**:
```cpp
if (timeout <= stopwatch) {
    task.cancel();
    // エラー処理
}
```

**現在の処理**:
- タスクをキャンセル（`task.cancel()`）
- 生成途中のファイルを削除（`FileSystem::Remove(savePath)`）
- `Print(U"Request timed out.")`
- `false` を返す

**タイムアウト値**:
| 処理 | デフォルト | 理由 |
|------|-----------|------|
| 話者一覧取得 | 50秒 | ネットワーク遅延を考慮 |
| SingQuery生成 | 120秒 | 楽譜解析の処理時間 |
| 音声合成 | 300秒 | 長い楽曲の合成時間を考慮 |

**影響範囲**:
- 音声生成が中断される
- 不完全なWAVファイルは削除される（クリーンアップ）

**ユーザー向け表示**:
- Scene3: 生成失敗メッセージ
- タイムアウトの具体的な情報は表示されない

---

### 3. HTTPエラー（4xx, 5xx）
**状況**: APIリクエストは届いたが、エラーレスポンスが返る

**検出方法**:
```cpp
if (not task.getResponse().isOK()) {
    // エラー処理
}
```

**現在の処理**:
- 接続エラーと同じ処理
- HTTPステータスコードは取得していない
- エラーメッセージは解析していない

**想定されるエラー**:
| ステータスコード | 意味 | 発生状況 |
|----------------|------|----------|
| 400 Bad Request | 不正なリクエスト | JSONフォーマット不正 |
| 404 Not Found | エンドポイント不在 | URLの誤り |
| 500 Internal Server Error | サーバーエラー | VOICEVOX内部エラー |

**影響範囲**:
- 音声生成失敗
- エラーの詳細が不明（デバッグしにくい）

**ユーザー向け表示**:
- 接続エラーと同じ（区別なし）

---

### 4. JSON解析エラー
**状況**: JSONファイルが読み込めない、フォーマットが不正

**検出方法**:
```cpp
const JSON json = JSON::Load(jsonFilePath);
if (not json) {
    Print(U"Failed to load JSON file.");
    return false;
}
```

**現在の処理**:
- `false` を返す
- `Print(U"Failed to load JSON file.")`

**発生原因**:
- ファイルが存在しない
- ファイルが破損している
- JSON構文エラー

**影響範囲**:
- 音声生成失敗
- 生成楽譜JSONまたは一時ファイル（SingQuery.json）が不正

**ユーザー向け表示**:
- Scene3: 生成失敗メッセージ

---

### 5. ファイルシステムエラー
**状況**: ファイルの読み書きに失敗

**検出方法**:
- 現状: 明示的なチェックなし
- ファイル書き込み失敗時の処理は実装されていない

**想定される問題**:
- ディスク容量不足
- 権限不足
- ファイルパスが不正

**現在の処理**:
- 検出されない（エラーが発生しても無視される可能性）

**影響範囲**:
- 音声ファイルが保存されない（ユーザーには見えない失敗）

**改善余地**:
- ファイル書き込み後の存在確認
- ディスク容量チェック

---

## エラー処理フロー

### GetSpeakers（話者一覧取得）
```
[HTTP GET /speakers]
    ↓ タイムアウト？
    YES → 空配列を返す
    NO  ↓
    HTTPエラー？
    YES → 空配列を返す
    NO  ↓
    JSON解析
    ↓
[Speaker配列を返す]
```

### SynthesizeFromJSONFile（音声データ生成）
```
[JSON読み込み]
    ↓ 失敗？
    YES → false, Print("Failed to load JSON file.")
    NO  ↓
[HTTP POST]
    ↓ タイムアウト？
    YES → task.cancel(), ファイル削除, false, Print("Request timed out.")
    NO  ↓
    HTTPエラー？
    YES → ファイル削除, false, Print("Request failed.")
    NO  ↓
[成功]
    true, Print("File saved to: ...")
```

### SynthesizeVoiceFromScore（楽譜から音声生成）
```
[段階1: SingQuery生成]
    SynthesizeFromJSONFile()
    ↓ 失敗？
    YES → false, Print("Failed to create SingQuery.")
    NO  ↓
[段階2: 音声合成]
    SynthesizeFromJSONFile()
    ↓ 失敗？
    YES → false, Print("Failed to synthesize audio.")
    NO  ↓
[成功]
    true, Print("Voice synthesis succeeded."), ClearPrint()
```

---

## ユーザー向けエラー表示方針

### 現在の表示方法
**Scene3での処理**:
```cpp
bool synthesisSuccess = task.get();
if (synthesisSuccess) {
    // 成功メッセージ表示
} else {
    // 失敗メッセージ表示（メッセージボックス）
}
```

### 表示内容
- **成功時**: 保存成功メッセージ
- **失敗時**: 「生成に失敗しました」的なメッセージ（具体的な内容はコードから確認）
- **エラー詳細**: 表示されない

### 改善余地（今後の設計変更時の参考）
- エラー種別ごとに異なるメッセージを表示
- VOICEVOX未起動時の対処方法を案内
- タイムアウト時は「時間がかかっています」の表示
- HTTPエラー時はエラーコードを表示

---

## リトライ方針

### 現状
- **リトライなし**
- エラー発生時は即座に失敗を返す
- ユーザーが手動で再実行

### リトライが有効なケース
1. **一時的な接続エラー** - VOICEVOXの一時的な応答遅延
2. **タイムアウト** - 処理が遅いが、最終的には成功する可能性

### リトライが無効なケース
1. **JSON解析エラー** - データが不正（リトライしても失敗）
2. **400 Bad Request** - リクエストが不正（リトライしても失敗）

### 改善案（今後の設計変更時の参考）
```cpp
// リトライ設定例
const int maxRetries = 3;
const Duration retryInterval = 5s;

for (int retry = 0; retry < maxRetries; ++retry) {
    if (SynthesizeFromJSONFile(...)) {
        return true; // 成功
    }
    if (retry < maxRetries - 1) {
        System::Sleep(retryInterval);
    }
}
return false; // すべて失敗
```

---

## エラーログ

### 現在のログ出力
**出力先**: Siv3Dの `Print()` 関数（コンソール表示）

**ログ内容**:
| 状況 | メッセージ |
|------|-----------|
| タイムアウト | `"Request timed out."` |
| HTTPエラー | `"Request failed."` |
| JSON読み込み失敗 | `"Failed to load JSON file."` |
| SingQuery生成失敗 | `"Failed to create SingQuery."` |
| 音声合成失敗 | `"Failed to synthesize audio."` |
| 成功 | `"Voice synthesis succeeded."` |
| ファイル保存 | `"File saved to: {path}"` |

### 不足している情報
- HTTPステータスコード
- エラーメッセージの詳細
- タイムスタンプ
- リクエストURL
- リクエストボディ

### 改善案（今後の設計変更時の参考）
```cpp
// ログレベルの導入
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

void Log(LogLevel level, const String& message) {
    String prefix;
    switch (level) {
        case LogLevel::ERROR: prefix = U"[ERROR]"; break;
        case LogLevel::WARN:  prefix = U"[WARN]"; break;
        case LogLevel::INFO:  prefix = U"[INFO]"; break;
        case LogLevel::DEBUG: prefix = U"[DEBUG]"; break;
    }
    Print << DateTime::Now().format(U"yyyy-MM-dd HH:mm:ss") << U" " << prefix << U" " << message;
}
```

---

## エラー処理のベストプラクティス（今後の参考）

### 1. エラー種別の明確化
```cpp
enum class VoicevoxError {
    None,
    Timeout,
    ConnectionFailed,
    HttpError,
    JsonParseError,
    FileSystemError
};

struct VoicevoxResult {
    bool success;
    VoicevoxError error;
    String errorMessage;
    int httpStatusCode; // HTTPエラー時のみ
};
```

### 2. エラーハンドリングの統一
```cpp
VoicevoxResult result = VOICEVOX::SynthesizeVoiceFromScore(...);
if (not result.success) {
    switch (result.error) {
        case VoicevoxError::Timeout:
            ShowMessage(U"処理に時間がかかっています。もう一度お試しください。");
            break;
        case VoicevoxError::ConnectionFailed:
            ShowMessage(U"VOICEVOXに接続できません。VOICEVOXが起動しているか確認してください。");
            break;
        // ...
    }
}
```

### 3. ファイルクリーンアップの確実化
```cpp
// RAII によるクリーンアップ
class TempFileGuard {
    FilePath path;
public:
    TempFileGuard(FilePath p) : path(p) {}
    ~TempFileGuard() {
        if (FileSystem::IsFile(path)) {
            FileSystem::Remove(path);
        }
    }
};
```

---

## まとめ

### 現状の特徴
- **シンプル**: エラー処理は最小限
- **直感的**: `bool` 戻り値で成功/失敗を判定
- **クリーンアップ**: 失敗時のファイル削除は実装済み

### 改善余地
- エラー種別の詳細化
- HTTPステータスコードの取得・表示
- リトライ機構の追加
- ログの充実化
- ユーザー向けメッセージの改善

### 今後の方針（想定）
- 基本設計は維持（シンプルさを保つ）
- 必要に応じてエラー詳細化
- ログレベルの導入
- リトライ設定の追加
