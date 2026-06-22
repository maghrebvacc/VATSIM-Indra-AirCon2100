#pragma once
#include <windows.h>
#include <string>
#include <cmath>
#include <cstdio>
#include "EuroScopePlugIn.h"
#include "FlighPlan/DrawWindow/View/DrawViewFlightPlanWindow.h"
#include "Modify/DrawModifyFlightPlanWindow.h"

using namespace EuroScopePlugIn;

void DrawFlightPlanWindow(
    HDC hdc,
    int ox,
    int oy,
    CFlightPlan fp,
    CRadarTargetPositionData rtPos,
    double scale,
    int modifyView,
    RECT& outViewRect,
    RECT& outModifyRect)
{
    SetRectEmpty(&outViewRect);
    SetRectEmpty(&outModifyRect);

    if (!fp.IsValid())
        return;

    if (modifyView == 0)
        DrawViewFlightPlanWindow(hdc, ox, oy, fp, rtPos, scale, modifyView, outViewRect, outModifyRect);
    else
        DrawModifyFlightPlanWindow(hdc, ox, oy, fp, rtPos, scale, modifyView, outViewRect, outModifyRect);
}
