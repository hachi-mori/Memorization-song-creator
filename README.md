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
