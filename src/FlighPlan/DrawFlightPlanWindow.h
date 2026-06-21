#pragma once

#include <windows.h>
#include "EuroScopePlugIn.h"

void DrawFlightPlanWindow(HDC hdc,int ox,int oy, EuroScopePlugIn::CFlightPlan fp, EuroScopePlugIn::CRadarTargetPositionData rtPos, double scale);
