// LyricsProcessor.hpp
#pragma once

#include "stdafx.h"

// 歌詞処理関連の関数宣言
int ProcessLyrics(const JSON& json, const Array<Array<String>>& originalLyricList, Array<Array<Note>>& phrases);
void HandleMoreMoraThanNotes(const JSON& json, Array<String>& moraList, Array<Note>& notes);
void HandleMoreNotesThanMora(const Array<String>& moraList, Array<Note>& notes);
bool AdjustMoraAndNotes(const JSON& json, Array<String>& moraList, Array<Note>& notes);

