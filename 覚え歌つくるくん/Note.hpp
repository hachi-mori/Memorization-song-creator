// Note.hpp
#pragma once

#include "stdafx.h"

// Noteクラスの定義
class Note
{
public:
	String lyric;                  // 歌詞
	size_t frame_length;           // フレームの長さ
	std::optional<size_t> key;     // 音のキー
	String notelen;                // 音符の長さ
};
