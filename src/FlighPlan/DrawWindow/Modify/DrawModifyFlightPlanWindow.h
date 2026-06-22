#pragma once
#include <windows.h>
#include <string>
#include <cmath>
#include <cstdio>
#include "EuroScopePlugIn.h"
#include "../src/FlighPlan/Get/GetFlightPlanData.h"


void DrawModifyFlightPlanWindow(
    HDC hdc,
    int ox, int oy,
    EuroScopePlugIn::CFlightPlan fp,
    EuroScopePlugIn::CRadarTargetPositionData rtPos,
    double scale,
    int  modifyView,
    RECT& outViewRect,
    RECT& outModifyRect);
