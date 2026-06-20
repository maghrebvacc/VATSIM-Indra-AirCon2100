#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

#include "EuroScopePlugIn.h"
#include "TopSkyFunctions.h"

using namespace Gdiplus;

void DrawBarMain(HDC hDC)
{
    Graphics graphics(hDC);

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 68, 68, 68));

    graphics.FillRectangle(&brush, 100, 860, 1820, 110);
}

void DrawBarMessage(HDC hDC) {
    Graphics graphics(hDC);

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 68, 68, 68));

    graphics.FillRectangle(&brush, 0, 860, 100, 110);
}

void RemoveBarMessage(HDC hDC) {
    Graphics graphics(hDC);
    graphics.Clear(Color(255, 0, 0, 0));
}