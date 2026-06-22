#pragma once
#include <windows.h>
#include <string>
#include <cmath>
#include <cstdio>
#include "EuroScopePlugIn.h"
#include "../../Get/GetFlightPlanData.h"
void DrawViewFlightPlanWindow(HDC hdc,int ox, int oy,EuroScopePlugIn::CFlightPlan fp,EuroScopePlugIn::CRadarTargetPositionData rtPos,double scale,int  modifyView,RECT& outViewRect,RECT& outModifyRect) {
    const COLORREF WINDOW_BG      = RGB(77,  77,  77);
    const COLORREF WINDOW_EDGE    = RGB(150, 150, 150);
    const COLORREF TITLE_BG       = RGB(100, 167, 150);
    const COLORREF TITLE_TEXT     = RGB(22,  43,  38);
    const COLORREF FIELD_FILL     = RGB(190, 190, 190);
    const COLORREF ABLE_FILL      = RGB(190, 190, 190);
    const COLORREF FIELD_TEXT     = RGB(0,   0,   0);
    const COLORREF LABEL_TEXT     = RGB(225, 225, 225);
    const COLORREF BTN_FILL       = RGB(110, 110, 110);
    const COLORREF BTN_LIGHT      = RGB(180, 180, 180);
    const COLORREF BTN_DARK       = RGB(40,  40,  40);
    const COLORREF BULLET_GREY    = RGB(210, 210, 210);
    const COLORREF BULLET_YELLOW  = RGB(255, 255, 0);

    auto X = [&](int v) -> int { return ox + (int)std::lround(v * scale); };
    auto Y = [&](int v) -> int { return oy + (int)std::lround(v * scale); };
    auto S = [&](int v) -> int { return (int)std::lround(v * scale); };

    HFONT fTitle = CreateFontA(-S(9), 0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fLabel = CreateFontA(-S(8), 0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fValue = CreateFontA(-S(9), 0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fBtn   = CreateFontA(-S(8), 0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");


    HPEN   penEdge  = CreatePen(PS_SOLID, 1, WINDOW_EDGE);
    HBRUSH brBack   = CreateSolidBrush(WINDOW_BG);
    HBRUSH brField  = CreateSolidBrush(FIELD_FILL);
    HBRUSH ableField = CreateSolidBrush(ABLE_FILL);
    HBRUSH brTitle  = CreateSolidBrush(TITLE_BG);
    HBRUSH brBtn    = CreateSolidBrush(BTN_FILL);

    HPEN   oldPen   = (HPEN)  SelectObject(hdc, penEdge);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brBack);
    HFONT  oldFont  = (HFONT) SelectObject(hdc, fLabel);
    SetBkMode(hdc, TRANSPARENT);



    // textC - centred horizontally and vertically in a pixel box
    auto textC = [&](int left, int top, int w, int h,
                      HFONT f, COLORREF col, const char* s)
    {
        HFONT prev = (HFONT)SelectObject(hdc, f);
        SetTextColor(hdc, col);
        SetBkMode(hdc, TRANSPARENT);
        SIZE sz = {};
        GetTextExtentPoint32A(hdc, s, (int)strlen(s), &sz);
        int px = left + (w - sz.cx) / 2;
        int py = top  + (h - sz.cy) / 2;
        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        TextOutA(hdc, px, py, s, (int)strlen(s));
        SelectObject(hdc, prev);
    };

    // textL - left-aligned, vertically centred in a pixel box
    auto textL = [&](int left, int top, int h,
                      HFONT f, COLORREF col, const char* s, int padX = 3)
    {
        HFONT prev = (HFONT)SelectObject(hdc, f);
        SetTextColor(hdc, col);
        SetBkMode(hdc, TRANSPARENT);
        SIZE sz = {};
        GetTextExtentPoint32A(hdc, s, (int)strlen(s), &sz);
        int py = top + (h - sz.cy) / 2;
        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        TextOutA(hdc, left + padX, py, s, (int)strlen(s));
        SelectObject(hdc, prev);
    };

    // textAt - raw placement at a design coordinate
    auto textAt = [&](int dx, int dy, HFONT f, COLORREF col, const char* s)
    {
        HFONT prev = (HFONT)SelectObject(hdc, f);
        SetTextColor(hdc, col);
        SetBkMode(hdc, TRANSPARENT);
        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        TextOutA(hdc, X(dx), Y(dy), s, (int)strlen(s));
        SelectObject(hdc, prev);
    };

    // field - small label above a grey data box
    auto field = [&](int dx, int dy, int dw, int dh,
                      const char* label, const char* value,
                      bool centerLabel = false)
    {
        if (label && *label)
        {
            if (centerLabel)
            {
                HFONT prev = (HFONT)SelectObject(hdc, fLabel);
                SIZE sz = {};
                GetTextExtentPoint32A(hdc, label, (int)strlen(label), &sz);
                int lx = X(dx) + (S(dw) - sz.cx) / 2;
                SetTextColor(hdc, LABEL_TEXT);
                SetBkMode(hdc, TRANSPARENT);
                SetTextAlign(hdc, TA_LEFT | TA_TOP);
                TextOutA(hdc, lx, Y(dy), label, (int)strlen(label));
                SelectObject(hdc, prev);
            }
            else
            {
                textAt(dx, dy, fLabel, LABEL_TEXT, label);
            }
        }
        int boxTop = dy + 11;
        SelectObject(hdc, brField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(dx), Y(boxTop), X(dx) + S(dw), Y(boxTop) + S(dh));
        SelectObject(hdc, penEdge);
        if (value && *value)
            textL(X(dx), Y(boxTop), S(dh), fValue, FIELD_TEXT, value);
    };

    auto fieldable = [&](int dx, int dy, int dw, int dh,
              const char* label, const char* value,
              bool centerLabel = false)
    {
        if (label && *label)
        {
            if (centerLabel)
            {
                HFONT prev = (HFONT)SelectObject(hdc, fLabel);
                SIZE sz = {};
                GetTextExtentPoint32A(hdc, label, (int)strlen(label), &sz);
                int lx = X(dx) + (S(dw) - sz.cx) / 2;
                SetTextColor(hdc, LABEL_TEXT);
                SetBkMode(hdc, TRANSPARENT);
                SetTextAlign(hdc, TA_LEFT | TA_TOP);
                TextOutA(hdc, lx, Y(dy), label, (int)strlen(label));
                SelectObject(hdc, prev);
            }
            else
            {
                textAt(dx, dy, fLabel, LABEL_TEXT, label);
            }
        }
        int boxTop = dy + 11;
        SelectObject(hdc, ableField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(dx), Y(boxTop), X(dx) + S(dw), Y(boxTop) + S(dh));
        SelectObject(hdc, penEdge);
        if (value && *value)
            textL(X(dx), Y(boxTop), S(dh), fValue, FIELD_TEXT, value);
    };

    // strip - inline label then grey box on the same row
    auto strip = [&](int dx, int dy, int totalDW, int dh,
                      const char* label, int labelDW, const char* value)
    {
        textL(X(dx), Y(dy), S(dh), fLabel, LABEL_TEXT, label, 0);
        SelectObject(hdc, brField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(dx + labelDW), Y(dy), X(dx) + S(totalDW), Y(dy) + S(dh));
        SelectObject(hdc, penEdge);
        if (value && *value)
            textL(X(dx + labelDW), Y(dy), S(dh), fValue, FIELD_TEXT, value);
    };

    auto stripable = [&](int dx, int dy, int totalDW, int dh,
              const char* label, int labelDW, const char* value)
    {
        textL(X(dx), Y(dy), S(dh), fLabel, LABEL_TEXT, label, 0);
        SelectObject(hdc, ableField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(dx + labelDW), Y(dy), X(dx) + S(totalDW), Y(dy) + S(dh));
        SelectObject(hdc, penEdge);
        if (value && *value)
            textL(X(dx + labelDW), Y(dy), S(dh), fValue, FIELD_TEXT, value);
    };

    // bullet - small diamond shape
    auto bullet = [&](int dx, int dy, bool selected)
    {
        int cx = X(dx);
        int cy = Y(dy);
        int r  = S(3) > 2 ? S(3) : 2;
        POINT d[4] = {
            { cx,     cy - r },
            { cx + r, cy     },
            { cx,     cy + r },
            { cx - r, cy     }
        };
        COLORREF fillCol    = selected ? BULLET_YELLOW : BULLET_GREY;
        COLORREF outlineCol = selected ? RGB(180,140,0) : RGB(120,120,120);
        HBRUSH b  = CreateSolidBrush(fillCol);
        HPEN   po = CreatePen(PS_SOLID, 1, outlineCol);
        SelectObject(hdc, b);
        SelectObject(hdc, po);
        Polygon(hdc, d, 4);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(b);
        DeleteObject(po);
    };

    // navLink - diamond bullet + label; returns bounding screen RECT
    auto navLink = [&](int dx, int dy, int rowDH,
                        const char* label, bool selected = false) -> RECT
    {
        bullet(dx + 4, dy + rowDH / 2, selected);
        textL(X(dx + 10), Y(dy), S(rowDH), fBtn, LABEL_TEXT, label, 0);

        HFONT prev = (HFONT)SelectObject(hdc, fBtn);
        SIZE sz = {};
        GetTextExtentPoint32A(hdc, label, (int)strlen(label), &sz);
        SelectObject(hdc, prev);

        RECT r = {
            X(dx),
            Y(dy),
            X(dx + 10) + sz.cx + S(4),
            Y(dy) + S(rowDH)
        };
        return r;
    };

    // raisedBtn - bevelled push button
    auto raisedBtn = [&](int dx, int dy, int dw, int dh,
                          const char* label, bool focused = false)
    {
        SelectObject(hdc, brBtn);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(dx), Y(dy), X(dx) + S(dw), Y(dy) + S(dh));

        HPEN pl = CreatePen(PS_SOLID, 1, BTN_LIGHT);
        HPEN pd = CreatePen(PS_SOLID, 1, BTN_DARK);
        SelectObject(hdc, pl);
        MoveToEx(hdc, X(dx),         Y(dy)+S(dh)-1, NULL);
        LineTo  (hdc, X(dx),         Y(dy));
        LineTo  (hdc, X(dx)+S(dw)-1, Y(dy));
        SelectObject(hdc, pd);
        LineTo  (hdc, X(dx)+S(dw)-1, Y(dy)+S(dh)-1);
        LineTo  (hdc, X(dx),         Y(dy)+S(dh)-1);
        SelectObject(hdc, oldPen);
        DeleteObject(pl);
        DeleteObject(pd);

        if (focused)
        {
            HPEN pf = CreatePen(PS_SOLID, 1, BTN_LIGHT);
            HGDIOBJ op = SelectObject(hdc, pf);
            HGDIOBJ ob = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, X(dx)+S(3), Y(dy)+S(3), X(dx)+S(dw)-S(3), Y(dy)+S(dh)-S(3));
            SelectObject(hdc, op);
            SelectObject(hdc, ob);
            DeleteObject(pf);
        }
        textC(X(dx), Y(dy), S(dw), S(dh), fBtn, LABEL_TEXT, label);
    };


    // Outer window background
    SelectObject(hdc, brBack);
    SelectObject(hdc, penEdge);
    Rectangle(hdc, X(0), Y(0), X(586), Y(229));

    // Title bar
    SelectObject(hdc, brTitle);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    Rectangle(hdc, X(0), Y(0), X(586), Y(13));
    textC(X(0), Y(0), S(586), S(13), fTitle, TITLE_TEXT, "FP OPERATION");

    // Close box
    SelectObject(hdc, brBack);
    SelectObject(hdc, penEdge);
    Rectangle(hdc, X(573), Y(1), X(584), Y(12));
    textC(X(573), Y(1), S(11), S(11), fLabel, LABEL_TEXT, "x");

    std::string navComStr       = std::string(1, GetNavData(fp));
    std::string squawkStr       = GetSquawk(rtPos);
    std::string rFieldStr       = std::string(1, GetNavData(fp));
    std::string Wake = std::string(1,GetWakeCatagory(fp));

    field(7,   20, 40,  14, "FLIGHT ID",              GetCallsign(fp).c_str());
    field(54,  20, 81,  14, "RADIO CALLSIGN",         "");
    field(142, 20, 15,  14, "NO",                     "01");
    fieldable(162, 20, 25,  14, "A/C",                    GetAircraftType(fp).c_str());
    fieldable(193, 20, 10,  14, "W",                      Wake.c_str());
    fieldable(210, 20, 25,  14, "DEP",                    GetOrigin(fp).c_str());
    fieldable(243, 20, 25,  14, "DEST",                   GetDest(fp).c_str());
    field(275, 20, 106, 14, "NAV/COM",                navComStr.c_str());
    field(388, 20, 15,  14, "RVSM",                   GetRVSM(fp) ? "ON" : "OFF");
    field(411, 20, 106, 14, "SURVEILLANCE EQUIPMENT", "C");
    field(522, 20, 25,  14, "CSSR",                   squawkStr.c_str());
    fieldable(554, 20, 10,  14, "R",                      GetAircraftFPType(fp).c_str());
    field(569, 20, 10,  14, "FT",                     "S");

    std::string route = GetRoute(fp);
    if (route.length() > 78)    { route.resize(78);    route += "..."; }

    std::string orgroute = GetRoute(fp);
    if (orgroute.length() > 90) { orgroute.resize(90); orgroute += "..."; }

    fieldable(7,   47, 40,  13, "SID",       GetSidName(fp).c_str());
    fieldable(55,  47, 439, 13, "FIR ROUTE", route.c_str(), true);
    fieldable(503, 47, 40,  13, "STAR",      GetStar(fp).c_str());

    textAt(261, 69, fLabel, LABEL_TEXT, "CRUISING");
    textAt(354, 69, fLabel, LABEL_TEXT, "ESTIMATE");
    textAt(440, 69, fLabel, LABEL_TEXT, "POS. REPORT");

    int finalLevelFt = GetFinalLevel(fp);
    char levelBuf[16];
    snprintf(levelBuf, sizeof(levelBuf), "F%03d", finalLevelFt / 100);
    std::string cruisingLevelStr = levelBuf;

    int speedKt = GetSpeed(fp);
    char speedBuf[16];
    snprintf(speedBuf, sizeof(speedBuf), "N%04d", speedKt);
    std::string cruisingSpeedStr = speedBuf;

    std::string finalLevelStr = std::to_string(finalLevelFt);

    field(7,   80, 35, 14, "EOBD",      "");
    fieldable(50,  80, 25, 14, "EOBT",      GetEOBT(fp).c_str());
    field(84,  80, 20, 14, "MSG",       "FPL");
    field(113, 80, 25, 14, "CTOT",      "");
    field(146, 80, 20, 14, "ATFCM",     "");
    field(175, 80, 25, 14, "ATD",       GetATD(fp).c_str());
    field(208, 80, 25, 14, "ETA",       "");
    fieldable(242, 80, 30, 14, "SPEED",     cruisingSpeedStr.c_str());
    fieldable(278, 80, 31, 14, "LEVEL",     cruisingLevelStr.c_str());
    field(317, 80, 30, 14, "FIX",       "");
    field(354, 80, 25, 14, "TIME",      "");
    field(385, 80, 30, 14, "LEVEL",     finalLevelStr.c_str());
    field(424, 80, 30, 14, "SPEED",     cruisingSpeedStr.c_str());
    field(460, 80, 30, 14, "FIX",       "");
    field(497, 80, 25, 14, "ETO",       "");
    fieldable(530, 80, 50, 14, "ALT AD(S)", GetAltAD(fp).c_str());

    std::string remarks = GetRemarks(fp);
    if (remarks.length() > 90) { remarks.resize(90); remarks += "..."; }

    stripable(7, 111, 573, 13, "FIELD 18", 39, remarks.c_str());

    field(7,   126, 25,  13, "EAT",       "");
    fieldable(40,  126, 106, 13, "FREE TEXT", "");
    fieldable(160, 126, 30,  13, "CFL",       std::to_string(ClearedAlt(fp)).c_str());
    field(204, 126, 31,  13, "ECL",       cruisingLevelStr.c_str());
    field(348, 126, 25,  13, "S",         "");
    field(382, 126, 40,  13, "REQ",       cruisingLevelStr.c_str());
    field(430, 126, 106, 13, "STS",       "");
    field(545, 126, 35,  13, "MODE S",    "TRUE");

    strip(7, 156, 573, 13, "ORIGINAL ROUTE", 64, orgroute.c_str());

    const int rowH = 10;
    outViewRect   = navLink(7, 178, rowH, "VIEW",   modifyView == 0);
    outModifyRect = navLink(7, 197, rowH, "MODIFY", modifyView == 1);

    raisedBtn(426, 184, 34, 20, "UPDATE", true);
    raisedBtn(464, 184, 34, 20, "CANCEL");
    raisedBtn(502, 184, 34, 20, "CLEAR");
    raisedBtn(540, 184, 34, 20, "PRINT");

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldFont);

    DeleteObject(penEdge);
    DeleteObject(brBack);
    DeleteObject(brField);
    DeleteObject(ableField);  // Fix: was missing, causing GDI handle leak
    DeleteObject(brTitle);
    DeleteObject(brBtn);
    DeleteObject(fTitle);
    DeleteObject(fLabel);
    DeleteObject(fValue);
    DeleteObject(fBtn);
}