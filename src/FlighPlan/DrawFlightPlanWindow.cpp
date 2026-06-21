#define NOMINMAX
#include <windows.h>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>
#include "FlighPlan/GetFlightPlanData.h"

void DrawFlightPlanWindow(HDC hdc,int ox,int oy, EuroScopePlugIn::CFlightPlan fp, EuroScopePlugIn::CRadarTargetPositionData rtPos, double scale)
{
    const COLORREF WINDOW_BG     = RGB(77,  77,  77);
    const COLORREF WINDOW_EDGE   = RGB(150, 150, 150);
    const COLORREF TITLE_BG      = RGB(100, 167, 150);
    const COLORREF TITLE_TEXT    = RGB(22,  43,  38);
    const COLORREF FIELD_FILL    = RGB(190, 190, 190);
    const COLORREF FIELD_TEXT    = RGB(0,  0,  0);
    const COLORREF LABEL_TEXT    = RGB(225, 225, 225);
    const COLORREF BTN_FILL      = RGB(110, 110, 110);
    const COLORREF BTN_LIGHT     = RGB(180, 180, 180);
    const COLORREF BTN_DARK      = RGB(40,  40,  40);
    const COLORREF BULLET_GREY   = RGB(210, 210, 210);
    const COLORREF BULLET_YELLOW = RGB(255, 255, 0);

    auto X = [&](int v) -> int { return ox + (int)std::lround(v * scale); };
    auto Y = [&](int v) -> int { return oy + (int)std::lround(v * scale); };
    auto S = [&](int v) -> int { return (int)std::lround(v * scale); };

    HFONT fTitle = CreateFontA(-S(9),  0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fLabel = CreateFontA(-S(8),  0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fValue = CreateFontA(-S(9),  0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");
    HFONT fBtn   = CreateFontA(-S(8),  0,0,0, FW_BOLD, 0,0,0, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN, "Courier New");

    HPEN   penEdge = CreatePen(PS_SOLID, 1, WINDOW_EDGE);
    HBRUSH brBack  = CreateSolidBrush(WINDOW_BG);
    HBRUSH brField = CreateSolidBrush(FIELD_FILL);
    HBRUSH brTitle = CreateSolidBrush(TITLE_BG);
    HBRUSH brBtn   = CreateSolidBrush(BTN_FILL);

    HPEN   oldPen   = (HPEN)  SelectObject(hdc, penEdge);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brBack);
    HFONT  oldFont  = (HFONT) SelectObject(hdc, fLabel);
    SetBkMode(hdc, TRANSPARENT);


    auto textC = [&](int left, int top, int w, int h,
                      HFONT f, COLORREF col, const char* s)
    {
        // Select font first so metrics reflect it
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

    // textL – draw text left-aligned, vertically centred in a box
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

    // textAt – raw placement, no centering (for small labels above boxes)
    auto textAt = [&](int x, int y, HFONT f, COLORREF col, const char* s)
    {
        HFONT prev = (HFONT)SelectObject(hdc, f);
        SetTextColor(hdc, col);
        SetBkMode(hdc, TRANSPARENT);
        SetTextAlign(hdc, TA_LEFT | TA_TOP);
        TextOutA(hdc, X(x), Y(y), s, (int)strlen(s));
        SelectObject(hdc, prev);
    };


    // field – label above, grey box below, value text vertically centred

    auto field = [&](int x, int y, int w, int h,
                      const char* label, const char* value,
                      bool centerLabel = false)
    {
        // Small label text above the box
        if (label && *label)
        {
            if (centerLabel)
            {
                // Measure label to centre it over the box
                HFONT prev = (HFONT)SelectObject(hdc, fLabel);
                SIZE sz = {};
                GetTextExtentPoint32A(hdc, label, (int)strlen(label), &sz);
                int lx = X(x) + (S(w) - sz.cx) / 2;
                SetTextColor(hdc, LABEL_TEXT);
                SetBkMode(hdc, TRANSPARENT);
                SetTextAlign(hdc, TA_LEFT | TA_TOP);
                TextOutA(hdc, lx, Y(y), label, (int)strlen(label));
                SelectObject(hdc, prev);
            }
            else
            {
                textAt(x, y, fLabel, LABEL_TEXT, label);
            }
        }

        // Box
        int boxTop = y + 11;
        SelectObject(hdc, brField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(x), Y(boxTop), X(x) + S(w), Y(boxTop) + S(h));
        SelectObject(hdc, penEdge);

        // Value inside box
        if (value && *value)
            textL(X(x), Y(boxTop), S(h), fValue, FIELD_TEXT, value);
    };


    // strip – inline label then grey box, all on one row

    auto strip = [&](int x, int y, int totalW, int h,
                      const char* label, int labelW, const char* value)
    {
        // Label centred vertically in the row
        textL(X(x), Y(y), S(h), fLabel, LABEL_TEXT, label, 0);

        // Box
        SelectObject(hdc, brField);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(x + labelW), Y(y), X(x) + S(totalW), Y(y) + S(h));
        SelectObject(hdc, penEdge);

        // Value
        if (value && *value)
            textL(X(x + labelW), Y(y), S(h), fValue, FIELD_TEXT, value);
    };


    // bullet – small diamond (filled + outline)

    auto bullet = [&](int x, int y, bool selected)
    {
        int cx = X(x);
        int cy = Y(y);
        int r  = S(3) > 2 ? S(3) : 2;

        POINT d[4] = {
            { cx,     cy - r },
            { cx + r, cy     },
            { cx,     cy + r },
            { cx - r, cy     }
        };

        COLORREF fillCol    = selected ? BULLET_YELLOW : BULLET_GREY;
        COLORREF outlineCol = selected ? RGB(180,140,0) : RGB(120,120,120);

        HBRUSH b    = CreateSolidBrush(fillCol);
        HPEN   po   = CreatePen(PS_SOLID, 1, outlineCol);
        SelectObject(hdc, b);
        SelectObject(hdc, po);
        Polygon(hdc, d, 4);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(b);
        DeleteObject(po);
    };


    // navLink – diamond bullet + label text on the same baseline

    auto navLink = [&](int x, int y, int rowH,
                        const char* label, bool selected = false)
    {
        // Diamond: centred vertically in the row, sitting just left of the text
        bullet(x + 4, y + rowH / 2, selected);

        // Label: left-aligned, vertically centred in the row
        textL(X(x + 10), Y(y), S(rowH), fBtn, LABEL_TEXT, label, 0);
    };


    // raisedBtn – bevelled push button, label centred

    auto raisedBtn = [&](int x, int y, int w, int h,
                          const char* label, bool focused = false)
    {
        // Fill
        SelectObject(hdc, brBtn);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, X(x), Y(y), X(x) + S(w), Y(y) + S(h));

        // Bevel
        HPEN pl = CreatePen(PS_SOLID, 1, BTN_LIGHT);
        HPEN pd = CreatePen(PS_SOLID, 1, BTN_DARK);
        SelectObject(hdc, pl);
        MoveToEx(hdc, X(x),        Y(y)+S(h)-1, NULL);
        LineTo  (hdc, X(x),        Y(y));
        LineTo  (hdc, X(x)+S(w)-1, Y(y));
        SelectObject(hdc, pd);
        LineTo  (hdc, X(x)+S(w)-1, Y(y)+S(h)-1);
        LineTo  (hdc, X(x),        Y(y)+S(h)-1);
        SelectObject(hdc, oldPen);
        DeleteObject(pl);
        DeleteObject(pd);

        // Focus ring
        if (focused)
        {
            HPEN pf = CreatePen(PS_SOLID, 1, BTN_LIGHT);
            HGDIOBJ op = SelectObject(hdc, pf);
            HGDIOBJ ob = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, X(x)+S(3), Y(y)+S(3), X(x)+S(w)-S(3), Y(y)+S(h)-S(3));
            SelectObject(hdc, op);
            SelectObject(hdc, ob);
            DeleteObject(pf);
        }

        // Label centred in button
        textC(X(x), Y(y), S(w), S(h), fBtn, LABEL_TEXT, label);
    };


    // DRAW  – outer window

    SelectObject(hdc, brBack);
    SelectObject(hdc, penEdge);
    Rectangle(hdc, X(0), Y(0), X(586), Y(229));


    // Title bar

    SelectObject(hdc, brTitle);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    Rectangle(hdc, X(0), Y(0), X(586), Y(13));
    // Title text: centred horizontally and vertically in the 13px bar
    textC(X(0), Y(0), S(586), S(13), fTitle, TITLE_TEXT, "FP OPERATION");

    // Close box
    SelectObject(hdc, brBack);
    SelectObject(hdc, penEdge);
    Rectangle(hdc, X(573), Y(1), X(584), Y(12));
    textC(X(573), Y(1), S(11), S(11), fLabel, LABEL_TEXT, "x");

    std::string aircraftTypeStr = GetAircraftFPType(fp);
    std::string navComStr = std::string(1, GetNavData(fp));

    std::string squawkStr = GetSquawk(rtPos);
    std::string rFieldStr = std::string(1, GetNavData(fp));

    // Row 1 – identification strip

    field(7,   20, 40,  14, "FLIGHT ID",              GetCallsign(fp).c_str());
    field(54,  20, 81,  14, "RADIO CALLSIGN",         "");
    field(142, 20, 15,  14, "NO",                     "01");
    field(162, 20, 25,  14, "A/C",                    aircraftTypeStr.c_str());
    field(193, 20, 10,  14, "W",                      "M");
    field(210, 20, 25,  14, "DEP",                    GetOrigin(fp).c_str());
    field(243, 20, 25,  14, "DEST",                   GetDest(fp).c_str());
    field(275, 20, 106, 14, "NAV/COM",                navComStr.c_str());
    field(388, 20, 15,  14, "RVSM",                   GetRVSM(fp) ? "YES" : "NO");
    field(411, 20, 106, 14, "SURVEILLANCE EQUIPMENT", "C");
    field(522, 20, 25,  14, "CSSR",                   squawkStr.c_str());
    field(554, 20, 10,  14, "R",                      rFieldStr.c_str());
    field(569, 20, 10,  14, "FT",                     "S");


    // Row 2 – SID / FIR ROUTE / STAR
    std::string route = GetRoute(fp).c_str();
    if (route.length() >= 105) {
        route.resize(105);
    }

    field(7,   47, 40,  13, "SID",      GetSidName(fp).c_str());
    field(55,  47, 439, 13, "FIR ROUTE", route.c_str(), true);
    field(503, 47, 40,  13, "STAR",     GetStar(fp).c_str());


    // Row 3 – section headers

    textAt(261, 69, fLabel, LABEL_TEXT, "CRUISING");
    textAt(354, 69, fLabel, LABEL_TEXT, "ESTIMATE");
    textAt(440, 69, fLabel, LABEL_TEXT, "POS. REPORT");


    // Row 3 – data fields
    std::string finalLevelStr = std::to_string(GetFinalLevel(fp));

    int finalLevelFt = GetFinalLevel(fp);
    char levelBuf[16];
    snprintf(levelBuf, sizeof(levelBuf), "F%03d", finalLevelFt / 100);
    std::string cruisingLevelStr = levelBuf;

    int speedKt = GetSpeed(fp);
    char speedBuf[16];
    snprintf(speedBuf, sizeof(speedBuf), "N%04d", speedKt);
    std::string cruisingSpeedStr = speedBuf;

    field(7,   80, 35, 14, "EOBD",      "");
    field(50,  80, 25, 14, "EOBT",      GetEOBT(fp).c_str());
    field(84,  80, 20, 14, "MSG",       "FPL");
    field(113, 80, 25, 14, "CTOT",      "");
    field(146, 80, 20, 14, "ATFCM",     "");
    field(175, 80, 25, 14, "ATD",       GetATD(fp).c_str());
    field(208, 80, 25, 14, "ETA",       "");
    field(242, 80, 30, 14, "SPEED",     cruisingSpeedStr.c_str());
    field(278, 80, 31, 14, "LEVEL",     cruisingLevelStr.c_str());
    field(317, 80, 30, 14, "FIX",       "");
    field(354, 80, 25, 14, "TIME",      "");
    field(385, 80, 30, 14, "LEVEL",     finalLevelStr.c_str());
    field(424, 80, 30, 14, "SPEED",     cruisingSpeedStr.c_str());
    field(460, 80, 30, 14, "FIX",       "");
    field(497, 80, 25, 14, "ETO",       "");
    field(530, 80, 50, 14, "ALT AD(S)", GetAltAD(fp).c_str());


    // FIELD 18
    std::string remarks = GetRemarks(fp).c_str();
    if (remarks.length() >= 105) {
        remarks.resize(105);
    }
    strip(7, 111, 573, 13, "FIELD 18", 39, remarks.c_str());


    // EAT row

    field(7,   126, 25,  13, "EAT",       "");
    field(40,  126, 106, 13, "FREE TEXT", "");
    field(160, 126, 30,  13, "CFL",       std::to_string(ClearedAlt(fp)).c_str());
    field(204, 126, 31,  13, "ECL",       cruisingLevelStr.c_str());
    field(348, 126, 25,  13, "S",         "");
    field(382, 126, 40,  13, "REQ",       cruisingLevelStr.c_str());
    field(430, 126, 106, 13, "STS",       "");
    field(545, 126, 35,  13, "MODE S",    "TRUE");


    // ORIGINAL ROUTE

    strip(7, 156, 573, 13, "ORIGINAL ROUTE", 64,
          route.c_str());


    // Nav-link grid

    const char* row1[8] = { "VIEW"};
    const char* row2[8] = { "MODIFY"  };
    const int rowH = 10;

    for (int i = 0; i < 1; ++i)
    {
        int cx = 7 + 42 * i;
        navLink(cx, 178, rowH, row1[i], i == 0);
        navLink(cx, 197, rowH, row2[i], false);
    }



    // Action buttons

    raisedBtn(426, 184, 34, 20, "UPDATE", true);
    raisedBtn(464, 184, 34, 20, "CANCEL");
    raisedBtn(502, 184, 34, 20, "CLEAR");
    raisedBtn(540, 184, 34, 20, "PRINT");

    // Cleanup

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldFont);

    DeleteObject(penEdge);
    DeleteObject(brBack);
    DeleteObject(brField);
    DeleteObject(brTitle);
    DeleteObject(brBtn);
    DeleteObject(fTitle);
    DeleteObject(fLabel);
    DeleteObject(fValue);
    DeleteObject(fBtn);
}