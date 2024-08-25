#include "stdafx.h"

bool Button(const Rect& rect, const Font& font, const String& text, bool enabled)
{
	if (enabled && rect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (enabled)
	{
		rect.draw(ColorF{ 0.3, 0.7, 1.0 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2));
	}
	else
	{
		rect.draw(ColorF{ 0.5 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2), ColorF{ 0.7 });
	}

	return (enabled && rect.leftClicked());
}
