#pragma once
#include "stdafx.h"

namespace SasaGUI
{
	namespace Config
	{
		struct ScrollBar
		{
			// |          ______________     |
			// |  =======|______________|==  |
			// |                             |
			//    ^---------Track---------^
			//           ^-----Thumb----^

			constexpr static ColorF BackgroundColor{ 1, 0 };
			constexpr static ColorF TrackColor{ 0, 0 };
			constexpr static ColorF ThumbColor{ 0.4, 0.7 };
			constexpr static ColorF HoveredBackgroundColor{ 1, 1 };
			constexpr static ColorF HoveredTrackColor{ 0, 0.1 };
			constexpr static ColorF HoveredThumbColor{ 0.4, 1 };

			constexpr static int32 TrackMargin = 3;
			constexpr static int32 ThumbThickness = 6;
			constexpr static int32 ThumbMinLength = 20;

			constexpr static Duration ScrollSmoothTime = 0.1s;
			constexpr static Duration LargeScrollDelay = 0.8s;
			constexpr static Duration LargeScrollInterval = 0.1s;
			constexpr static Duration FadeInDuration = 0.06s;
			constexpr static Duration FadeOutDuration = 0.5s;
			constexpr static Duration FadeOutDelay = 2s;
		};
	}

	class Delay
	{
	public:

		constexpr Delay(Duration duration) noexcept
			: m_duration(duration.count())
			, m_time(m_duration)
		{
			assert(duration >= 0s);
		}

		bool update(bool in, double d = Scene::DeltaTime()) noexcept
		{
			if (in)
			{
				m_time = 0.0;
			}
			else
			{
				m_time += d;
			}
			return in || m_time <= m_duration;
		}

	private:

		double m_duration;

		double m_time;
	};

	struct Repeat
	{
	public:

		Repeat(Duration interval, Duration delay = 0s)
			: m_interval(interval.count())
			, m_delay(delay.count())
		{
			assert(interval > 0s);
			assert(delay >= 0s);
		}

		bool update(bool in, double d = Scene::DeltaTime()) noexcept
		{
			if (not in)
			{
				m_first = true;
				m_delayed = false;
				return false;
			}

			bool out = false;

			if (m_first)
			{
				out = true;
				m_accumulation = 0.0;
				m_first = false;
			}

			m_accumulation += d;

			if (not m_delayed)
			{
				if (m_accumulation < m_delay)
				{
					return out;
				}

				out = true;
				m_accumulation -= m_delay;
				m_delayed = true;
			}

			double count = std::floor(m_accumulation / m_interval);
			out |= count > 0.0;
			m_accumulation -= count * m_interval;

			return out;
		}

	private:

		double m_interval;

		double m_delay;

		bool m_first = true;

		bool m_delayed = false;

		double m_accumulation = 0.0;
	};

	enum class Orientation
	{
		Horizontal, Vertical
	};

	class ScrollBar
	{
	public:

		using Config = Config::ScrollBar;

		constexpr static int32 Thickness = Config::TrackMargin * 2 + Config::ThumbThickness;
		constexpr static int32 MinLength = Config::TrackMargin * 2 + Config::ThumbMinLength + 10;
		constexpr static double ThumbRoundness = Config::ThumbThickness * 0.5;

		ScrollBar(Orientation orientation)
			: orientation(orientation)
		{
			updateLayout({ 0, 0, 0, 0 });
		}

	public:

		const Orientation orientation;

		Rect rect() const { return m_rect; }

		double minimum() const { return m_minimum; }

		double maximum() const { return m_maximum; }

		double value() const { return m_value; }

		double viewportSize() const { return m_viewportSize; }

		void updateLayout(Rect rect);

		void updateConstraints(double minimum, double maximum, double viewportSize);

		void show();

		void scroll(double delta, bool teleport = false);

		void moveTo(double value);

		void update(Optional<Vec2> cursorXYPos = Cursor::PosF(), double deltaTime = Scene::DeltaTime());

		void draw() const;

	private:

		// 制限&スクロール位置

		double m_minimum = 0.0;

		double m_maximum = 1.0;

		double m_value = 0.0;

		double m_viewportSize = 1.0;

		// レイアウト

		Rect m_rect;

		double m_trackLength;

		double m_thumbLength = 0.0; // 表示しないとき0

		// 状態

		bool m_thumbGrabbed = false;

		bool m_trackPressed = false;

		// スクロール状態

		double m_scrollTarget = 0.0;

		double m_scrollVelocity = 0.0;

		int32 m_largeScrollDirection = 0;

		double m_largeScrollTargetPos = 0.0;

		// アニメーション関連

		Repeat m_largeScrollTimer{ Config::LargeScrollInterval, Config::LargeScrollDelay };

		Transition m_colorTransition{ Config::FadeInDuration, Config::FadeOutDuration };

		Delay m_colorTransitionDelay{ Config::FadeOutDelay };

		// その他

		Optional<double> prevCursorPos;

		void updateThumbLength();

		Rect getTrackRect() const { return m_rect.stretched(-Config::TrackMargin); }

		inline double calculateThumbPos(double value) const
		{
			return (m_trackLength - m_thumbLength) * value / (m_maximum - m_minimum - m_viewportSize);
		}

		RectF getThumbRect() const;

		inline double getMainAxisValue(Vec2 input) const
		{
			switch (orientation)
			{
			case Orientation::Horizontal: return input.x;
			case Orientation::Vertical: return input.y;
			}
		}
	};
}
