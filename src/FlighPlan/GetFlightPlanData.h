#pragma once

#include <string>
#include "EuroScopePlugIn.h"

std::string GetCallsign(EuroScopePlugIn::CFlightPlan fp);

// Single capability/equipment character (NOT the ICAO type string).
char GetAircraftType(EuroScopePlugIn::CFlightPlan fp);

// ICAO aircraft type string, e.g. "B738". Use this for display fields.
std::string GetAircraftFPType(EuroScopePlugIn::CFlightPlan fp);

char GetWakeCatagory(EuroScopePlugIn::CFlightPlan fp);

std::string GetOrigin(EuroScopePlugIn::CFlightPlan fp);
std::string GetDest(EuroScopePlugIn::CFlightPlan fp);

char GetNavData(EuroScopePlugIn::CFlightPlan fp);

bool GetRVSM(EuroScopePlugIn::CFlightPlan fp);

std::string GetSquawk(EuroScopePlugIn::CRadarTargetPositionData fp);

std::string GetPlan(EuroScopePlugIn::CFlightPlan fp);

std::string GetSidName(EuroScopePlugIn::CFlightPlan fp);
std::string GetRoute(EuroScopePlugIn::CFlightPlan fp);
std::string GetStar(EuroScopePlugIn::CFlightPlan fp);

std::string GetEOBT(EuroScopePlugIn::CFlightPlan fp);
std::string GetATD(EuroScopePlugIn::CFlightPlan fp);

int GetFinalLevel(EuroScopePlugIn::CFlightPlan fp);

// Filed true airspeed in knots, wraps CFlightPlanData::GetTrueAirspeed().
int GetSpeed(EuroScopePlugIn::CFlightPlan fp);

std::string GetAltAD(EuroScopePlugIn::CFlightPlan fp);
std::string GetRemarks(EuroScopePlugIn::CFlightPlan fp);

int ClearedAlt(EuroScopePlugIn::CFlightPlan fp);
int FinalAlt(EuroScopePlugIn::CFlightPlan fp);