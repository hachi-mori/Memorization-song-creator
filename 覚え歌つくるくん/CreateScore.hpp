#pragma once

#include "stdafx.h"

// Noteクラスの定義
class Note
{
public:
	String lyric;         // 歌詞
	size_t frame_length; // フレームの長さ
	std::optional<size_t> key; // 音のキー
	String notelen;      // 音符の長さ
};

// 関数の宣言
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath);

// フレーズごとにモーラを音符に割り当てる処理
void ProcessLyrics(const Array<Array<String>>& lyricList, Array<Array<Note>>& phrases);

// モーラ数が音符数を超えた場合の処理
void HandleMoreMoraThanNotes(Array<Array<String>>& lyricList, Array<Note>& notes);

// 音符数がモーラ数を超えた場合の処理
void HandleMoreNotesThanMora(const Array<Array<String>>& moraList, Array<Note>& notes);

// 文字列をモーラ単位に分割する関数
Array<String> SplitToMora(const String& input);

// ひらがな/カタカナから母音を取得する関数
String GetVowelFromKana(const String& kana);

Array<Array<Note>> DeterminePhrasesFromJSON(const JSON& json);
