// VoiceVoxSynthesizer.cpp
#include "VoiceVoxSynthesizer.hpp"

namespace s3d
{
	namespace VOICEVOX
	{
		[[nodiscard]]
		Array<Speaker> GetSpeakers(const Duration timeout)
		{
			constexpr URLView url = U"http://localhost:50021/speakers";

			AsyncHTTPTask task = SimpleHTTP::GetAsync(url, {});

			Stopwatch stopwatch{ StartImmediately::Yes };

			while (not task.isReady())
			{
				if (timeout <= stopwatch)
				{
					task.cancel();
					return{};
				}

				System::Sleep(1ms);
			}

			if (not task.getResponse().isOK())
			{
				return{};
			}

			const JSON json = task.getAsJSON();
			Array<Speaker> speakers;

			for (auto&& [i, speaker] : json)
			{
				Speaker s;
				s.name = speaker[U"name"].get<String>();

				for (auto&& [k, style] : speaker[U"styles"])
				{
					Speaker::Style st;
					st.name = style[U"name"].get<String>();
					st.id = style[U"id"].get<int32>();
					s.styles.push_back(st);
				}

				speakers.push_back(s);
			}

			return speakers;
		}

		[[nodiscard]]
		bool SynthesizeFromJSONFile(const FilePath jsonFilePath, const FilePath savePath, const URL& synthesisURL, const Duration timeout)
		{
			// Read the JSON file
			const JSON query = JSON::Load(jsonFilePath);

			if (not query)
			{
				Print(U"Failed to load JSON file.");
				return false;
			}

			// Format JSON data as UTF-8
			const std::string data = query.formatUTF8Minimum();
			const HashTable<String, String> headers{ { U"Content-Type", U"application/json" } };

			// Send asynchronous POST request
			AsyncHTTPTask task = SimpleHTTP::PostAsync(synthesisURL, headers, data.data(), data.size(), savePath);
			Stopwatch stopwatch{ StartImmediately::Yes };

			// Wait for the request to complete or timeout
			while (not task.isReady())
			{
				if (timeout <= stopwatch)
				{
					task.cancel();

					if (FileSystem::IsFile(savePath))
					{
						FileSystem::Remove(savePath);
					}

					Print(U"Request timed out.");
					return false;
				}

				System::Sleep(1ms);
			}

			// Check if the response is successful
			if (not task.getResponse().isOK())
			{
				if (FileSystem::IsFile(savePath))
				{
					FileSystem::Remove(savePath);
				}

				Print(U"Request failed.");
				return false;
			}

			Print(U"File saved to: " + savePath);
			return true;
		}

		[[nodiscard]]
		bool SynthesizeVoiceFromScore(const FilePath scoreFilePath, const FilePath outputAudioPath, const int32 speakerID, const Duration timeout)
		{
			const FilePath singQueryFilePath = U"SingQuery.json";

			// Set URLs for VOICEVOX synthesis
			const URL singFrameAudioQueryURL = U"http://localhost:50021/sing_frame_audio_query?speaker=6000";
			const URL frameSynthesisURL = U"http://localhost:50021/frame_synthesis?speaker={}"_fmt(speakerID);

			// Generate the SingQuery JSON file from ScoreQuery
			if (not SynthesizeFromJSONFile(scoreFilePath, singQueryFilePath, singFrameAudioQueryURL, timeout))
			{
				Print(U"Failed to create SingQuery.");
				return false;
			}

			// Generate the audio file from SingQuery
			if (not SynthesizeFromJSONFile(singQueryFilePath, outputAudioPath, frameSynthesisURL, timeout))
			{
				Print(U"Failed to synthesize audio.");
				return false;
			}

			Print(U"Voice synthesis succeeded.");
			return true;
		}
	}
}
