#pragma once
#include "stdafx.h"

namespace s3d
{
	namespace VOICEVOX
	{
		struct Speaker
		{
			String name;

			struct Style
			{
				String name;
				int32 id;
			};

			Array<Style> styles;
		};

		[[nodiscard]]
		Array<Speaker> GetSpeakers(const Duration timeout = SecondsF{ 50.0 });

		[[nodiscard]]
		bool SynthesizeFromJSONFile(const FilePath jsonFilePath, const FilePath savePath, const URL& synthesisURL, const Duration timeout = SecondsF{ 120.0 });

		[[nodiscard]]
		bool SynthesizeVoiceFromScore(const FilePath scoreFilePath, const FilePath outputAudioPath, const int32 speakerID = 3003, const Duration timeout = SecondsF{ 300.0 });
	}
}
