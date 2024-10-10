# include <Siv3D.hpp> // Siv3Dのヘッダーファイル

void Main()
{
	// XMLファイルのパス
	const FilePath xmlFilePath = U"幸せなら手をたたこう.xml";

	// tempoを保存する変数
	double tempo = 0.0;

	// テンポを取得するために XML ファイルを一度読み込む
	{
		TextReader tempoReader(xmlFilePath);
		String line;
		while (tempoReader.readLine(line))
		{
			// <sound tempo= を検出
			if (line.indexOf(U"<sound tempo=") != -1)
			{
				// tempo の値を抽出
				String tempString = line.substr(line.indexOf(U"tempo=") + 6);
				tempString = tempString.trim(); // 前後の空白を削除

				// 先頭と末尾の引用符を削除
				if (tempString[0] == U'"') tempString = tempString.substr(1);
				if (tempString[tempString.length() - 1] == U'"') tempString = tempString.substr(0, tempString.length() - 1);

				tempo = Parse<double>(tempString);
				break; // テンポが取得できたのでループを抜ける
			}
		}
	}

	// 音階とその MIDI 番号を対応させるマップ
	const std::map<String, int> noteToMidi = {
		{ U"C", 0 },
		{ U"C#", 1 }, { U"Db", 1 },
		{ U"D", 2 },
		{ U"D#", 3 }, { U"Eb", 3 },
		{ U"E", 4 },
		{ U"F", 5 },
		{ U"F#", 6 }, { U"Gb", 6 },
		{ U"G", 7 },
		{ U"G#", 8 }, { U"Ab", 8 },
		{ U"A", 9 },
		{ U"A#", 10 }, { U"Bb", 10 },
		{ U"B", 11 }
	};

	// ノート情報を格納するリスト
	Array<struct NoteData> notes;

	// NoteData 構造体の定義
	struct NoteData
	{
		int frame_length;
		Optional<int> key; // 休符の場合は null
		String lyric;
		String notelen;
	};

	// XML ファイルを読み込み、ノート情報を解析してリストに格納
	{
		TextReader reader(xmlFilePath);
		String line;
		bool inNote = false;
		bool foundStep = false, foundOctave = false, foundDuration = false, foundText = false, foundRest = false;
		bool skipFirstRest = true; // 最初の休符をスキップするためのフラグ

		String step;
		int octave = 0;
		int duration = 0;
		String text;

		while (reader.readLine(line))
		{
			// <note> タグを検出
			if (line.indexOf(U"<note") != -1)
			{
				inNote = true;
				foundStep = false;
				foundOctave = false;
				foundDuration = false;
				foundText = false;
				foundRest = false;
				step.clear();
				octave = 0;
				duration = 0;
				text.clear();
			}

			if (inNote)
			{
				// <rest /> タグを検出
				if (line.indexOf(U"<rest") != -1)
				{
					foundRest = true;
				}

				// <step> タグを検出
				if (line.indexOf(U"<step>") != -1)
				{
					step = line.substr(line.indexOf(U"<step>") + 6);
					step = step.substr(0, step.indexOf(U"</step>")).trim();
					foundStep = true;
				}

				// <octave> タグを検出
				if (line.indexOf(U"<octave>") != -1)
				{
					String octaveString = line.substr(line.indexOf(U"<octave>") + 8);
					octaveString = octaveString.substr(0, octaveString.indexOf(U"</octave>")).trim();
					octave = Parse<int>(octaveString);
					foundOctave = true;
				}

				// <duration> タグを検出
				if (line.indexOf(U"<duration>") != -1)
				{
					String durationString = line.substr(line.indexOf(U"<duration>") + 10);
					durationString = durationString.substr(0, durationString.indexOf(U"</duration>")).trim();
					duration = Parse<int>(durationString);
					foundDuration = true;
				}

				// <text> タグを検出
				if (line.indexOf(U"<text>") != -1)
				{
					text = line.substr(line.indexOf(U"<text>") + 6);
					text = text.substr(0, text.indexOf(U"</text>")).trim();
					foundText = true;
				}

				// <note> タグの終わりを検出
				if (line.indexOf(U"</note>") != -1)
				{
					if (foundDuration)
					{
						// フレーム長を計算
						int frame_length = static_cast<int>(std::round((duration / 960.0) / (tempo / 60.0) * 93.75));

						// 最初の休符をスキップ
						if (foundRest && skipFirstRest)
						{
							skipFirstRest = false; // 次回からは休符を処理する
							inNote = false;
							continue; // この休符をスキップ
						}

						NoteData noteData;
						noteData.frame_length = frame_length;

						if (foundRest)
						{
							noteData.key = none; // 休符の場合は null
							noteData.lyric = U"";
							noteData.notelen = U"R";
						}
						else if (foundStep && foundOctave)
						{
							int midiKey = 60 + noteToMidi.at(step) + (octave - 4) * 12;
							noteData.key = midiKey;
							noteData.lyric = text;
							noteData.notelen = step + Format(octave);
						}

						notes.push_back(noteData);
					}
					inNote = false; // <note> タグの終了を検出
				}
			}
		}
	}

	// 最後の歌詞付きの音符のインデックスを特定
	int lastLyricNoteIndex = -1;
	for (size_t i = 0; i < notes.size(); ++i)
	{
		if (notes[i].lyric.isEmpty() == false)
		{
			lastLyricNoteIndex = static_cast<int>(i);
		}
	}

	// 書き込み用のテキストファイルをオープンする
	TextWriter writer{ U"幸せなら手をたたこう.json" };

	// 16thnoteframe_length の計算
	double sixteenthNoteDuration = 0.25 / (tempo / 60.0); // 16分音符の長さ（秒）
	int16 frameLength16th = static_cast<int16>(std::round(sixteenthNoteDuration * 93.75));

	// JSONの最初の部分を書き込む
	writer.writeln(U"{");
	writer.writeln(U"  \"16thnoteframe_length\": ", frameLength16th, U",");
	writer.writeln(U"  \"notes\": [");

	// 最初のダミーノートを書き込む
	writer.writeln(U"    {");
	writer.writeln(U"      \"frame_length\": 2,");
	writer.writeln(U"      \"key\": null,");
	writer.writeln(U"      \"lyric\": \"\",");
	writer.writeln(U"      \"notelen\": \"R\"");
	writer.writeln(U"    },");

	// ノート情報を JSON に出力
	bool firstNote = true;
	for (size_t i = 0; i < notes.size(); ++i)
	{
		// 最後の歌詞付きの音符の後の休符をスキップ
		if (static_cast<int>(i) > lastLyricNoteIndex && notes[i].key.has_value() == false)
		{
			continue;
		}

		// 最初のノート以降の場合、カンマを追加
		if (firstNote)
		{
			firstNote = false;
		}
		else
		{
			writer.writeln(U",");
		}

		writer.writeln(U"    {");
		writer.write(U"      \"frame_length\": ");
		writer.write(notes[i].frame_length);
		writer.write(U",\n      \"key\": ");

		if (notes[i].key.has_value())
		{
			writer.write(notes[i].key.value());
			writer.write(U",\n      \"lyric\": \"");
			writer.write(notes[i].lyric);
			writer.write(U"\",\n      \"notelen\": \"");
			writer.write(notes[i].notelen);
			writer.write(U"\"");
		}
		else
		{
			writer.write(U"null");
			writer.write(U",\n      \"lyric\": \"\",");
			writer.write(U"\n      \"notelen\": \"R\"");
		}
		writer.write(U"\n    }");
	}

	// JSONの最後の部分を書き込む
	writer.writeln(U"\n  ]");
	writer.writeln(U"}");

	// メインループ
	while (System::Update())
	{
		// 何か描画する場合はここに記述
	}
}
