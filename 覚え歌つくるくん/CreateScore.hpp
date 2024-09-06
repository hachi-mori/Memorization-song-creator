#pragma once

#include "stdafx.h"


struct Note
{
	String lyric;  // 歌詞を格納するフィールド
};

// 関数の宣言
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath);

void ProcessLyrics(const Array<String>& moraList, Array<Note>& notes);

void HandleMoreMoraThanNotes(Array<String>& moraList, Array<Note>& notes);

void HandleMoreNotesThanMora(const Array<String>& moraList, Array<Note>& notes);

Array<String> SplitToMora(const String& input);

String GetVowelFromKana(const String& kana);
