#include "LyricsProcessor.hpp"

// ProcessLyrics 関数の実装
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

// HandleMoreMoraThanNotes 関数の実装
void HandleMoreMoraThanNotes(Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// モーラ数を音符数に合わせるために、余分なモーラを削除
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

	// 必要に応じて、モーラの調整ロジックを追加
}

// HandleMoreNotesThanMora 関数の実装
void HandleMoreNotesThanMora(const Array<String>& moraList, Array<Note>& notes)
{
	size_t noteCount = notes.size();
	size_t moraCount = moraList.size();

	// モーラ数が少ない場合、余分な音符に空の歌詞を割り当て
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

	// 必要に応じて、音符の調整ロジックを追加
}
