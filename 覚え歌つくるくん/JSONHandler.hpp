// JSONHandler.hpp
#pragma once

#include "stdafx.h"

// JSON処理関連の関数宣言
void UpdateJSONFromCSV(const FilePath& csvPath, const FilePath& jsonPath, const FilePath& outputPath);
Array<Array<Note>> DeterminePhrasesFromJSON(const JSON& json);
