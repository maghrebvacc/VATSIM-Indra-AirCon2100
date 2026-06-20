#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

#include "EuroScopePlugIn.h"

#include "TopSkyFunctions.h"

#include "common/bar.h"
#include "common/buttons.h"
using namespace EuroScopePlugIn;
using namespace Gdiplus;

// Required for GDI+
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

    virtual void OnRefresh(HDC hDC, int Phase) override
    {
        Graphics graphics(hDC);
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
        }



        RECT ExecutiveRECT = {};
        ExecutiveRECT = {
            10,
            870,
            10 + 100,
            870 + 30
        };
        RECT PlannerRECT = {};
        PlannerRECT = {
            10,
            900,
            10 + 100,
            900 + 30
        };
        RECT PlannerFPL = {};
        PlannerFPL = {
            140,
            900,
            140 + 100,
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