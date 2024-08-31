#include "CreateScore.hpp"

void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath)
{
	// CSV ファイルからデータを読み込む
	const CSV csv(csvPath);

	// 2列目のデータを格納する配列
	Array<String> lyricList;

	// CSVの全行を走査し、2列目のデータを取得
	for (size_t row = 0; row < csv.rows(); ++row)
	{
		const String& word = csv[row][1];
		// データが空でない場合にのみ処理
		if (!word.isEmpty())
		{
			// 文字列をモーラ単位に分割
			Array<String> moraList = SplitToMora(word);
			// 各モーラをリストに追加
			lyricList.append(moraList);
		}
	}

	// JSON ファイルを読み込む
	JSON json = JSON::Load(jsonPath);
	Print(U"lyricList.size(): ", lyricList.size());

	// JSON の "notes" を更新
	for (size_t i = 1; i < json[U"notes"].size(); ++i)
	{
		if (i - 1 < lyricList.size())
		{
			json[U"notes"][i][U"lyric"] = lyricList[i - 1];
		}
		else
		{
			json[U"notes"][i][U"lyric"] = U"ら"; // デフォルト値「ら」を設定
		}

		// デバッグ出力: 実際に何が代入されたか確認
		Print(U"json[U\"notes\"][", i, U"][U\"lyric\"] = ", json[U"notes"][i][U"lyric"]);
	}

	// 編集したJSONを保存
	json.save(outputPath);
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

		// 次の文字が小文字のカタカナ/ひらがなか、または「っ」なら、続けて1モーラとする
		if (i + 1 < input.size() && (U"ぁぃぅぇぉゃゅょァィゥェォャュョっ"_s.includes(input[i + 1])))
		{
			mora += input[i + 1];
			++i;
		}

		moraList << mora;
		++i;
	}

	return moraList;
}

