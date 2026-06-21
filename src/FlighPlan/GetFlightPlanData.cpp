#define NOMINMAX
#include <windows.h>
#include <string>
#include <cmath>
#include "GetFlightPlanData.h"
#include "EuroScopePlugIn.h"
using namespace EuroScopePlugIn;

std::string GetCallsign(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    return fp.GetCallsign();
}

// GetAircraftType() returns a single capability/equipment character, NOT the
// ICAO aircraft type string (e.g. "B738"). Kept as-is in case other code
// already depends on this single-char value.
char GetAircraftType(CFlightPlan fp) {
    if (!fp.IsValid())
        return '\0';

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetAircraftType();
}

// GetAircraftFPType() returns the actual filed ICAO aircraft type string
// (e.g. "B738", "A320"). This is what DrawFlightPlanWindow.cpp needs for
// the "A/C" field — it did not exist in the original file, which is why
// the draw code failed to compile/link.
std::string GetAircraftFPType(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetAircraftFPType();
}

char GetWakeCatagory(CFlightPlan fp) {
    if (!fp.IsValid())
        return '\0';

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetAircraftWtc();
}

std::string GetOrigin(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();

    return fpData.GetOrigin();
}

std::string GetDest(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();

    return fpData.GetDestination();
}

char GetNavData(CFlightPlan fp) {
    if (!fp.IsValid())
        return '\0';

    CFlightPlanData fpData = fp.GetFlightPlanData();

    return fpData.GetCapibilities();
}

bool GetRVSM(CFlightPlan fp) {
    if (!fp.IsValid())
        return false;

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.IsRvsm();
}

std::string GetSquawk(CRadarTargetPositionData fp) {
    if (!fp.IsValid())
        return "";
    return fp.GetSquawk();
}

std::string GetPlan(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetPlanType();
}

std::string GetSidName(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";
    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetSidName();
}

std::string GetRoute(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";
    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetRoute();
}

std::string GetStar(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetStarName();
}

// GetEstimatedDepartureTime()/GetActualDepartureTime() return const char*
// pointing into an SDK-owned buffer that can be invalidated/overwritten on
// the next call. DrawFlightPlanWindow.cpp calls GetEOBT(fp).c_str() and
// GetATD(fp).c_str() — that requires std::string, not const char*, both to
// compile and to be safe. Fixed both return types.
std::string GetEOBT(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetEstimatedDepartureTime();
}

std::string GetATD(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetActualDepartureTime();
}

// GetFinalAltitude() returns int (feet), not char. The old char return type
// truncated altitudes (e.g. 35000) down to a single byte. Fixed to int.
int GetFinalLevel(CFlightPlan fp) {
    if (!fp.IsValid())
        return 0;

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetFinalAltitude();
}

// Filed true airspeed in knots. New wrapper around GetTrueAirspeed(),
// which was never previously exposed through this file.
int GetSpeed(CFlightPlan fp) {
    if (!fp.IsValid())
        return 0;

    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetTrueAirspeed();
}

std::string GetAltAD(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";
    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetAlternate();
}

std::string GetRemarks(CFlightPlan fp) {
    if (!fp.IsValid())
        return "";
    CFlightPlanData fpData = fp.GetFlightPlanData();
    return fpData.GetRemarks();
}

int ClearedAlt(CFlightPlan fp) {
    if (!fp.IsValid())
        return 0;
    return fp.GetClearedAltitude();
}

int FinalAlt(CFlightPlan fp) {
    if (!fp.IsValid())
        return 0;
    return fp.GetFinalAltitude();
}