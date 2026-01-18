# VOICEVOX API 使用例

このドキュメントでは、VOICEVOX APIライブラリの具体的な使用例を記載する。

## 基本的な使用例

### 1. 話者一覧の取得と表示

```cpp
#include "VoiceVoxSynthesizer.hpp"

void ShowSpeakers()
{
    // 話者一覧を取得（タイムアウト: 50秒）
    Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers();
    
    if (speakers.isEmpty())
    {
        Print << U"VOICEVOXに接続できませんでした。";
        Print << U"VOICEVOXが起動しているか確認してください。";
        return;
    }
    
    // 話者とスタイルを表示
    for (const auto& speaker : speakers)
    {
        Print << U"話者: " << speaker.name;
        
        for (const auto& style : speaker.styles)
        {
            Print << U"  スタイル: " << style.name << U" (ID: " << style.id << U")";
        }
    }
}
```

**出力例**:
```
話者: 四国めたん
  スタイル: ノーマル (ID: 2)
  スタイル: あまあま (ID: 0)
  スタイル: ツンツン (ID: 6)
話者: ずんだもん
  スタイル: ノーマル (ID: 3)
  スタイル: あまあま (ID: 1)
...
```

---

### 2. 楽譜から音声を生成（基本）

```cpp
#include "VoiceVoxSynthesizer.hpp"

void GenerateVoice()
{
    // ファイルパス設定
    FilePath scorePath = U"App/CreatedScores/花-ちょうちょ.json";
    FilePath outputPath = U"App/Voice/花-ちょうちょ-ずんだもん.wav";
    int32 speakerID = 3003; // ずんだもん（歌声）
    
    // 音声生成
    bool success = VOICEVOX::SynthesizeVoiceFromScore(
        scorePath,
        outputPath,
        speakerID
    );
    
    if (success)
    {
        Print << U"音声生成成功: " << outputPath;
    }
    else
    {
        Print << U"音声生成失敗";
    }
}
```

---

### 3. カスタムタイムアウトの使用

```cpp
#include "VoiceVoxSynthesizer.hpp"

void GenerateVoiceWithCustomTimeout()
{
    FilePath scorePath = U"App/CreatedScores/大きな古時計.json";
    FilePath outputPath = U"App/Voice/大きな古時計.wav";
    int32 speakerID = 3002; // 四国めたん（歌声）
    
    // 長い楽曲のため、タイムアウトを600秒に延長
    Duration timeout = SecondsF{ 600.0 };
    
    bool success = VOICEVOX::SynthesizeVoiceFromScore(
        scorePath,
        outputPath,
        speakerID,
        timeout
    );
    
    if (success)
    {
        Print << U"音声生成成功";
    }
    else
    {
        Print << U"タイムアウトまたはエラーが発生しました";
    }
}
```

---

## Scene3での使用例（実際のコード）

### 非同期タスクでの音声生成

```cpp
// Scene3::update() 内での使用例
if (SaveButton.mouseOver() && MouseL.down() && !task.isValid())
{
    if (listBoxState1.selectedItemIndex && 
        listBoxState2.selectedItemIndex && 
        listBoxState3.selectedItemIndex)
    {
        isloading = true;
        
        // 非同期タスクを開始
        task = Async([this]()
        {
            // 話者IDを計算（読み上げID + 3000）
            Optional<int32> selectedSpeakerID;
            if (listBoxState3.selectedItemIndex)
            {
                selectedSpeakerID = speakerIDs[*listBoxState3.selectedItemIndex] + 3000;
            }
            
            // ファイル名を構築
            String selectedlyricsName = lyricsfileNames[listBoxState1.selectedItemIndex.value()];
            String selectedScoreName = OriginalScoresfileNames[listBoxState2.selectedItemIndex.value()];
            String selectedCharacterName = speakers[listBoxState3.selectedItemIndex.value()];
            
            const FilePath lyricsFilePath = U"lyrics/{}.csv"_fmt(selectedlyricsName);
            const FilePath scoreFilePath = U"OriginalScores/{}.json"_fmt(selectedScoreName);
            const FilePath outputAudioFilePath = U"Voice/{}-{}-{}.wav"_fmt(
                selectedlyricsName, 
                selectedScoreName, 
                selectedCharacterName
            );
            const FilePath createscoreFilePath = U"CreatedScores/{}-{}.json"_fmt(
                selectedlyricsName, 
                selectedScoreName
            );
            
            // 音声合成
            return VOICEVOX::SynthesizeVoiceFromScore(
                createscoreFilePath, 
                outputAudioFilePath, 
                *selectedSpeakerID
            );
        });
    }
    else
    {
        System::MessageBoxOK(U"", U"「かし」と「きょく」と「キャラクター」をえらんでからほぞんしてください");
    }
}

// タスク完了チェック
if (task.isReady())
{
    bool synthesisSuccess = task.get();
    if (synthesisSuccess)
    {
        // 成功処理
        saved = true;
        MessageOpacity = 1.0;
    }
    else
    {
        // 失敗処理（メッセージ表示）
    }
    
    isloading = false;
    task = AsyncTask<bool>{}; // タスクリセット
}
```

---

## 高度な使用例

### 1. 複数の音声を連続生成

```cpp
void GenerateMultipleVoices()
{
    // 楽譜リスト
    Array<FilePath> scores = {
        U"App/CreatedScores/花-ちょうちょ.json",
        U"App/CreatedScores/桜-むすんでひらいて.json",
        U"App/CreatedScores/春-雪.json"
    };
    
    // 話者IDリスト
    Array<int32> speakerIDs = { 3002, 3003, 3008 };
    Array<String> speakerNames = { U"四国めたん", U"ずんだもん", U"春日部つむぎ" };
    
    // すべての組み合わせで生成
    for (const auto& score : scores)
    {
        String scoreName = FileSystem::BaseName(score);
        
        for (size_t i = 0; i < speakerIDs.size(); ++i)
        {
            FilePath outputPath = U"App/Voice/{}-{}.wav"_fmt(scoreName, speakerNames[i]);
            
            Print << U"生成中: " << outputPath;
            
            bool success = VOICEVOX::SynthesizeVoiceFromScore(
                score,
                outputPath,
                speakerIDs[i]
            );
            
            if (success)
            {
                Print << U"  ✓ 成功";
            }
            else
            {
                Print << U"  ✗ 失敗";
            }
        }
    }
}
```

---

### 2. エラーハンドリングの詳細例

```cpp
void GenerateVoiceWithErrorHandling()
{
    FilePath scorePath = U"App/CreatedScores/花-ちょうちょ.json";
    FilePath outputPath = U"App/Voice/花-ちょうちょ-ずんだもん.wav";
    int32 speakerID = 3003;
    
    // ファイル存在確認
    if (not FileSystem::IsFile(scorePath))
    {
        Print << U"エラー: 楽譜ファイルが見つかりません: " << scorePath;
        return;
    }
    
    // 音声生成
    Print << U"音声生成を開始します...";
    bool success = VOICEVOX::SynthesizeVoiceFromScore(scorePath, outputPath, speakerID);
    
    if (success)
    {
        // 生成ファイルの確認
        if (FileSystem::IsFile(outputPath))
        {
            size_t fileSize = FileSystem::FileSize(outputPath);
            Print << U"✓ 音声生成成功";
            Print << U"  ファイル: " << outputPath;
            Print << U"  サイズ: " << fileSize << U" bytes";
        }
        else
        {
            Print << U"✗ 生成は成功しましたが、ファイルが見つかりません";
        }
    }
    else
    {
        Print << U"✗ 音声生成失敗";
        Print << U"  考えられる原因:";
        Print << U"  - VOICEVOXが起動していない";
        Print << U"  - 楽譜ファイルが不正";
        Print << U"  - タイムアウト";
    }
}
```

---

### 3. 話者IDの変換ユーティリティ

```cpp
// 読み上げIDから歌声IDへの変換
int32 ToSingingVoiceID(int32 readingVoiceID)
{
    return readingVoiceID + 3000;
}

// 使用例
void Example()
{
    int32 zunndamonReadingID = 3;     // ずんだもん（読み上げ）
    int32 zunndamonSingingID = ToSingingVoiceID(zunndamonReadingID); // 3003
    
    FilePath scorePath = U"App/CreatedScores/test.json";
    FilePath outputPath = U"App/Voice/test.wav";
    
    bool success = VOICEVOX::SynthesizeVoiceFromScore(
        scorePath,
        outputPath,
        zunndamonSingingID
    );
}
```

---

## デバッグ・トラブルシューティング

### 1. VOICEVOX接続確認

```cpp
void CheckVoicevoxConnection()
{
    Print << U"VOICEVOX接続チェック...";
    
    // タイムアウトを短く設定（5秒）- デフォルト50秒を上書き
    Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers(SecondsF{ 5.0 });
    
    if (speakers.isEmpty())
    {
        Print << U"✗ VOICEVOXに接続できません";
        Print << U"  以下を確認してください:";
        Print << U"  1. VOICEVOXが起動しているか";
        Print << U"  2. ポート50021が使用可能か";
        Print << U"  3. ファイアウォールがブロックしていないか";
    }
    else
    {
        Print << U"✓ VOICEVOX接続成功";
        Print << U"  利用可能な話者数: " << speakers.size();
    }
}
```

---

### 2. 音声生成のステップごとデバッグ

```cpp
void DebugVoiceSynthesis()
{
    FilePath scorePath = U"App/CreatedScores/花-ちょうちょ.json";
    FilePath singQueryPath = U"SingQuery.json";
    FilePath outputPath = U"App/Voice/debug.wav";
    
    Print << U"=== 音声生成デバッグ ===";
    
    // 段階1: SingQuery生成
    Print << U"[1/2] SingQuery生成中...";
    URL singQueryURL = U"http://localhost:50021/sing_frame_audio_query?speaker=6000";
    bool step1 = VOICEVOX::SynthesizeFromJSONFile(scorePath, singQueryPath, singQueryURL);
    
    if (step1)
    {
        Print << U"  ✓ SingQuery生成成功";
        Print << U"  ファイルサイズ: " << FileSystem::FileSize(singQueryPath) << U" bytes";
    }
    else
    {
        Print << U"  ✗ SingQuery生成失敗";
        return;
    }
    
    // 段階2: 音声合成
    Print << U"[2/2] 音声合成中...";
    URL synthesisURL = U"http://localhost:50021/frame_synthesis?speaker=3003";
    bool step2 = VOICEVOX::SynthesizeFromJSONFile(singQueryPath, outputPath, synthesisURL);
    
    if (step2)
    {
        Print << U"  ✓ 音声合成成功";
        Print << U"  ファイルサイズ: " << FileSystem::FileSize(outputPath) << U" bytes";
    }
    else
    {
        Print << U"  ✗ 音声合成失敗";
    }
}
```

---

## よくある問題と解決方法

### 問題1: 「音声生成失敗」が表示される

**原因**:
- VOICEVOXが起動していない
- ポート50021が使用中
- 楽譜JSONが不正

**解決方法**:
```cpp
// 接続確認
CheckVoicevoxConnection();

// 楽譜JSONの確認
JSON json = JSON::Load(U"App/CreatedScores/test.json");
if (json) {
    Print << U"JSON読み込み成功";
} else {
    Print << U"JSON読み込み失敗";
}
```

---

### 問題2: タイムアウトが発生する

**原因**:
- 楽曲が長い
- PCの性能が低い

**解決方法**:
```cpp
// タイムアウトを延長（デフォルト300秒 → 600秒）
Duration longTimeout = SecondsF{ 600.0 };
bool success = VOICEVOX::SynthesizeVoiceFromScore(
    scorePath,
    outputPath,
    speakerID,
    longTimeout
);
```

---

### 問題3: 話者が表示されない

**原因**:
- VOICEVOXのバージョンが古い
- 話者データが未インストール

**解決方法**:
```cpp
// 全話者を表示して確認
Array<VOICEVOX::Speaker> speakers = VOICEVOX::GetSpeakers();
for (const auto& speaker : speakers) {
    Print << speaker.name;
    for (const auto& style : speaker.styles) {
        Print << U"  ID: " << style.id;
    }
}
```

---

## まとめ

- **基本**: `GetSpeakers()` で話者取得、`SynthesizeVoiceFromScore()` で音声生成
- **非同期**: Scene3では `Async` でラップしてUI非ブロック
- **エラー処理**: 戻り値 `bool` で成功/失敗を判定
- **デバッグ**: ステップごとに確認、接続チェックを実施
