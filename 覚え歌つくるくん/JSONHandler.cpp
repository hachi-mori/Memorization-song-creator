#include "JSONHandler.hpp"
#include "LyricsProcessor.hpp" // 必要に応じてインクルード
#include "StringUtils.hpp"     // 必要に応じてインクルード

// UpdateJSONFromCSV 関数の実装
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath)
{
	// CSVファイルからデータを読み込む
	CSV csv(csvPath);
	if (!csv)
	{
		Print << U"Failed to open CSV file: " << csvPath << U"\n";
		return;
	}

	Array<Array<String>> lyricList;
	for (size_t row = 0; row < csv.rows(); ++row)
	{
		const String& word = csv[row][1];
		if (!word.isEmpty())
		{
			Array<String> moraList = SplitToMora(word);
			lyricList.push_back(moraList);
		}
	}

	// JSONファイルを読み込む
	JSON json = JSON::Load(jsonPath);
	if (json.isNull())
	{
		Print << U"Failed to load JSON file: " << jsonPath << U"\n";
		return;
	}

	Array<Array<Note>> phrases = DeterminePhrasesFromJSON(json);

	int difference = ProcessLyrics(lyricList, phrases);

	json[U"__type"] = U"Difference:" + ToString(difference);

	size_t jsonIndex = 0;
	for (size_t i = 0; i < phrases.size(); ++i)
	{
		for (size_t j = 0; j < phrases[i].size(); ++j)
		{
			if (jsonIndex < json[U"notes"].size())
			{
				json[U"notes"][jsonIndex][U"lyric"] = phrases[i][j].lyric;
				++jsonIndex;
			}
			else
			{
				Print << U"Error: JSON index out of range.\n";
				return;
			}
		}
	}

	for (size_t i = 1; i < json[U"notes"].size(); ++i)
	{
		Print(U"json[U\"notes\"][", i, U"][U\"lyric\"] = ", json[U"notes"][i][U"lyric"].getString());
	}

	json.save(outputPath);
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
