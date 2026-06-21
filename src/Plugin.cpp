#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")

#include "EuroScopePlugIn.h"

#include "TopSkyFunctions.h"

#include "common/bar.h"
#include "common/buttons.h"
#include "FlighPlan/DrawFlightPlanWindow.h"
#include "FlighPlan/GetFlightPlanData.h"
using namespace EuroScopePlugIn;
using namespace Gdiplus;

/*/
 * Dear programmer:
 * When this was written, only god knew what it ment
 *
 * Therefore, if you are trying to add to to
 * it WILL fail and i wish you the best of luck
 *
 * Inshallah It will work
 */

ULONG_PTR g_gdiplusToken = 0;

int IsMessageOnScreen = 1;
int IsExecutive = 1;
int IsPlanner = 1;
int IsFlightPlan = 1;

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        GdiplusShutdown(g_gdiplusToken);
    }

    return TRUE;
}

void CheckMessageDraw()
{
    IsMessageOnScreen = 0;
}

const int ObjectExecutive = 100;
const int ObjectPlanner = 101;
const int ObjectFPL = 102;
class MyRadarScreen : public CRadarScreen
{
public:
    MyRadarScreen() {}
    virtual ~MyRadarScreen() {}

    std::string m_OpenedFplCallsign;

    virtual void OnRefresh(HDC hDC, int Phase) override
    {
        Graphics graphics(hDC);
        if (Phase != EuroScopePlugIn::REFRESH_PHASE_AFTER_LISTS) return;
        DrawBarMain(hDC);
        DrawBarMessage(hDC);

        if (IsExecutive) {
            DrawButton(hDC, 10, 870, 110, 25, "EXECUTIVE", 0, 1);
        }
        if (!IsExecutive) {
            DrawButton(hDC, 10, 870, 110, 25, "EXECUTIVE", 1, 1);
        }

        if (IsPlanner) {
            DrawButton(hDC, 10, 900, 110, 25, "PLANNER", 0, 1);
        }
        if (!IsPlanner) {
            DrawButton(hDC, 10, 900, 110, 25, "PLANNER", 1, 1);
        }

        if (IsFlightPlan) {
            DrawButton(hDC, 140, 900, 60, 25, "FPL", 0, 1);
        }

        if (!IsFlightPlan) {
            DrawButton(hDC, 140, 900, 60, 25, "FPL", 1, 1);
            CFlightPlan fp = GetPlugIn()->FlightPlanSelect(m_OpenedFplCallsign.c_str());
            CRadarTarget rt = fp.GetCorrelatedRadarTarget();
            CRadarTargetPositionData rtPos = rt.IsValid() ? rt.GetPosition() : CRadarTargetPositionData();

            DrawFlightPlanWindow(hDC, 100, 100, fp, rtPos, 1.15);
            std::string rFieldStr = std::string(1, GetNavData(fp));
            GetPlugIn()->DisplayUserMessage("Indra","Indra",rFieldStr.c_str(),true,true,true,true,true);
        }

        RECT ExecutiveRECT = {};
        ExecutiveRECT = {
            10,
            870,
            10 + 110,
            870 + 30
        };
        RECT PlannerRECT = {};
        PlannerRECT = {
            10,
            900,
            10 + 110,
            900 + 30
        };
        RECT PlannerFPL = {};
        PlannerFPL = {
            140,
            900,
            140 + 60,
            900 + 30
        };

        AddScreenObject(ObjectExecutive,"Executive", ExecutiveRECT,false, "Toggle Executive");
        AddScreenObject(ObjectPlanner,"Planner", PlannerRECT,false, "Toggle Planner");
        AddScreenObject(ObjectFPL,"FPL", PlannerFPL,false, "Toggle FPL");
    }

    virtual void OnClickScreenObject(
        int ObjectType,
        const char* sObjectId,
        POINT Pt,
        RECT Area,
        int Button) override
    {
        if (ObjectType == ObjectExecutive && Button == BUTTON_LEFT)
        {
            IsExecutive = 1 - IsExecutive;
            RequestRefresh();
        }

        if (ObjectType == ObjectPlanner && Button == BUTTON_LEFT)
        {
            IsPlanner = 1 - IsPlanner;
            RequestRefresh();
        }
        if (ObjectType == ObjectFPL && Button == BUTTON_LEFT)
        {
            if (IsFlightPlan)
            {
                CFlightPlan asel = GetPlugIn()->FlightPlanSelectASEL();
                m_OpenedFplCallsign = asel.IsValid() ? asel.GetCallsign() : "";
            }

            IsFlightPlan = 1 - IsFlightPlan;
            RequestRefresh();
        }
    }

    virtual void OnMoveScreenObject(
        int ObjectType,
        const char* sObjectId,
        POINT Pt,
        RECT Area,
        bool Released) override
    {
    }

    virtual void OnAsrContentToBeClosed() override
    {
    }
};

class MyPlugIn : public CPlugIn
{
    MyRadarScreen* m_Screen = nullptr;

public:
    MyPlugIn()
        : CPlugIn(
            COMPATIBILITY_CODE,
            "Indra Plugin",
            "1.0.1 B1",
            "Jamie Datson",
            "GPL V3")
    {
    }

    virtual ~MyPlugIn() {}

    virtual CRadarScreen* OnRadarScreenCreated(
        const char* sDisplayName,
        bool NeedRadarContent,
        bool GeoReferenced,
        bool CanBeSaved,
        bool CanBeCreated) override
    {
        m_Screen = new MyRadarScreen();
        return m_Screen;
    }

    virtual void OnRadarTargetPositionUpdate(CRadarTarget rt) override {}
    virtual void OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area) override {}
    virtual void OnGetTagItem(
        CFlightPlan FlightPlan,
        CRadarTarget rt,
        int ItemCode,
        int TagData,
        char sItemString[16],
        int* pColorCode,
        COLORREF* pRgbColor,
        double* pFontSize) override
    {
    }

    virtual void OnFlightPlanDisconnect(CFlightPlan fp) override {}
};

MyPlugIn* gPlugin = nullptr;


__declspec(dllexport) void EuroScopePlugInInit(CPlugIn** ppPlugIn)
{
    gPlugin = new MyPlugIn();
    *ppPlugIn = gPlugin;
}

__declspec(dllexport) void EuroScopePlugInExit()
{
    delete gPlugin;
    gPlugin = nullptr;
}