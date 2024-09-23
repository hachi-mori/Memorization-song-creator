// LyricsProcessor.hpp
#pragma once

#include "stdafx.h"

// 歌詞処理関連の関数宣言
int ProcessLyrics(const Array<Array<String>>& lyricList, Array<Array<Note>>& phrases);
void HandleMoreMoraThanNotes(Array<String>& moraList, Array<Note>& notes);
void HandleMoreNotesThanMora(const Array<String>& moraList, Array<Note>& notes);
void AdjustMoraAndNotes(Array<String>& moraList, Array<Note>& notes);
