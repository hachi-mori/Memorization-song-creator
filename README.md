# ② 高専プロコン2024 自由部門「覚え歌つくるくん」リポジトリ 2

### 前のリポジトリ
[Procon2024_jiyu](https://github.com/nitsc-proclub/Procon2024_jiyu)
### Googleドライブ
[高専プロコン自由部門2024](https://drive.google.com/drive/folders/1HmZsJvoMT-5zhSHq9_fHqYmKP89a_8km?usp=sharing)
### Googleドキュメント
[プロコン自由部門](https://docs.google.com/document/d/14RA8oUoEWT44qQ685kf_xuRA-mvPgYWn0L0EnTpojF0/edit)

## 構成図 (一部省略)
<pre>
覚え歌つくるくん/
├ Button.cpp  //ボタン
├ Button.hpp
├ ScrollBar.cpp  //スクロールバーとスプレッドシート
├ ScrollBar.hpp
├ VoiceVoxSynthesizer.cpp  //VOICEVOXAPIを叩く
├ VoiceVoxSynthesizer.hpp
├ Note.hpp  //楽譜生成で使う構造体
├ JSONHandler.cpp  //JSONファイルを操作する
├ JSONHandler.cpp 
├ LyricsProcessor.cpp  //歌詞の処理
├ LyricsProcessor.cpp
├ StringUtils.cpp  //文字列の処理
├ StringUtils.cpp
├ CreditScene.cpp  //クレジット画面
├ CreditScene.hpp
├ Scene1.cpp  //トップ画面
├ Scene1.hpp 
├ Scene2.cpp  //語句入力画面
├ Scene2.hpp
├ Scene3.cpp  //覚え歌生成画面
├ Scene3.hpp
├ Scene4.cpp  //再生画面
├ Scene4.hpp
├ stdafx.h
├ Main.cpp
└ App/
　├ OriginalScores/　←あらかじめ用意した楽譜がここ
　│　└ 幸せなら手をたたこう.json
　│　└ チューリップ.json
　│　└	…
　├ Instruments/　←あらかじめ用意した伴奏がここ
　│　└ 幸せなら手をたたこう.wav
　│　└ チューリップ.wav
　│　└	…
　├ lyrics/　←ユーザーが入力した語句がここ
　│　└ 語句1.csv
　│　└ 語句2.csv
　│　└	…
　├ CreatedScores/　←プログラムで作成した覚え歌の楽譜がここ
　│　└ 語句1-幸せなら手をたたこう.json
　│　└ 語句2-チューリップ.json
　│　└	…
　├ Voice/　←プログラムで作成した歌声がここ
　│　└ 語句1-幸せなら手をたたこう-ずんだもん.wav
　│　└ 語句2-チューリップ-春日部つむぎ.wav
　│　└	…
　└ 覚え歌つくるくん.exe

<更新情報>
2024.08.31 : CreditScene.cpp, CreditScene.hpp を追加
2024.09.22 : 
  CreateScore.cpp, CreateScore.hpp を削除
  Note.hpp, JSONHandler.cpp, JSONHandler.cpp, LyricsProcessor.cpp, LyricsProcessor.cpp. StringUtils.cpp, StringUtils.cpp,を追加
  構成図にコメント文を追加
</pre>
