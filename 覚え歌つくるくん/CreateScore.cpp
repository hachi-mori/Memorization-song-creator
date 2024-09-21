#include "CreateScore.hpp"

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

int ProcessLyrics(const Array<Array<String>>& lyricList, Array<Array<Note>>& phrases)
{
	int totalDifference = 0; // 差分を保存する変数

	// 各フレーズに対して処理
	for (size_t phraseIndex = 0; phraseIndex < phrases.size() - 1; ++phraseIndex)
	{
		if (phraseIndex >= lyricList.size())
		{
			Print << U"Error: No lyrics available for phrase index " << phraseIndex << U"\n";
			break;
		}

		Array<Note>& notes = phrases[phraseIndex + 1];
		const Array<String>& moraList = lyricList[phraseIndex];

		size_t noteCount = notes.size();
		size_t moraCount = moraList.size();

		if (moraCount == noteCount)
		{
			// モーラ数と音符数が同じ場合
			for (size_t i = 0; i < noteCount; ++i)
			{
				notes[i].lyric = moraList[i];
			}
			continue;
		}
		else if (moraCount > noteCount)
		{
			// モーラ数が音符数を超えている場合
			// moraList をコピーして渡す
			Array<String> moraListCopy = moraList;
			HandleMoreMoraThanNotes(moraListCopy, notes);
			totalDifference += static_cast<int>(moraCount - noteCount);
		}
		else // moraCount < noteCount
		{
			// 音符数がモーラ数を超えている場合
			HandleMoreNotesThanMora(moraList, notes);
			totalDifference += static_cast<int>(noteCount - moraCount);
		}
	}

	return totalDifference;
}

// モーラ数が音符数を超えた場合の処理
void HandleMoreMoraThanNotes(Array<String> moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// モーラ数を音符数に合わせるために、余分なモーラを削除または調整します
	// ここでは単純にモーラリストを音符数にトリミングします
	if (moraCount > noteCount)
	{
		moraList.resize(noteCount);
	}

	// 音符にモーラを割り当て
	for (size_t i = 0; i < noteCount; ++i)
	{
		if (i < moraList.size())
		{
			notes[i].lyric = moraList[i];
		}
		else
		{
			notes[i].lyric = U"";
		}
	}

	// 必要に応じて、モーラの調整ロジックを追加してください
}

// 音符数がモーラ数を超えた場合の処理
void HandleMoreNotesThanMora(const Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// モーラ数が少ない場合、余分な音符に適切な処理を行います
	// ここでは単純に空の歌詞を割り当てます
	for (size_t i = 0; i < noteCount; ++i)
	{
		if (i < moraCount)
		{
			notes[i].lyric = moraList[i];
		}
		else
		{
			notes[i].lyric = U"";
		}
	}

	// 必要に応じて、音符の調整ロジックを追加してください
}

// 文字列をモーラ単位に分割する関数
Array<String> SplitToMora(const String& input)
{
	Array<String> moraList;

	size_t i = 0;
	while (i < input.size())
	{
		String mora;

		// 1文字目
		mora += input[i];

		// 次の文字が小文字なら、続けて1モーラとする（「っ」以外）
		if (i + 1 < input.size() && (U"ぁぃぅぇぉゃゅょァィゥェォャュョ"_s.includes(input[i + 1])))
		{
			mora += input[i + 1];
			++i;
		}

		moraList << mora;
		++i;

		// 次の文字が伸ばし棒「ー」なら、前のモーラから母音を取得して置き換える
		if (i < input.size() && input[i] == U'ー')
		{
			// 直前のモーラ（最後に追加されたモーラ）から母音を取得
			String previousMora = moraList.back();
			String vowel = GetVowelFromKana(previousMora);

			// 伸ばし棒は母音に置き換える
			if (!vowel.isEmpty())
			{
				moraList << vowel; // 母音を追加
			}

			++i; // 伸ばし棒をスキップ
		}
	}

	return moraList;
}

// ひらがな/カタカナから母音を取得する関数
String GetVowelFromKana(const String& kana)
{
	// 母音対応表
	static const HashTable<String, String> vowelMap = {
		// あ行
		{U"ア", U"ア"}, {U"イ", U"イ"}, {U"ウ", U"ウ"}, {U"エ", U"エ"}, {U"オ", U"オ"},
		{U"あ", U"あ"}, {U"い", U"い"}, {U"う", U"う"}, {U"え", U"え"}, {U"お", U"お"},
		// か行
		{U"カ", U"ア"}, {U"キ", U"イ"}, {U"ク", U"ウ"}, {U"ケ", U"エ"}, {U"コ", U"オ"},
		{U"か", U"あ"}, {U"き", U"い"}, {U"く", U"う"}, {U"け", U"え"}, {U"こ", U"お"},
		// さ行
		{U"サ", U"ア"}, {U"シ", U"イ"}, {U"ス", U"ウ"}, {U"セ", U"エ"}, {U"ソ", U"オ"},
		{U"さ", U"あ"}, {U"し", U"い"}, {U"す", U"う"}, {U"せ", U"え"}, {U"そ", U"お"},
		// た行
		{U"タ", U"ア"}, {U"チ", U"イ"}, {U"ツ", U"ウ"}, {U"テ", U"エ"}, {U"ト", U"オ"},
		{U"た", U"あ"}, {U"ち", U"い"}, {U"つ", U"う"}, {U"て", U"え"}, {U"と", U"お"},
		// な行
		{U"ナ", U"ア"}, {U"ニ", U"イ"}, {U"ヌ", U"ウ"}, {U"ネ", U"エ"}, {U"ノ", U"オ"},
		{U"な", U"あ"}, {U"に", U"い"}, {U"ぬ", U"う"}, {U"ね", U"え"}, {U"の", U"お"},
		// は行
		{U"ハ", U"ア"}, {U"ヒ", U"イ"}, {U"フ", U"ウ"}, {U"ヘ", U"エ"}, {U"ホ", U"オ"},
		{U"は", U"あ"}, {U"ひ", U"い"}, {U"ふ", U"う"}, {U"へ", U"え"}, {U"ほ", U"お"},
		// ま行
		{U"マ", U"ア"}, {U"ミ", U"イ"}, {U"ム", U"ウ"}, {U"メ", U"エ"}, {U"モ", U"オ"},
		{U"ま", U"あ"}, {U"み", U"い"}, {U"む", U"う"}, {U"め", U"え"}, {U"も", U"お"},
		// や行
		{U"ヤ", U"ア"}, {U"ユ", U"ウ"}, {U"ヨ", U"オ"},
		{U"や", U"あ"}, {U"ゆ", U"う"}, {U"よ", U"お"},
		// ら行
		{U"ラ", U"ア"}, {U"リ", U"イ"}, {U"ル", U"ウ"}, {U"レ", U"エ"}, {U"ロ", U"オ"},
		{U"ら", U"あ"}, {U"り", U"い"}, {U"る", U"う"}, {U"れ", U"え"}, {U"ろ", U"お"},
		// わ行
		{U"ワ", U"ア"}, {U"ヲ", U"オ"}, {U"ン", U"ン"},
		{U"わ", U"あ"}, {U"を", U"お"}, {U"ん", U"ん"},
		// 濁音・半濁音 (が行〜ぱ行)
		{U"ガ", U"ア"}, {U"ギ", U"イ"}, {U"グ", U"ウ"}, {U"ゲ", U"エ"}, {U"ゴ", U"オ"},
		{U"が", U"あ"}, {U"ぎ", U"い"}, {U"ぐ", U"う"}, {U"げ", U"え"}, {U"ご", U"お"},
		{U"ザ", U"ア"}, {U"ジ", U"イ"}, {U"ズ", U"ウ"}, {U"ゼ", U"エ"}, {U"ゾ", U"オ"},
		{U"ざ", U"あ"}, {U"じ", U"い"}, {U"ず", U"う"}, {U"ぜ", U"え"}, {U"ぞ", U"お"},
		{U"ダ", U"ア"}, {U"ヂ", U"イ"}, {U"ヅ", U"ウ"}, {U"デ", U"エ"}, {U"ド", U"オ"},
		{U"だ", U"あ"}, {U"ぢ", U"い"}, {U"づ", U"う"}, {U"で", U"え"}, {U"ど", U"お"},
		{U"バ", U"ア"}, {U"ビ", U"イ"}, {U"ブ", U"ウ"}, {U"ベ", U"エ"}, {U"ボ", U"オ"},
		{U"ば", U"あ"}, {U"び", U"い"}, {U"ぶ", U"う"}, {U"べ", U"え"}, {U"ぼ", U"お"},
		{U"パ", U"ア"}, {U"ピ", U"イ"}, {U"プ", U"ウ"}, {U"ペ", U"エ"}, {U"ポ", U"オ"},
		{U"ぱ", U"あ"}, {U"ぴ", U"い"}, {U"ぷ", U"う"}, {U"ぺ", U"え"}, {U"ぽ", U"お"}
	};

	// 対応する母音があれば返す、なければ空文字
	if (vowelMap.contains(kana))
	{
		return vowelMap.at(kana);
	}

	return U""; // 対応する母音がない場合は空文字を返す
}
