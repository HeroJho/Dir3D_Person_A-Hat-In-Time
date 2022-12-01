#pragma once

#include <fstream>
#include <iostream>
#include <random>

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_BOSS, LEVEL_ENDING, LEVEL_END };
	enum SOUND { SOUND_BGM, SOUND_END }; // �ִ� 32
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
