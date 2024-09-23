// StringUtils.hpp
#pragma once

#include "stdafx.h"

// 文字列処理関連の関数宣言
Array<String> SplitToMoraWithoutLongVowel(const String& input);
void ReplaceLongVowelMarks(Array<Array<Note>>& phrases);
String GetVowelFromKana(const String& kana);
