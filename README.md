# ② 高専プロコン2024 自由部門「覚え歌つくるくん」リポジトリ 2

## 構成図
<pre>
覚え歌つくるくん/
├ Button.cpp
├ Button.hpp
├ ScrollBar.cpp
├ ScrollBar.hpp
├ VoiceVoxSynthesizer.cpp
├ VoiceVoxSynthesizer.hpp
├ CreateScore.cpp
├ CreateScore.hpp
├ Scene1.cpp
├ Scene1.hpp
├ Scene2.cpp
├ Scene2.hpp
├ Scene3.cpp
├ Scene3.hpp
├ Scene4.cpp
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
</pre>
