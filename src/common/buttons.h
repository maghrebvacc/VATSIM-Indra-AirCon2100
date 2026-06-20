#pragma once
#include <windows.h>
#include <string>
#include <functional>

void DrawButton(HDC hDC, int LocationX, int LocationY, int Width, int Height, std::string text, int IsActive,int IsMessage,std::function<void()> onActive = nullptr);