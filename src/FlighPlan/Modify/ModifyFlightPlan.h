#pragma once

#include <string>
#include "EuroScopePlugIn.h"

void ChangeFlightPlan(EuroScopePlugIn::CFlightPlan fp, std::string NewAircraftType, std::string NewDeparture, std::string NewDest, std::string ChangeFlightRules,  std::string NewRoute, int NewFinalAlt, int NewFinalSpeedKts, std::string NewAltADs, int NewClearedAlt);