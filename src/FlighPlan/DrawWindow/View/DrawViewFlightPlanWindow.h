#pragma once
#include <windows.h>
#include <string>
#include <cmath>
#include <cstdio>
#include "EuroScopePlugIn.h"
#include "../src/FlighPlan/Get/GetFlightPlanData.h"

// ---------------------------------------------------------------------------
//  DrawFlightPlanWindow
//
//  All positions/sizes are in "design units" and scaled uniformly by `scale`.
//  ox, oy  = top-left corner of the window in screen pixels.
//  modifyView   = 0 → VIEW tab active, 1 → MODIFY tab active
//  outViewRect  = [out] hit-test RECT for the VIEW navlink
//  outModifyRect= [out] hit-test RECT for the MODIFY navlink
// ---------------------------------------------------------------------------
void DrawViewFlightPlanWindow(
    HDC hdc,
    int ox, int oy,
    EuroScopePlugIn::CFlightPlan fp,
    EuroScopePlugIn::CRadarTargetPositionData rtPos,
    double scale,
    int  modifyView,
    RECT& outViewRect,
    RECT& outModifyRect);
