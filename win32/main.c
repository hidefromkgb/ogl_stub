#define _WIN32_IE 0x0500
#define _WIN32_WINNT 0x0501
#define WINVER _WIN32_WINNT

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#include "../core/core.h"



HRESULT APIENTRY WindowProc(HWND hWnd, UINT uMsg, WPARAM wPrm, LPARAM lPrm) {
    static uint8_t keys[256] = {
        KEY_NONE      , KEY_LMB       , KEY_RMB       , KEY_NONE      ,
        KEY_MMB       , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_BACKSPACE , KEY_TAB       , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_ENTER     , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_PAUSE     ,
        KEY_CAPSLOCK  , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_ESC       ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_SPACE     , KEY_PAGEUP    , KEY_PAGEDOWN  , KEY_END       ,
        KEY_HOME      , KEY_LEFT      , KEY_UP        , KEY_RIGHT     ,
        KEY_DOWN      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_PRTSCR    , KEY_INSERT    , KEY_DELETE    , KEY_NONE      ,
        KEY_0         , KEY_1         , KEY_2         , KEY_3         ,
        KEY_4         , KEY_5         , KEY_6         , KEY_7         ,
        KEY_8         , KEY_9         , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_A         , KEY_B         , KEY_C         ,
        KEY_D         , KEY_E         , KEY_F         , KEY_G         ,
        KEY_H         , KEY_I         , KEY_J         , KEY_K         ,
        KEY_L         , KEY_M         , KEY_N         , KEY_O         ,
        KEY_P         , KEY_Q         , KEY_R         , KEY_S         ,
        KEY_T         , KEY_U         , KEY_V         , KEY_W         ,
        KEY_X         , KEY_Y         , KEY_Z         , KEY_LSYSTEM   ,
        KEY_RSYSTEM   , KEY_RMENU     , KEY_NONE      , KEY_NONE      ,
        KEY_NUM_0     , KEY_NUM_1     , KEY_NUM_2     , KEY_NUM_3     ,
        KEY_NUM_4     , KEY_NUM_5     , KEY_NUM_6     , KEY_NUM_7     ,
        KEY_NUM_8     , KEY_NUM_9     , KEY_NUM_MUL   , KEY_NUM_ADD   ,
        KEY_NUM_ENTER , KEY_NUM_SUB   , KEY_NUM_DEL   , KEY_NUM_DIV   ,
        KEY_F1        , KEY_F2        , KEY_F3        , KEY_F4        ,
        KEY_F5        , KEY_F6        , KEY_F7        , KEY_F8        ,
        KEY_F9        , KEY_F10       , KEY_F11       , KEY_F12       ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NUM_LOCK  , KEY_SCRLOCK   , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_LSHIFT    , KEY_RSHIFT    , KEY_LCTRL     , KEY_RCTRL     ,
        KEY_LALT      , KEY_RALT      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_COLON     , KEY_EQUALS    ,
        KEY_LESS      , KEY_HYPHEN    , KEY_GREATER   , KEY_QUESTION  ,
        KEY_TILDE     , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_LBRACKET  ,
        KEY_PIPE      , KEY_RBRACKET  , KEY_QUOTE     , KEY_NONE      ,
        /** only KEY_NONE`s here... **/
    };
    INPUT vkey;
    POINT movp;

    switch (uMsg) {
        case WM_CREATE:
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            return 0;

        case WM_HOTKEY:
            /** Emulate a PrtScr, since we`ve captured it, and now
                the system redirects all PrtScr keystrokes to us.
                This is indeed ugly, but it`s how Windows works... **/
            UnregisterHotKey(hWnd, IDHOT_SNAPWINDOW);
            UnregisterHotKey(hWnd, IDHOT_SNAPDESKTOP);
            vkey = (INPUT){INPUT_KEYBOARD};
            vkey.ki.wVk = VK_SNAPSHOT;
            SendInput(1, &vkey, sizeof(INPUT));
            RegisterHotKey(hWnd, IDHOT_SNAPDESKTOP, 0, VK_SNAPSHOT);
            RegisterHotKey(hWnd, IDHOT_SNAPWINDOW, 0, VK_SNAPSHOT);
            if (GetActiveWindow() != hWnd)
                break;
            wPrm = VK_SNAPSHOT;
        case WM_SYSKEYDOWN:
            uMsg = WM_KEYDOWN;
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            switch (wPrm) {
                case VK_SHIFT: /** [ MAPVK_VSC_TO_VK_EX ]-----v **/
                    wPrm = MapVirtualKey((lPrm >> 16) & 0xFF, 3); break;
                #define WIN_MKEY(a, b) wPrm = (lPrm & 0x01000000)? a : b
                case VK_MENU:    WIN_MKEY(VK_RMENU,     VK_LMENU);    break;
                case VK_CONTROL: WIN_MKEY(VK_RCONTROL,  VK_LCONTROL); break;
                case VK_RETURN:  WIN_MKEY(VK_SEPARATOR, VK_RETURN);   break;
                case VK_DELETE:  WIN_MKEY(VK_DELETE,    VK_DECIMAL);  break;
                case VK_INSERT:  WIN_MKEY(VK_INSERT,    VK_NUMPAD0);  break;
                case VK_END:     WIN_MKEY(VK_END,       VK_NUMPAD1);  break;
                case VK_DOWN:    WIN_MKEY(VK_DOWN,      VK_NUMPAD2);  break;
                case VK_NEXT:    WIN_MKEY(VK_NEXT,      VK_NUMPAD3);  break;
                case VK_LEFT:    WIN_MKEY(VK_LEFT,      VK_NUMPAD4);  break;
                case VK_CLEAR:   WIN_MKEY(VK_NUMPAD5,   VK_NUMPAD5);  break;
                case VK_RIGHT:   WIN_MKEY(VK_RIGHT,     VK_NUMPAD6);  break;
                case VK_HOME:    WIN_MKEY(VK_HOME,      VK_NUMPAD7);  break;
                case VK_UP:      WIN_MKEY(VK_UP,        VK_NUMPAD8);  break;
                case VK_PRIOR:   WIN_MKEY(VK_PRIOR,     VK_NUMPAD9);  break;
                #undef WIN_MKEY
            }
            cKbdInput((ENGC*)GetWindowLongPtr(hWnd, GWLP_USERDATA),
                       keys[wPrm & 0xFF], (uMsg == WM_KEYDOWN)? TRUE : FALSE);
            return 0;

        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if ((uMsg == WM_LBUTTONDOWN)
            ||  (uMsg == WM_MBUTTONDOWN)
            ||  (uMsg == WM_RBUTTONDOWN))
                SetCapture(hWnd);
            else
                ReleaseCapture();
            GetCursorPos(&movp);
            ScreenToClient(hWnd, &movp);
            cMouseInput((ENGC*)GetWindowLongPtr(hWnd, GWLP_USERDATA), movp.x,
                         movp.y, ((uMsg == WM_LBUTTONDOWN)? 1 << 1 : 0) |
                                 ((uMsg == WM_MBUTTONDOWN)? 1 << 2 : 0) |
                                 ((uMsg == WM_RBUTTONDOWN)? 1 << 3 : 0));
            return 0;

        case WM_MOUSEMOVE:
            GetCursorPos(&movp);
            ScreenToClient(hWnd, &movp);
            cMouseInput((ENGC*)GetWindowLongPtr(hWnd, GWLP_USERDATA), movp.x,
                         movp.y, ((wPrm & MK_LBUTTON)? 2 : 0) |
                                 ((wPrm & MK_MBUTTON)? 4 : 0) |
                                 ((wPrm & MK_RBUTTON)? 8 : 0) | 1);
            return 0;

        case WM_SIZE: {
            ENGC *engc = (ENGC*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

            if (engc && (wPrm != SIZE_MINIMIZED))
                cResizeWindow(engc, LOWORD(lPrm), HIWORD(lPrm));
            return 0;
        }
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wPrm, lPrm);
}



int APIENTRY WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdl, int show) {
    INITCOMMONCONTROLSEX icct = {sizeof(icct), ICC_STANDARD_CLASSES};
    WNDCLASSEX wndc = {sizeof(wndc), CS_HREDRAW | CS_VREDRAW, WindowProc,
                       0, 0, GetModuleHandle(0), 0, LoadCursor(0, IDC_ARROW),
                       0, 0, " ", 0};
    PIXELFORMATDESCRIPTOR ppfd = {sizeof(ppfd), 1, PFD_SUPPORT_OPENGL |
                                  PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
                                  PFD_TYPE_RGBA, 32};
    RECT rect = {};
    MSG pmsg = {};
    HGLRC mwrc;
    HDC mwdc;
    HWND hwnd;
    ENGC *engc;

    int64_t oldt = 0, time = 0;
    int32_t lbit = 0, hbit = 0;

//    AllocConsole();
//    freopen("CONOUT$", "wb", stdout);

    InitCommonControlsEx(&icct);
    RegisterClassEx(&wndc);
    hwnd = CreateWindowEx(0, wndc.lpszClassName, 0, WS_TILEDWINDOW,
                          0, 0, 0, 0, 0, 0, wndc.hInstance, 0);
    RegisterHotKey(hwnd, IDHOT_SNAPDESKTOP, 0, VK_SNAPSHOT);
    RegisterHotKey(hwnd, IDHOT_SNAPWINDOW, 0, VK_SNAPSHOT);
    SendMessage(hwnd, WM_SETICON, ICON_BIG,
               (LPARAM)LoadIcon(wndc.hInstance, MAKEINTRESOURCE(1)));

    mwdc = GetDC(hwnd);
    ppfd.iLayerType = PFD_MAIN_PLANE;
    SetPixelFormat(mwdc, ChoosePixelFormat(mwdc, &ppfd), &ppfd);
    wglMakeCurrent(mwdc, mwrc = wglCreateContext(mwdc));
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(engc = cMakeEngine()));

    rect.right = 800;
    rect.bottom = 600;
    rect.left = (GetSystemMetrics(SM_CXSCREEN) - rect.right) >> 1;
    rect.top = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) >> 1;
    SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top,
                 rect.right, rect.bottom, SWP_SHOWWINDOW);

    while (pmsg.message != WM_QUIT) {
        if (PeekMessage(&pmsg, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&pmsg);
            continue;
        }
        lbit = GetTickCount();
        if ((time < 0) && (lbit >= 0))
            hbit++;
        time = ((uint64_t)lbit & 0xFFFFFFFF) | ((uint64_t)hbit << 32);
        if (time - oldt >= DEF_UTMR) {
            cUpdateState(engc);
            oldt = time;
        }
        cRedrawWindow(engc);
        SwapBuffers(mwdc);
    }
    cFreeEngine(&engc);
    wglMakeCurrent(0, 0);
    wglDeleteContext(mwrc);
    ReleaseDC(hwnd, mwdc);
    DeleteDC(mwdc);
    DestroyWindow(hwnd);
    UnregisterHotKey(hwnd, IDHOT_SNAPWINDOW);
    UnregisterHotKey(hwnd, IDHOT_SNAPDESKTOP);

//    fclose(stdout);
//    FreeConsole();

    exit(0);
    return 0;
}
