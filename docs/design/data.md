# データ設計

## 主要データの定義

### 1. 語句データ（CSV形式）

#### ファイル場所
- `App/lyrics/*.csv`

#### 形式
```csv
番号,単語,読み,文字数
1,花,はな,2
2,桜,さくら,3
3,春,はる,2
```

#### 列定義
| 列番号 | 列名 | データ型 | 説明 | 必須 |
|--------|------|----------|------|------|
| 0 | 番号 | 整数 | 連番（1から開始） | ○ |
| 1 | 単語 | 文字列 | 覚えたい単語 | ○ |
| 2 | 読み | 文字列 | ひらがな/カタカナでの読み | ○ |
| 3 | 文字数 | 整数 | 読みの文字数（検証用） | ○ |

#### データフロー
```
[Scene2: ユーザー入力]
    ↓ 保存
[App/lyrics/*.csv]
    ↓ 読み込み
[Scene3: 楽譜生成]
    ↓ モーラ分割（SplitToMoraWithoutLongVowel）
[モーラリスト]
    ↓ 音符割り当て（ProcessLyrics）
[生成楽譜JSON]
```

#### 妥当性検証
- Scene2で文字数の一致をチェック（`単語.length() == 文字数`）
- 不一致の行は赤色で表示（`invalidRows`）

---

### 2. 楽譜データ（JSON形式）

#### ファイル種類
1. **元楽譜**（OriginalScores）: 既存曲の楽譜テンプレート
2. **生成楽譜**（CreatedScores）: 語句を割り当てた覚え歌楽譜

#### ファイル場所
- 元楽譜: `App/OriginalScores/*.json`（Git管理）
- 生成楽譜: `App/CreatedScores/*.json`（実行時生成、Git無視）

#### 形式（元楽譜）
```json
{
  "16thnoteframe_length": 12,
  "notes": [
    {
      "frame_length": 47,
      "key": 67,
      "lyric": "い",
      "notelen": "G4"
    },
    {
      "frame_length": 2,
      "key": null,
      "lyric": "",
      "notelen": "R"
    }
  ]
}
```

#### フィールド定義

**トップレベル**
| フィールド | データ型 | 説明 |
|-----------|----------|------|
| `16thnoteframe_length` | 整数 | 16分音符のフレーム長（テンポ基準） |
| `notes` | 配列 | 音符の配列 |
| `__type` | 文字列 | 生成楽譜のみ: "Difference:値" 形式で類似度を記録 |

**notes要素**
| フィールド | データ型 | 説明 | 必須 |
|-----------|----------|------|------|
| `frame_length` | 整数 | 音符のフレーム長（音の長さ） | ○ |
| `key` | 整数 or null | MIDIノート番号（休符の場合はnull） | ○ |
| `lyric` | 文字列 | 歌詞（モーラ単位、休符は空文字） | ○ |
| `notelen` | 文字列 | 音符表記（"G4", "E4", "R"など） | ○ |

#### データフロー
```
[元楽譜JSON]
    ↓ 読み込み（DeterminePhrasesFromJSON）
[フレーズ構造: Array<Array<Note>>]
    ↓ 語句割り当て（ProcessLyrics）
[生成楽譜: 歌詞が埋め込まれたJSON]
    ↓ 保存
[App/CreatedScores/*.json]
    ↓ VOICEVOX API送信
[音声WAV]
```

#### Note構造体
```cpp
class Note {
public:
    String lyric;                  // 歌詞（モーラ）
    size_t frame_length;           // フレーム長
    std::optional<size_t> key;     // MIDIキー（休符はnullopt）
    String notelen;                // 音符表記
};
```

---

### 3. 音声データ（WAV形式）

#### ファイル場所
- `App/Voice/*.wav`

#### ファイル名形式
```
{語句名}-{楽曲名}-{キャラクター名}.wav
```
例: `花-ちょうちょ-ずんだもん.wav`

#### データフロー
```
[生成楽譜JSON]
    ↓ VOICEVOX API
[SingQuery.json]（一時ファイル）
    ↓ VOICEVOX API
[音声WAV]
    ↓ 保存
[App/Voice/*.wav]
    ↓ 読み込み
[Scene4: 再生]
```

#### 生成プロセス
1. **楽譜 → SingQuery**:
   - エンドポイント: `http://localhost:50021/sing_frame_audio_query?speaker=6000`
   - 入力: ScoreQuery JSON（楽譜）
   - 出力: SingQuery.json（歌唱クエリ）
2. **SingQuery → 音声**:
   - エンドポイント: `http://localhost:50021/frame_synthesis?speaker={speakerID}`
   - 入力: SingQuery.json
   - 出力: 音声WAV

---

### 4. 伴奏データ（WAV形式）

#### ファイル場所
- `App/Instruments/*.wav`

#### ファイル名形式
```
{楽曲名}.wav
```
例: `ちょうちょ.wav`

#### データフロー
```
[Instruments/*.wav]
    ↓ 読み込み（Scene4）
[Audio再生]（語句音声と同期）
```

#### 使用方法
- Scene4で「伴奏を同時再生」チェック時に使用
- 語句音声のファイル名から楽曲名を抽出し、対応する伴奏を読み込む

---

### 5. キャラクターデータ

#### ファイル場所
- 画像: `App/Character/*.png`
- サンプル音声: `App/Character/*_ららららら.wav`

#### ファイル種類
- `シルエット.png` - デフォルト画像
- `四国めたん_ららららら.wav` - サンプル音声（話者ID: 2）
- `ずんだもん_ららららら.wav` - サンプル音声（話者ID: 3）
- `春日部つむぎ_ららららら.wav` - サンプル音声（話者ID: 8）

#### データフロー
```
[VOICEVOX API: /speakers]
    ↓ 話者一覧取得
[Scene3: キャラクター選択リスト]
    ↓ 選択
[Character/*.png, *.wav]
    ↓ プレビュー表示・再生
```

---

## データ永続化の方式

### ファイルベースの永続化
- **データベース不使用**
- すべてのデータはファイルシステム上に保存
- CSV, JSON, WAV形式を使用

### ファイル種類と管理

| ファイル種類 | 場所 | 形式 | Git管理 | 生成タイミング |
|------------|------|------|---------|--------------|
| 語句データ | `App/lyrics/` | CSV | × | Scene2で保存 |
| 元楽譜 | `App/OriginalScores/` | JSON | ○ | 事前準備 |
| 生成楽譜 | `App/CreatedScores/` | JSON | × | Scene3で生成 |
| 音声 | `App/Voice/` | WAV | × | Scene3で生成 |
| 伴奏 | `App/Instruments/` | WAV | ○ | 事前準備 |
| キャラクター | `App/Character/` | PNG/WAV | ○ | 事前準備 |

---

## データ処理フロー（全体）

### 1. 語句入力フロー（Scene2）
```
ユーザー入力
    ↓
スプレッドシート（SimpleTable）
    ↓ 保存ボタン
CSV形式に変換
    ↓
App/lyrics/*.csv に保存
```

### 2. 覚え歌生成フロー（Scene3）
```
[入力選択]
語句CSV + 元楽譜JSON + キャラクターID
    ↓
[ステップ1: 歌詞処理]
CSV読み込み → モーラ分割
    ↓
JSON読み込み → フレーズ抽出
    ↓
歌詞を音符に割り当て
    ↓
伸ばし棒を母音に変換
    ↓
類似度を計算
    ↓
生成楽譜JSON保存（App/CreatedScores/）
    ↓
[ステップ2: 音声合成]
生成楽譜 → VOICEVOX API（sing_frame_audio_query）
    ↓
SingQuery.json（一時ファイル）
    ↓
SingQuery → VOICEVOX API（frame_synthesis）
    ↓
音声WAV保存（App/Voice/）
```

### 3. 音声再生フロー（Scene4）
```
[入力]
App/Voice/*.wav 一覧取得
    ↓
ファイル選択
    ↓
[処理]
対応する生成楽譜JSON読み込み
    ↓
歌詞をテーブル表示
    ↓
音声再生（Audio::play）
    ↓ オプション
伴奏同時再生（Instruments/*.wav）
```

---

## データ形式の例

### CSV例（lyrics/花.csv）
```csv
番号,単語,読み,文字数
1,花,はな,2
2,桜,さくら,3
3,春,はる,2
4,庭,にわ,2
```

### JSON例（OriginalScores/ちょうちょ.json）
```json
{
  "16thnoteframe_length": 12,
  "notes": [
    {
      "frame_length": 47,
      "key": 67,
      "lyric": "い",
      "notelen": "G4"
    },
    {
      "frame_length": 47,
      "key": 64,
      "lyric": "い",
      "notelen": "E4"
    },
    {
      "frame_length": 2,
      "key": null,
      "lyric": "",
      "notelen": "R"
    }
  ]
}
```

### JSON例（CreatedScores/花-ちょうちょ.json）
```json
{
  "__type": "Difference:15",
  "16thnoteframe_length": 12,
  "notes": [
    {
      "frame_length": 47,
      "key": 67,
      "lyric": "は",
      "notelen": "G4"
    },
    {
      "frame_length": 47,
      "key": 64,
      "lyric": "な",
      "notelen": "E4"
    },
    {
      "frame_length": 2,
      "key": null,
      "lyric": "",
      "notelen": "R"
    }
  ]
}
```

---

## データ設計の方針

### ファイル名規約
- 語句CSV: ユーザーが指定（日本語可）
- 生成楽譜JSON: `{語句名}-{楽曲名}.json`
- 音声WAV: `{語句名}-{楽曲名}-{キャラ名}.wav`
- 伴奏WAV: `{楽曲名}.wav`

### データの一貫性
- ファイル名の一致により関連データを紐付け
- 楽曲名は元楽譜JSONのファイル名（拡張子なし）
- キャラクター名はVOICEVOX話者名

### 今後の拡張（想定）
- 新しい楽曲追加: `OriginalScores/` と `Instruments/` にファイルを追加
- 新しいキャラクター: VOICEVOX話者一覧から自動取得（コード変更不要）
- データのインポート/エクスポート機能の追加が考えられる
