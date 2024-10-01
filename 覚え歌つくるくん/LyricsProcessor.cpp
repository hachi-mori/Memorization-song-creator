#include "LyricsProcessor.hpp"

// ProcessLyrics 関数の実装
int ProcessLyrics(const JSON& json, const Array<Array<String>>& originalLyricList, Array<Array<Note>>& phrases)
{
	int totalDifference = 0; // 差分を保存する変数

	size_t lyricIndex = 0; // 単語リストの現在位置

	// lyricListをコピーしておく（lyricListを変更するため）
	Array<Array<String>> lyricList = originalLyricList;

	// 最初のフレーズを飛ばすために、phraseIndexを1から開始
	size_t phraseIndex = 1;
	size_t maxAppendTimes = 10; // 最大で楽譜を10回まで複製
	size_t appendCount = 0;

	while (lyricIndex < lyricList.size())
	{
		// フレーズが足りなくなった場合、楽譜を後ろにコピーして追加
		if (phraseIndex >= phrases.size())
		{
			if (appendCount >= maxAppendTimes)
			{
				Print << U"Error: Reached maximum number of appends. Unable to process all lyrics.\n";
				break;
			}

			// phrasesのフレーズを後ろにコピー
			Array<Array<Note>> newPhrases;
			for (size_t i = 1; i < phrases.size(); ++i)
			{
				// ノートをディープコピー
				Array<Note> newNotes;
				for (const Note& note : phrases[i])
				{
					newNotes.push_back(note);
				}
				newPhrases.push_back(newNotes);
			}
			phrases.append(newPhrases);
			appendCount++;
			Print << U"Appended phrases. New total phrases: " << phrases.size() << U"\n";
		}

		Array<Note>& notes = phrases[phraseIndex];
		size_t noteCount = notes.size();

		// 単語リストが終了している場合はループを抜ける
		if (lyricIndex >= lyricList.size())
		{
			Print << U"All lyrics have been assigned.\n";
			break;
		}

		bool adjusted = false;

		while (!adjusted)
		{
			// 最適な単語数を見つけるための変数を初期化
			size_t bestK = 0;
			size_t minimalAbsDiff = std::numeric_limits<size_t>::max();
			size_t totalMoraCount = 0;

			// 最大で残りの単語数までループ
			size_t maxWords = lyricList.size() - lyricIndex;

			totalMoraCount = 0;

			for (size_t k = 1; k <= maxWords; ++k)
			{
				// k番目の単語のモーラ数を取得し、合計に加算
				size_t wordMoraCount = lyricList[lyricIndex + k - 1].size();
				totalMoraCount += wordMoraCount;

				size_t absDiff = std::abs(static_cast<int>(totalMoraCount) - static_cast<int>(noteCount));

				if (absDiff < minimalAbsDiff)
				{
					minimalAbsDiff = absDiff;
					bestK = k;
				}
				else if (absDiff == minimalAbsDiff)
				{
					// 差が同じ場合はより多い単語数を選択するため、ループを続行
					minimalAbsDiff = absDiff;
					bestK = k; // より多い単語数に更新
					// ループを続けてさらに最適な解を探す
				}
				else
				{
					// 差が増加したらこれ以上ループする必要はない
					break;
				}
			}

			// 選択した単語がない場合はエラー
			if (bestK == 0)
			{
				Print << U"Error: Unable to find suitable lyrics for phrase index " << phraseIndex << U"\n";
				break;
			}

			// 選択した単語からモーラリストを作成
			Array<String> moraList;

			for (size_t i = 0; i < bestK; ++i)
			{
				const Array<String>& wordMoras = lyricList[lyricIndex + i];
				moraList.append(wordMoras); // モーラを追加
			}

			// フレーズごとの元のモーラ数を記録
			size_t originalMoraCount = moraList.size();

			// モーラと音符の調整を行う関数を呼び出す（ここで json を渡す）
			Array<String> moraListCopy = moraList; // 後で再試行するためにコピーしておく
			Array<Note> notesCopy = notes; // notesもコピーしておく

			bool success = AdjustMoraAndNotes(json, moraList, notes);

			if (success)
			{
				// フレーズごとの差の絶対値を計算し、totalDifferenceに加算
				int difference = static_cast<int>(originalMoraCount) - static_cast<int>(noteCount);
				totalDifference += std::abs(difference);

				// lyricIndex を更新
				lyricIndex += bestK;

				adjusted = true; // 調整成功

				break; // whileループを抜けて次のフレーズへ
			}
			else
			{
				// 調整失敗の場合、単語を分割して再試行
				// 最後の単語を分割する
				size_t lastWordIndex = lyricIndex + bestK - 1;
				Array<String>& lastWordMoras = lyricList[lastWordIndex];

				// フレーズに収まるモーラ数を計算
				size_t neededMoras = notes.size();
				size_t totalMorasSoFar = 0;
				for (size_t i = 0; i < bestK - 1; ++i)
				{
					totalMorasSoFar += lyricList[lyricIndex + i].size();
				}

				size_t morasNeededFromLastWord = neededMoras - totalMorasSoFar;

				// モーラ数が足りない場合はエラー
				if (morasNeededFromLastWord == 0 || morasNeededFromLastWord >= lastWordMoras.size())
				{
					Print << U"Error: Unable to split word at lyric index " << lastWordIndex << U"\n";
					break;
				}

				// 単語を分割
				Array<String> firstPart(lastWordMoras.begin(), lastWordMoras.begin() + morasNeededFromLastWord);
				Array<String> secondPart(lastWordMoras.begin() + morasNeededFromLastWord, lastWordMoras.end());

				// lyricListを更新
				lyricList[lastWordIndex] = firstPart;
				lyricList.insert(lyricList.begin() + lastWordIndex + 1, secondPart);

				// moraListとnotesを元に戻す
				moraList = moraListCopy;
				notes = notesCopy;

				// 再試行するためにcontinue
				continue;
			}
		}

		phraseIndex++;
	}

	// 歌詞の割り当てが完了した後の余分な音符やフレーズを削除する

	// 最後に処理したフレーズのインデックス
	size_t lastProcessedPhraseIndex = phraseIndex - 1;

	// 最後に処理したフレーズの余分な音符を削除
	if (lastProcessedPhraseIndex >= 1 && lastProcessedPhraseIndex < phrases.size())
	{
		Array<Note>& lastProcessedNotes = phrases[lastProcessedPhraseIndex];

		// 歌詞が割り当てられていない音符を削除
		size_t writeIndex = 0;
		for (size_t i = 0; i < lastProcessedNotes.size(); ++i)
		{
			if (!lastProcessedNotes[i].lyric.isEmpty())
			{
				lastProcessedNotes[writeIndex++] = lastProcessedNotes[i];
			}
		}
		lastProcessedNotes.resize(writeIndex);
	}

	// 残りのフレーズを削除
	if (phraseIndex < phrases.size())
	{
		phrases.erase(phrases.begin() + phraseIndex, phrases.end());
	}

	// 最後まで実行されたことを確認するためのメッセージ
	Print << U"ProcessLyrics has completed successfully.\n";

	return totalDifference;
}

// HandleMoreMoraThanNotes 関数の実装
void HandleMoreMoraThanNotes(const JSON& json, Array<String>& moraList, Array<Note>& notes)
{
	// 16分音符のframe_lengthをJSONから読み取る
	int frameLength16th = json[U"16thnoteframe_length"].get<int>();
	Print << U"16th note frame length: " << frameLength16th << U"\n";

	// デバッグ用メッセージ
	Print << U"Before moraList.size(): " << moraList.size() << U", notes.size(): " << notes.size() << U"\n";

	// 促音「っ」や長音「ー」を削除し、モーラ数を調整
	while (moraList.size() > notes.size())
	{
		bool foundSpecialCase = false;

		// 最長の音符を見つける
		size_t longestNoteIndex = 0;
		int maxFrameLength = 0;

		for (size_t i = 0; i < notes.size(); ++i)
		{
			if (notes[i].frame_length > maxFrameLength)
			{
				maxFrameLength = notes[i].frame_length;
				longestNoteIndex = i;
			}
		}

		// 「ん」があるかを確認し、その1つ前のモーラに対応する音符を優先して分割
		for (size_t i = 1; i < moraList.size(); ++i)
		{
			if (moraList[i] == U"ん" || moraList[i] == U"ン")
			{
				// 「ん」の1つ前の音符が最長の音符より前にあるか確認
				size_t targetNoteIndex = i - 1;
				if (targetNoteIndex >= longestNoteIndex)
				{
					Print << U"Skipping 'ん' at moraList index: " << i << U" because it is not before the longest note.\n";
					continue;  // 最長の音符より後にある場合はスキップ
				}

				// すでに「ん」に16分音符の長さが割り当てられているか確認
				if (notes[i].frame_length == frameLength16th)
				{
					Print << U"Skipping 'ん' at moraList index: " << i << U" because it already has a 16th note frame length.\n";
					continue;
				}

				// 分割するかどうかのチェック（分割する音符が16分音符の長さを考慮して分割可能かどうか）
				if (notes[targetNoteIndex].frame_length >= frameLength16th * 2)
				{
					foundSpecialCase = true;
					Print << U"Found mora 'ん' at moraList index: " << i << U", splitting note before it at index: " << targetNoteIndex << U"\n";

					// 分割する音符の長さと分割後の長さを計算
					Note& targetNote = notes[targetNoteIndex];
					int originalFrameLength = targetNote.frame_length;
					int lengthForN = frameLength16th;  // 「ん」に割り当てる長さ
					int lengthForPreviousMora = originalFrameLength - lengthForN;  // 残りの長さ

					// 音符が16分音符の長さ以上であるか確認
					if (lengthForPreviousMora >= frameLength16th)
					{
						// 新しい音符を作成し、「ん」に割り当てる
						Note nNote;
						nNote.frame_length = lengthForN;
						nNote.key = targetNote.key;
						nNote.notelen = targetNote.notelen;
						nNote.lyric = U"ん";  // 「ん」に割り当て

						// 元の音符を短くして「れ」に割り当てる
						targetNote.frame_length = lengthForPreviousMora;

						// 「ん」の音符を挿入
						notes.insert(notes.begin() + targetNoteIndex + 1, nNote);

						Print << U"Splitting note at index: " << targetNoteIndex << U", frame_length for 'れ': " << lengthForPreviousMora << U", frame_length for 'ん': " << lengthForN << U"\n";
					}
				}
				break;  // 「ん」が見つかったら処理を終了
			}
		}

		// 「ん」が見つからなかった場合、最長の音符を分割する
		if (!foundSpecialCase)
		{
			// 分割するかどうかのチェック（長さが16分音符の2倍以上かどうか）
			if (maxFrameLength < 2 * frameLength16th)
			{
				Print << U"Note at index: " << longestNoteIndex << U" is too short to split. Skipping.\n";
				break;  // 分割できないのでループを抜ける
			}

			// 分割処理が正しく呼ばれているかデバッグ
			Print << U"Splitting longest note at index: " << longestNoteIndex << U", frame_length: " << notes[longestNoteIndex].frame_length << U"\n";

			// 最長の音符を2つに分割
			Note& longestNote = notes[longestNoteIndex];
			int originalFrameLength = longestNote.frame_length;
			int firstHalfLength = originalFrameLength / 2;
			int secondHalfLength = originalFrameLength - firstHalfLength;

			// 新しい音符を作成し、分割したデータを格納する
			Note firstHalfNote;
			firstHalfNote.frame_length = firstHalfLength;
			firstHalfNote.key = longestNote.key;
			firstHalfNote.notelen = longestNote.notelen;

			Note secondHalfNote;
			secondHalfNote.frame_length = secondHalfLength;
			secondHalfNote.key = longestNote.key;
			secondHalfNote.notelen = longestNote.notelen;

			// 分割した2つの音符をモーラリストから対応する歌詞を割り当てる
			firstHalfNote.lyric = moraList[notes.size() - 1];  // 既存の音符に次ぐモーラ
			secondHalfNote.lyric = moraList[notes.size()];     // 新しい音符に次ぐモーラ

			// notes配列に追加（元の音符を削除し、分割した2つの音符を挿入）
			notes.erase(notes.begin() + longestNoteIndex);
			notes.insert(notes.begin() + longestNoteIndex, secondHalfNote);  // 2つ目を先に挿入
			notes.insert(notes.begin() + longestNoteIndex, firstHalfNote);   // 1つ目を後から挿入

			// デバッグ用メッセージ
			Print << U"After splitting, notes.size(): " << notes.size() << U"\n";
		}
	}

	// **モーラと音符の対応付け**
	for (size_t i = 0; i < notes.size(); ++i)
	{
		if (i < moraList.size())
		{
			notes[i].lyric = moraList[i];  // モーラリストの内容を音符に対応させる
		}
		else
		{
			notes[i].lyric = U"";  // 空の歌詞を設定
		}
	}

	// デバッグ用にnotesの内容を再度出力
	Print << U"After adjusting lyrics based on moraList:" << U"\n";
	for (size_t i = 0; i < notes.size(); ++i)
	{
		Print << U"Note " << i << U": frame_length=" << notes[i].frame_length
			<< U", lyric=" << notes[i].lyric
			<< U", key=" << (notes[i].key ? *notes[i].key : -1)
			<< U", notelen=" << notes[i].notelen << U"\n";
	}
}

// HandleMoreNotesThanMora 関数の実装
void HandleMoreNotesThanMora(Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// モーラ数が音符数より少ない場合の処理
	while (moraCount < noteCount)
	{
		// まずは長音「ー」を複製
		for (size_t i = 0; i < moraList.size(); ++i)
		{
			if (moraList[i] == U"ー")
			{
				moraList.insert(moraList.begin() + i, U"ー");
				moraCount++;
				if (moraCount >= noteCount) break;
			}
		}

		// それでもモーラが足りない場合は母音「あ、い、う、え、お」を複製
		const Array<String> vowels = { U"あ", U"い", U"う", U"え", U"お" };

		for (size_t i = 0; i < moraList.size(); ++i)
		{
			if (vowels.includes(moraList[i]))
			{
				moraList.insert(moraList.begin() + i, moraList[i]);
				moraCount++;
				if (moraCount >= noteCount) break;
			}
		}

		// それでもモーラが足りない場合は「ら」で埋める
		while (moraCount < noteCount)
		{
			moraList.push_back(U"っ");
			moraCount++;
		}
	}

	// モーラと音符を対応付ける
	for (size_t i = 0; i < noteCount; ++i)
	{
		notes[i].lyric = (i < moraList.size()) ? moraList[i] : U"";
	}
}

// AdjustMoraAndNotes 関数の実装
bool AdjustMoraAndNotes(const JSON& json, Array<String>& moraList, Array<Note>& notes)
{
	int maxIterations = 100; // ループ回数の上限を設定
	int iterationCount = 0;  // 現在のループ回数

	// モーラ数と音符数が一致するまでループする
	while (moraList.size() != notes.size() && iterationCount < maxIterations)
	{
		size_t moraCount = moraList.size();
		size_t noteCount = notes.size();

		Print << U"Iteration " << iterationCount << U": moraCount=" << moraCount << U", noteCount=" << noteCount << U"\n";

		if (moraCount > noteCount)
		{
			// モーラ数が音符数を超えている場合、モーラを調整する
			HandleMoreMoraThanNotes(json, moraList, notes);
		}
		else if (moraCount < noteCount)
		{
			// 音符数がモーラ数を超えている場合、音符を調整する
			HandleMoreNotesThanMora(moraList, notes);
		}

		// ループカウントを増やす
		iterationCount++;
	}

	// 調整が成功したかを確認
	if (iterationCount >= maxIterations || moraList.size() != notes.size())
	{
		Print << U"Error: Mora and notes could not be adjusted.\n";
		return false;
	}

	// モーラ数と音符数が一致したら、モーラを音符に割り当てる
	for (size_t i = 0; i < notes.size(); ++i)
	{
		notes[i].lyric = moraList[i];
	}

	// デバッグ用に調整後の音符とモーラの対応を出力
	Print << U"After adjusting, moraList and notes size are equal:\n";
	for (size_t i = 0; i < notes.size(); ++i)
	{
		Print << U"Note " << i << U": frame_length=" << notes[i].frame_length
			<< U", lyric=" << notes[i].lyric
			<< U", key=" << (notes[i].key ? *notes[i].key : -1)
			<< U", notelen=" << notes[i].notelen << U"\n";
	}

	return true;
}
