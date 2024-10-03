// JSONHandler.cpp
#include "stdafx.h"

// UpdateJSONFromCSV 関数の実装
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath)
{
	//Print << U"UpdateJSONFromCSV called. Loading CSV and JSON files...\n";

	// CSVファイルからデータを読み込む
	CSV csv(csvPath);
	if (!csv)
	{
		//Print << U"Failed to open CSV file: " << csvPath << U"\n";
		return;
	}

	// JSONファイルを読み込む
	JSON json = JSON::Load(jsonPath);
	if (json.isNull())
	{
		//Print << U"Failed to load JSON file: " << jsonPath << U"\n";
		return;
	}

	Array<Array<String>> lyricList;
	for (size_t row = 0; row < csv.rows(); ++row)
	{
		const String& word = csv[row][1];
		if (!word.isEmpty())
		{
			Array<String> moraList = SplitToMoraWithoutLongVowel(word);
			lyricList.push_back(moraList);
		}
	}

	Array<Array<Note>> phrases = DeterminePhrasesFromJSON(json);

	// jsonを引数に追加してProcessLyricsを呼び出す
	int difference = ProcessLyrics(json, lyricList, phrases);
	//Print << U"ProcessLyrics returned with difference: " << difference << U"\n";

	// ProcessLyrics の後に、伸ばし棒を母音に変換
	ReplaceLongVowelMarks(phrases);

	// 類似度の追加
	json[U"__type"] = U"Difference:" + ToString(difference);

	// JSONファイルを更新
	size_t jsonIndex = 0;
	for (size_t i = 0; i < phrases.size(); ++i)
	{
		for (size_t j = 0; j < phrases[i].size(); ++j)
		{
			if (jsonIndex >= json[U"notes"].size())
			{
				// jsonIndexがjson[U"notes"]の範囲外の場合は新しい要素を追加
				JSON newNote;
				json[U"notes"].push_back(newNote);
				//Print << U"Adding new note to JSON at index: " << jsonIndex << U"\n";
			}

			json[U"notes"][jsonIndex][U"lyric"] = phrases[i][j].lyric;
			json[U"notes"][jsonIndex][U"frame_length"] = phrases[i][j].frame_length;

			if (phrases[i][j].key.has_value())
			{
				json[U"notes"][jsonIndex][U"key"] = phrases[i][j].key.value();
			}
			else
			{
				json[U"notes"][jsonIndex][U"key"] = nullptr;
			}

			json[U"notes"][jsonIndex][U"notelen"] = phrases[i][j].notelen;
			++jsonIndex;
		}
	}

	// JSONファイルを保存
	if (json.save(outputPath))
	{
		//Print << U"JSON saved successfully to " << outputPath << U"\n";
	}
	else
	{
		Print << U"Failed to save JSON to " << outputPath << U"\n";
	}
}

// DeterminePhrasesFromJSON 関数の実装
Array<Array<Note>> DeterminePhrasesFromJSON(const JSON& json)
{
	Array<Array<Note>> phrases;
	Array<Note> currentPhrase;

	String previousLyric;

	for (const auto& noteData : json[U"notes"].arrayView())
	{
		Note note;
		note.lyric = noteData[U"lyric"].getString();
		note.frame_length = noteData[U"frame_length"].get<size_t>();
		note.key = noteData[U"key"].isNull() ? std::nullopt : std::optional<size_t>(noteData[U"key"].get<size_t>());
		note.notelen = noteData[U"notelen"].getString();

		if (note.lyric != previousLyric)
		{
			if (!currentPhrase.isEmpty())
			{
				phrases.push_back(currentPhrase);
				currentPhrase.clear();
			}
		}

		currentPhrase.push_back(note);
		previousLyric = note.lyric;
	}

	if (!currentPhrase.isEmpty())
	{
		phrases.push_back(currentPhrase);
	}

	return phrases;
}
