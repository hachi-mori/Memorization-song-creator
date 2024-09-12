#include "CreateScore.hpp"

void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath)
{
	// CSV ファイルからデータを読み込む
	const CSV csv(csvPath);

	// 2列目のデータを格納する配列
	Array<Array<String>> lyricList;

	// CSVの全行を走査し、2列目のデータを取得
	for (size_t row = 0; row < csv.rows(); ++row)
	{
		const String& word = csv[row][1];
		// データが空でない場合にのみ処理
		if (!word.isEmpty())
		{
			// 文字列をモーラ単位に分割し、各単語のモーラリストを作成
			Array<String> moraList = SplitToMora(word);
			// 単語ごとのモーラリストをリストに追加
			lyricList.push_back(moraList);
		}
	}

	// JSON ファイルを読み込む
	JSON json = JSON::Load(jsonPath);

	Array<Array<Note>> phrases = DeterminePhrasesFromJSON(json);
	// フレーズごとの情報を出力
	for (size_t i = 0; i < phrases.size(); ++i)
	{
		Print << U"フレーズ " << i + 1 << U": 音符数 = " << phrases[i].size();
	}
	Print << U"phrases"<< phrases.size() <<U"lyric" << lyricList.size() << U"\n";

	// モーラ数と音符数を比較して適切に処理
	ProcessLyrics(lyricList, phrases);

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
				// JSONのインデックスが範囲を超えた場合のエラー処理
				Print << U"Error: JSON index out of range.\n";
				return;
			}
		}
	}

	// 歌詞をデバッグ出力: 実際に何が代入されたか確認
	for (size_t i = 1; i < json[U"notes"].size(); ++i)
	{
		Print(U"json[U\"notes\"][", i, U"][U\"lyric\"] = ", json[U"notes"][i][U"lyric"].getString());
	}
	
	// 編集したJSONを保存
	json.save(outputPath);
}

// フレーズをJSONから判定し、フレーズごとに音符を分ける関数
Array<Array<Note>> DeterminePhrasesFromJSON(const JSON& json)
{
	Array<Array<Note>> phrases;
	Array<Note> currentPhrase;

	// 前回の歌詞を保存する変数
	String previousLyric;

	for (const auto& noteData : json[U"notes"].arrayView())
	{
		Note note;
		note.lyric = noteData[U"lyric"].getString();
		note.frame_length = noteData[U"frame_length"].get<size_t>();
		note.key = noteData[U"key"].isNull() ? std::nullopt : std::optional<size_t>(noteData[U"key"].get<size_t>());
		note.notelen = noteData[U"notelen"].getString();

		// 最初の音符、もしくは前回と異なる歌詞の場合、フレーズを新しくする
		if (note.lyric != previousLyric)
		{
			// 現在のフレーズを追加して新しいフレーズを開始
			if (!currentPhrase.isEmpty())
			{
				phrases.push_back(currentPhrase);
				currentPhrase.clear();
			}
		}

		// 現在の音符をフレーズに追加
		currentPhrase.push_back(note);

		// 前回の歌詞を更新
		previousLyric = note.lyric;
	}

	// 最後のフレーズを追加
	if (!currentPhrase.isEmpty())
	{
		phrases.push_back(currentPhrase);
	}

	return phrases;
}

// 音符とモーラの数を比較し、歌詞を割り当てる関数
void ProcessLyrics(const Array<Array<String>>& lyricList, Array<Array<Note>>& phrases)
{
	// 各フレーズに対して処理
	for (size_t phraseIndex = 0; phraseIndex < phrases.size() - 1; ++phraseIndex)
	{
		Array<Note>& notes = phrases[phraseIndex+1];
		const Array<String>& moraList = lyricList[phraseIndex];

		/*
		// デバッグ出力: moraList の内容
		Print(U"Phrase ", phraseIndex, U" moraList contents:");
		for (const auto& mora : moraList)
		{
			Print(U"  ", mora);
		}
		*/

		size_t totalMora = 0;
		size_t wordIndex = 0;
		size_t noteCount = notes.size();

		// デバッグ出力: モーラ数と音符数
		Print(U"Phrase Index: ", phraseIndex);
		Print(U"Number of notes: ", noteCount);
		Print(U"Number of moras: ", moraList.size());

		// フレーズのモーラ数と音符数が完全に一致する場合
		if (moraList.size() == noteCount)
		{
			Print(U"aaaaa");
			for (size_t i = 0; i < noteCount; ++i)
			{
				notes[i].lyric = moraList[i];  // 1モーラずつ対応させる
				Print(notes[i].lyric);
			}
			continue;  // 次のフレーズへ
		}

		while (wordIndex < moraList.size())
		{
			const String& currentWordMora = moraList[wordIndex];
			size_t currentWordMoraCount = currentWordMora.size();

			size_t newTotalMora = totalMora + currentWordMoraCount;

			if (newTotalMora > noteCount)
			{
				size_t differenceWithoutAdding = noteCount - totalMora;
				size_t differenceWithAdding = newTotalMora - noteCount;

				if (differenceWithoutAdding < differenceWithAdding)
				{
					break;
				}
			}

			for (size_t i = 0; i < currentWordMoraCount && totalMora < noteCount; ++i)
			{
				// 型が一致していることを確認
				notes[totalMora].lyric = currentWordMora;  // 文字列全体を代入
				totalMora++;
			}

			wordIndex++;
		}
	}
}

// フレーズ内にモーラを割り当てた結果、モーラ数が音符数を超えてしまった場合
void HandleMoreMoraThanNotes(s3d::Array<Array<String>>& lyricList, s3d::Array<Note>& notes)
{
	// この部分にモーラが音符数を超えた場合の処理を追加
}

// 音符数の方が多い場合の処理
void HandleMoreNotesThanMora(const s3d::Array<Array<String>>& moraList, s3d::Array<Note>& notes)
{
	// この部分に音符数がモーラ数を超えた場合の処理を追加
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
