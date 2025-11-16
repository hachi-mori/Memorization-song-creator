# 「覚え歌つくるくん」リポジトリ

「覚え歌つくるくん」ソフトのプログラム一覧  
→[覚え歌つくるくん](/覚え歌つくるくん)

ソフトに同封する楽譜を生成するために使用するプログラム  
→[XmlToJsonConverter.cpp](XmlToJsonConverter.cpp)

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
2025.11.16 : リポジトリを現リポジトリへ移動
</pre>
