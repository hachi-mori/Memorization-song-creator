//Gitのテスト


#include "stdafx.h"

using App = SceneManager<String>;

void Main()
{
	Window::SetTitle(U"覚え歌つくるくん");
	Window::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetFullscreen(true);
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	App manager;

	manager.add<Scene1>(U"Scene1");
	manager.add<Scene2>(U"Scene2");
	manager.add<Scene3>(U"Scene3");
	manager.add<Scene4>(U"Scene4");
	manager.add<CreditScene>(U"CreditScene");
	// 他のシーンも同様に登録する

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
