#define NOMINMAX
#include <windows.h>
#include <string>
#include <cmath>
#include "ModifyFlightPlan.h"
#include "EuroScopePlugIn.h"
using namespace EuroScopePlugIn;

void ChangeFlightPlan(CFlightPlan fp, std::string NewAircraftType, std::string NewDeparture, std::string NewDest, std::string ChangeFlightRules,  std::string NewRoute, int NewFinalAlt, int NewFinalSpeedKts, std::string NewAltADs, int NewClearedAlt) {

/*/
 * This function sets the following new details about an already existing flightplan:
 *  Aircraft Type
 *  Departure aerodrome
 *  Destination aerodrome
 *  FlightRules (I = IFR, V = VFR)
 *  New SID
 *  New STAR
 *  Changes their whole route to the one provided within the parameters
 *  New final altitude (36000 NOT 360)
 *  New Final speed (KT's)
 *  Changes their Alternate aerodromes
 *  Changes their cleared Altitude (36000 NOT 360)
 */


    CFlightPlanControllerAssignedData fpAssigned = fp.GetControllerAssignedData();
    CFlightPlanData fpData = fp.GetFlightPlanData();


    fpData.SetAircraftInfo(NewAircraftType.c_str());
    fpData.SetOrigin(NewDeparture.c_str());
    fpData.SetDestination(NewDest.c_str());
    fpData.SetPlanType(ChangeFlightRules.c_str());
    fpData.SetRoute(NewRoute.c_str());
    fpData.SetFinalAltitude(NewFinalAlt);
    fpData.SetTrueAirspeed(NewFinalSpeedKts);
    fpData.SetAlternate(NewAltADs.c_str());
    fpAssigned.SetClearedAltitude(NewClearedAlt);
}