#pragma once

#include <Siv3D.hpp> // OpenSiv3D v0.6.4

// 関数の宣言
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath);

Array<String> SplitToMora(const String& input);
