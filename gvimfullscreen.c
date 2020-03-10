/*
cl /LD gvimfullscreen.c user32.lib
------------------------------
:call libcallnr("gvimfullscreen.dll", "EnableFullScreen", 1)
*/

#include <windows.h>

int g_x, g_y, g_dx, g_dy;

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lParam)
{
    HWND* pphWnd = (HWND*)lParam;

    if (GetParent(hwnd))
    {
        *pphWnd = NULL;
        return TRUE;
    }

    *pphWnd = hwnd;
    return FALSE;
}

LONG _declspec(dllexport) ToggleFullScreen(int flag)
{
    HWND hTop = NULL;
    DWORD dwThreadID;

    dwThreadID = GetCurrentThreadId();
    EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTop);

    BOOL force    = flag & 0x0001;
    BOOL monitors = flag & 0x0002;

    if (hTop)
    {
        // Determine the current state of the window
        MONITORINFO mi;
        RECT rc;
        HMONITOR hMonitor;

        GetWindowRect(hTop, &rc);
        hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

        // Get the work area or entire monitor rect.
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMonitor, &mi);

        g_x = mi.rcMonitor.left;
        g_y = mi.rcMonitor.top;
        g_dx = (mi.rcMonitor.right - g_x)  * (monitors + 1);
        g_dy = (mi.rcMonitor.bottom - g_y) * (monitors + 1);

        if ( force || GetWindowLong(hTop, GWL_STYLE) & WS_CAPTION )
        {
            // Has a caption, so isn't maximised
            // Remove border, caption, and edges
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_BORDER);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_CAPTION);
            SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
            SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
        }
        else
        {
            // Already full screen, so restore all the previous styles
            SetWindowLong(hTop, GWL_EXSTYLE, GetWindowLong(hTop, GWL_EXSTYLE) | WS_BORDER);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_CAPTION);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MINIMIZEBOX);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_MAXIMIZEBOX);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_SYSMENU);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_CLIENTEDGE);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_EX_WINDOWEDGE);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_THICKFRAME);
            SetWindowLong(hTop, GWL_STYLE, GetWindowLong(hTop, GWL_STYLE) | WS_DLGFRAME);

            SendMessage(hTop, WM_SYSCOMMAND, SC_RESTORE, 0);
            SendMessage(hTop, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }

        SetWindowPos(hTop, HWND_TOP, g_x, g_y, g_dx, g_dy, SWP_SHOWWINDOW);

        // Now need to find the child text area window
        // and set it's size accordingly
        EnumChildWindows(hTop, EnumChildProc, 0);
    }

    // nord0.gui = #2E3440
    SetClassLongPtr(hTop, GCLP_HBRBACKGROUND,
            (ULONG_PTR)CreateSolidBrush(RGB(46, 52, 64)));

    return GetLastError();
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    char lpszClassName[100];
    GetClassName(hwnd, lpszClassName, 100);

    if ( strcmp(lpszClassName, "VimTextArea") == 0 )
    {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_CLIENTEDGE);
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
        // UHD settings:
        // (2,0) for font 12
        // (6,9) for font 22
        const int custom_offset_x = 2;
        const int custom_offset_y = 0;
        SetWindowPos(hwnd, HWND_TOP, custom_offset_x, custom_offset_y, g_dx, g_dy, SWP_SHOWWINDOW);
    }

    return TRUE;
}
