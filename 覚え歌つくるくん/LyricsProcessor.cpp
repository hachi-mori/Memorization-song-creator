#include "LyricsProcessor.hpp"

// ProcessLyrics 関数の実装
int ProcessLyrics(const Array<Array<String>>& lyricList, Array<Array<Note>>& phrases)
{
	int totalDifference = 0; // 差分を保存する変数

	size_t lyricIndex = 0; // 単語リストの現在位置

	// 最初のフレーズを飛ばすために、phraseIndexを1から開始
	for (size_t phraseIndex = 1; phraseIndex < phrases.size(); ++phraseIndex)
	{
		Array<Note>& notes = phrases[phraseIndex];
		size_t noteCount = notes.size();

		// 単語リストが終了している場合はループを抜ける
		if (lyricIndex >= lyricList.size())
		{
			Print << U"Error: No more lyrics available for phrase index " << phraseIndex << U"\n";
			break;
		}

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
				// 差が同じ場合はより少ない単語数を選択するため、これ以上ループしない
				break;
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

		// モーラと音符の調整を行う関数を呼び出す
		AdjustMoraAndNotes(moraList, notes);

		// フレーズごとの差の絶対値を計算し、totalDifferenceに加算
		int difference = static_cast<int>(originalMoraCount) - static_cast<int>(noteCount);
		totalDifference += std::abs(difference);

		// lyricIndex を更新
		lyricIndex += bestK;
	}

	return totalDifference;
}




// HandleMoreMoraThanNotes 関数の実装
void HandleMoreMoraThanNotes(Array<String>& moraList, Array<Note>& notes)
{
	// まず、促音「っ」を削除してモーラ数を調整
	while (moraList.size() > notes.size())
	{
		bool sokuonFound = false;

		// モーラを先頭から見て、促音「っ」を探す
		for (size_t i = 0; i < moraList.size(); ++i)
		{
			if (moraList[i] == U"っ" || moraList[i] == U"ッ")
			{
				// 促音「っ」を削除
				moraList.erase(moraList.begin() + i);
				sokuonFound = true;
				break; // 最初に見つけた促音を削除したらループを抜ける
			}
		}

		if (!sokuonFound)
		{
			// 促音「っ」が見つからない場合はループを抜ける
			break;
		}
	}

	// 促音を削除してもまだモーラ数が多い場合、長音符「ー」を削除
	while (moraList.size() > notes.size())
	{
		bool chouonFound = false;

		// モーラを先頭から見て、長音符「ー」を探す
		for (size_t i = 0; i < moraList.size(); ++i)
		{
			if (moraList[i] == U"ー")
			{
				// 長音符「ー」を削除
				moraList.erase(moraList.begin() + i);
				chouonFound = true;
				break; // 最初に見つけた長音符を削除したらループを抜ける
			}
		}

		if (!chouonFound)
		{
			// 長音符「ー」が見つからない場合はループを抜ける
			break;
		}
	}

	// それでもまだモーラ数が多い場合、モーラリストの末尾から削除
	while (moraList.size() > notes.size())
	{
		moraList.pop_back();
	}

	// モーラと音符を対応付ける
	for (size_t i = 0; i < notes.size(); ++i)
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
}


// HandleMoreNotesThanMora 関数の実装
void HandleMoreNotesThanMora(Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// 元の moraList を保存しておく（変更を取り消すために使う）
	Array<String> originalMoraList = moraList;

	// 音符数がモーラ数より多い場合の処理
	while (moraCount < noteCount)
	{
		bool lengthened = false;

		// 1. まず、長音「ー」を複製する
		for (size_t i = 0; i < moraList.size(); ++i)
		{
			if (moraList[i] == U"ー")
			{
				// 前後のモーラが長音でないことを確認
				if ((i == 0 || moraList[i - 1] != U"ー") && (i + 1 >= moraList.size() || moraList[i + 1] != U"ー"))
				{
					// 長音を複製する（1つの長音を2つに増やす）
					moraList.insert(moraList.begin() + i, U"ー");
					moraCount++; // モーラ数を更新

					lengthened = true;
					break; // 1つの長音を2つに増やしたらループを抜ける
				}
			}
		}

		// 2. 長音の処理が終わった後、まだモーラ数が不足している場合、母音「あ、い、う、え、お」を複製する
		if (!lengthened)
		{
			const Array<String> vowels = { U"あ", U"い", U"う", U"え", U"お" };

			for (size_t i = 0; i < moraList.size(); ++i)
			{
				if (vowels.includes(moraList[i]))
				{
					// 前後に同じ母音がないことを確認
					if ((i == 0 || moraList[i - 1] != moraList[i]) && (i + 1 >= moraList.size() || moraList[i + 1] != moraList[i]))
					{
						// 母音を複製する（1つの母音を2つに増やす）
						moraList.insert(moraList.begin() + i, moraList[i]);
						moraCount++; // モーラ数を更新

						lengthened = true;
						break; // 1つの母音を2つに増やしたらループを抜ける
					}
				}
			}
		}

		// 3. 長音と母音の複製を行った後でも、モーラ数が足りない場合
		if (!lengthened)
		{
			// 3-1. 変更を取り消して元の moraList に戻す
			moraList = originalMoraList;
			moraCount = moraList.size();

			// 3-2. モーラ数が音符数に満たない場合は「ら」で埋める
			while (moraCount < noteCount)
			{
				moraList.push_back(U"ら");
				moraCount++;
			}
			break; // 処理を終了
		}
	}

	// モーラと音符の割り当て
	for (size_t i = 0; i < noteCount; ++i)
	{
		if (i < moraList.size())
		{
			notes[i].lyric = moraList[i]; // モーラを音符に割り当て
		}
		else
		{
			notes[i].lyric = U""; // 音符が余っている場合は空にする
		}
	}
}




void AdjustMoraAndNotes(Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	if (moraCount == noteCount)
	{
		// モーラ数と音符数が同じ場合
		for (size_t i = 0; i < noteCount; ++i)
		{
			notes[i].lyric = moraList[i];
		}
	}
	else if (moraCount > noteCount)
	{
		// モーラ数が音符数を超えている場合
		HandleMoreMoraThanNotes(moraList, notes);

		// 調整後のモーラ数と音符数を再取得
		moraCount = moraList.size();

		if (moraCount == noteCount)
		{
			// モーラ数と音符数が一致した場合
			for (size_t i = 0; i < noteCount; ++i)
			{
				notes[i].lyric = moraList[i];
			}
		}
		else if (moraCount > noteCount)
		{
			// まだモーラ数が多い場合、余分なモーラを削除して再度対応付け
			moraList.resize(noteCount);
			for (size_t i = 0; i < noteCount; ++i)
			{
				notes[i].lyric = moraList[i];
			}
		}
		else
		{
			// モーラ数が音符数より少なくなった場合
			HandleMoreNotesThanMora(moraList, notes);
		}
	}
	else // moraCount < noteCount
	{
		// 音符数がモーラ数を超えている場合
		HandleMoreNotesThanMora(moraList, notes);
	}
}
