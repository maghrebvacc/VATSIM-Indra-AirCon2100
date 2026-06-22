#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")

#include "EuroScopePlugIn.h"

#include "TopSkyFunctions.h"

#include "common/bar.h"
#include "common/buttons.h"
#include "FlighPlan/DrawWindow/DrawFlightPlanWindow.h"
#include "FlighPlan/Get/GetFlightPlanData.h"
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

const int ObjectExecutive  = 100;
const int ObjectPlanner    = 101;
const int ObjectFPL        = 102;
const int ObjectFPLWindow  = 103;
const int ObjectFPLView    = 104;
const int ObjectFPLModify  = 105;
const int ObjectFPLClose = 106;

class MyRadarScreen : public CRadarScreen
{
public:
    MyRadarScreen() {}
    virtual ~MyRadarScreen() {}

    std::string m_OpenedFplCallsign;

    // Flight-plan window state
    int    m_FplModifyView = 0;   // 0 = VIEW, 1 = MODIFY
    double m_FplScale      = 1.15;

    // Window position (top-left corner, in screen pixels)
    int    m_FplX = 100;
    int    m_FplY = 100;

    // Offset from window top-left to the cursor grab point inside the title bar
    int    m_DragOffsetX = 0;
    int    m_DragOffsetY = 0;

    virtual void OnRefresh(HDC hDC, int Phase) override
    {
        Graphics graphics(hDC);
        if (Phase != EuroScopePlugIn::REFRESH_PHASE_AFTER_LISTS) return;

        DrawBarMain(hDC);
        DrawBarMessage(hDC);

        if (IsExecutive)
            DrawButton(hDC, 10, 850, 110, 25, "EXECUTIVE", 0, 1);
        else
            DrawButton(hDC, 10, 850, 110, 25, "EXECUTIVE", 1, 1);

        if (IsPlanner)
            DrawButton(hDC, 10, 880, 110, 25, "PLANNER", 0, 1);
        else
            DrawButton(hDC, 10, 880, 110, 25, "PLANNER", 1, 1);

        if (IsFlightPlan)
        {
            DrawButton(hDC, 140, 880, 60, 25, "FPL", 0, 1);
        }
        else
        {
            DrawButton(hDC, 140, 880, 60, 25, "FPL", 1, 1);

            CFlightPlan fp = GetPlugIn()->FlightPlanSelect(m_OpenedFplCallsign.c_str());
            CRadarTarget rt = fp.GetCorrelatedRadarTarget();
            CRadarTargetPositionData rtPos = rt.IsValid()
                ? rt.GetPosition()
                : CRadarTargetPositionData();

            //  Draw the window
            RECT viewRect   = {};
            RECT modifyRect = {};

            DrawFlightPlanWindow(
                hDC,
                m_FplX, m_FplY,
                fp, rtPos,
                m_FplScale,
                m_FplModifyView,
                viewRect,
                modifyRect);

            //  Scaled helpers (mirror what the draw function uses)
            auto ScaleI = [&](int v) -> int {
                return (int)std::lround(v * m_FplScale);
            };

            int winW = ScaleI(586);
            int winH = ScaleI(229);

            //  Title-bar drag region (full width, 13 px tall)
            RECT titleRect = {
                m_FplX,
                m_FplY,
                m_FplX + winW,
                m_FplY + ScaleI(13)
            };

            // Close
            RECT closeRect = {
                m_FplX + ScaleI(573),
                m_FplY + ScaleI(1),
                m_FplX + ScaleI(584),
                m_FplY + ScaleI(12)
            };

            // Register screen objects so EuroScope delivers click/move events
            AddScreenObject(ObjectFPLWindow, "FPLTitleBar", titleRect, true,  "Drag FPL window");
            AddScreenObject(ObjectFPLView,   "FPLView",     viewRect,  false, "Switch to View");
            AddScreenObject(ObjectFPLModify, "FPLModify",   modifyRect,false, "Switch to Modify");
            AddScreenObject(ObjectFPLClose,  "FPLClose",    closeRect, false, "Close FPL window");
        }

        //  Static button screen objects
        RECT ExecutiveRECT = { 10, 850, 10 + 110, 850 + 30 };
        RECT PlannerRECT   = { 10, 880, 10 + 110, 880 + 30 };
        RECT PlannerFPL    = { 140, 880, 140 + 60, 880 + 30 };

        AddScreenObject(ObjectExecutive, "Executive", ExecutiveRECT, false, "Toggle Executive");
        AddScreenObject(ObjectPlanner,   "Planner",   PlannerRECT,   false, "Toggle Planner");
        AddScreenObject(ObjectFPL,       "FPL",       PlannerFPL,    false, "Toggle FPL");
    }

    virtual void OnClickScreenObject(
        int ObjectType,
        const char* sObjectId,
        POINT Pt,
        RECT Area,
        int Button) override {
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

        // VIEW / MODIFY nav-links
        if (ObjectType == ObjectFPLView && Button == BUTTON_LEFT)
        {
            m_FplModifyView = 0;
            RequestRefresh();
        }
        if (ObjectType == ObjectFPLModify && Button == BUTTON_LEFT)
        {
            m_FplModifyView = 1;
            RequestRefresh();
        }
        if (ObjectType == ObjectFPLClose)
        {
            IsFlightPlan = 1;
            RequestRefresh();
        }
    }

    virtual void OnButtonDownScreenObject(
        int ObjectType,
        const char* sObjectId,
        POINT Pt,
        RECT Area,
        int Button) override
    {
        if (ObjectType == ObjectFPLWindow && Button == BUTTON_LEFT)
        {
            m_DragOffsetX = Pt.x - m_FplX;
            m_DragOffsetY = Pt.y - m_FplY;
        }
    }

    // OnMoveScreenObject is called continuously while the user drags.
    // EuroScope passes the current cursor position in Pt and the original
    // object Area.  We move the window so its top-left follows the cursor,
    // offset by where the user first grabbed inside the title bar.
    virtual void OnMoveScreenObject(
        int ObjectType,
        const char* sObjectId,
        POINT Pt,
        RECT Area,
        bool Released) override
    {
        if (ObjectType == ObjectFPLWindow)
        {
            // Pt is the absolute cursor position; subtract the grab offset
            // recorded in OnButtonDownScreenObject.
            m_FplX = Pt.x - m_DragOffsetX;
            m_FplY = Pt.y - m_DragOffsetY;

            RequestRefresh();
        }
    }

    virtual void OnAsrContentToBeClosed() override {}
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
        double* pFontSize) override {}

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