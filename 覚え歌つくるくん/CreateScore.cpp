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
			// 1文字ずつ分割して追加
			for (const auto& ch : word)
			{
				lyricList << String{ String(1, ch) }; // 1文字の文字列を作成
			}
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
