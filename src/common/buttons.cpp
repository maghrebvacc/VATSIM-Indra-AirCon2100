#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>
#pragma comment(lib, "gdiplus.lib")

#include "EuroScopePlugIn.h"
#include "TopSkyFunctions.h"
#include <functional>

int shadowSize = 2;
using namespace Gdiplus;


void DrawButton(HDC hDC, int LocationX, int LocationY, int Width, int Height, std::string text, int IsActive,int IsMessage, std::function<void()> onActive) {
    Graphics graphics(hDC);
    if (IsActive) {
        Gdiplus::Color light(255, 176, 176, 176);
        Gdiplus::Color dark(255, 58, 58, 58);

        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 153, 153, 153));
        graphics.FillRectangle(&brush, LocationX, LocationY, Width, Height);

        Gdiplus::Pen penTop(dark, 1);
        graphics.DrawLine(&penTop, LocationX, LocationY, LocationX + Width + 1, LocationY);

        Gdiplus::Pen penLeft(dark, 1);
        graphics.DrawLine(&penLeft, LocationX, LocationY, LocationX, LocationY + Height + 1);

        Gdiplus::Pen penBottom(dark, 1);
        graphics.DrawLine(&penBottom, LocationX, LocationY + Height + 1, LocationX + Width + 1, LocationY + Height + 1);

        Gdiplus::Pen penRight(dark, 1);
        graphics.DrawLine(&penRight, LocationX + Width + 1, LocationY, LocationX + Width + 1, LocationY + Height + 1);


        //DRAW TEXT
        Font MainFont(L"Courier New", 13, FontStyleBold);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 0,0,0));
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF layout((Gdiplus::REAL)LocationX,(Gdiplus::REAL)LocationY,(Gdiplus::REAL)Width,(Gdiplus::REAL)Height);

        std::wstring wtext(text.begin(), text.end());

        graphics.DrawString(wtext.c_str(),wtext.length(),&MainFont,layout,&format,&textBrush);
    }
    else {
        Gdiplus::Color light(255, 176, 176, 176);
        Gdiplus::Color dark(255, 58, 58, 58);

        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 77, 77, 77));
        graphics.FillRectangle(&brush, LocationX, LocationY, Width, Height);

        Gdiplus::Pen penTop(light, 1);
        graphics.DrawLine(&penTop,LocationX, LocationY,LocationX + Width - 1, LocationY);

        Gdiplus::Pen penLeft(light, 1);
        graphics.DrawLine(&penLeft,LocationX, LocationY,LocationX, LocationY + Height - 1);

        Gdiplus::Pen penBottom(dark, 1);
        graphics.DrawLine(&penBottom,LocationX, LocationY + Height - 1,LocationX + Width - 1, LocationY + Height - 1);

        Gdiplus::Pen penRight(dark, 1);
        graphics.DrawLine(&penRight,LocationX + Width - 1, LocationY,LocationX + Width - 1, LocationY + Height - 1);

        //DRAW TEXT
        Font MainFont(L"Courier New", 13, FontStyleBold);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 199, 199, 199));
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF layout((Gdiplus::REAL)LocationX,(Gdiplus::REAL)LocationY,(Gdiplus::REAL)Width,(Gdiplus::REAL)Height);

        std::wstring wtext(text.begin(), text.end());

        graphics.DrawString(wtext.c_str(),wtext.length(),&MainFont,layout,&format,&textBrush);
    }
}