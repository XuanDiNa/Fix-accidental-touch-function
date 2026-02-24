#include <windows.h>
#include <cstdio>

#pragma comment(lib, "user32.lib")

namespace {

bool g_autoSprintEnabled = false;
bool g_autoCtrlHeld = false;
bool g_blockUntilRelease = false;
HHOOK g_keyboardHook = nullptr;
HANDLE g_thread = nullptr;
bool g_running = false;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_autoSprintEnabled) {
        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        
        if (kb->vkCode == 'B' && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
            if (g_autoCtrlHeld) {
                keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                g_autoCtrlHeld = false;
                OutputDebugStringA("[FixTouch] Blocked Ctrl+B\n");
                return 1;
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

bool IsInGame() {
    char title[256];
    HWND hwnd = GetForegroundWindow();
    GetWindowTextA(hwnd, title, sizeof(title));
    
    if (strstr(title, "Minecraft") == nullptr) return false;
    if (strstr(title, "Play") != nullptr) return false;
    if (strstr(title, "Settings") != nullptr) return false;
    if (strstr(title, "Options") != nullptr) return false;
    if (strstr(title, "Chat") != nullptr) return false;
    if (strstr(title, "World") != nullptr) return false;
    if (strstr(title, "Server") != nullptr) return false;
    if (strstr(title, "Menu") != nullptr) return false;
    
    return true;
}

bool HasInterferenceKey() {
    return (GetAsyncKeyState('B') & 0x8000) != 0;
}

void InstallHook() {
    g_keyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandleA(nullptr), 0);
}

void UninstallHook() {
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

DWORD WINAPI InputThread(LPVOID) {
    while (g_running) {
        if (IsInGame()) {
            bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            
            if (ctrlPressed && !g_autoCtrlHeld && !g_blockUntilRelease) {
                g_autoSprintEnabled = !g_autoSprintEnabled;
                g_autoCtrlHeld = true;
                char buf[256];
                sprintf_s(buf, "[FixTouch] Mode: %s\n", g_autoSprintEnabled ? "ON" : "OFF");
                OutputDebugStringA(buf);
            }
            
            if (!ctrlPressed && g_autoCtrlHeld) {
                g_autoCtrlHeld = false;
            }
            
            if (ctrlPressed && g_autoCtrlHeld && !g_blockUntilRelease) {
                g_blockUntilRelease = true;
            }
            
            if (!ctrlPressed) {
                g_blockUntilRelease = false;
            }
            
            if (g_autoSprintEnabled && !g_blockUntilRelease) {
                bool wPressed = (GetAsyncKeyState('W') & 0x8000) != 0;
                bool bPressed = HasInterferenceKey();
                
                if (bPressed && g_autoCtrlHeld) {
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                    g_autoCtrlHeld = false;
                    g_blockUntilRelease = true;
                }
                
                if (wPressed && !g_autoCtrlHeld && !bPressed) {
                    keybd_event(VK_CONTROL, 0, 0, 0);
                    g_autoCtrlHeld = true;
                } else if (!wPressed && g_autoCtrlHeld) {
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                    g_autoCtrlHeld = false;
                }
            }
        } else {
            if (g_autoCtrlHeld) {
                keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                g_autoCtrlHeld = false;
            }
            g_blockUntilRelease = false;
        }
        
        Sleep(5);
    }
    return 0;
}

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            OutputDebugStringA("[FixTouch] Loaded! Press Ctrl to toggle\n");
            InstallHook();
            g_running = true;
            g_thread = CreateThread(nullptr, 0, InputThread, nullptr, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            g_running = false;
            UninstallHook();
            if (g_thread) {
                WaitForSingleObject(g_thread, 1000);
                CloseHandle(g_thread);
            }
            break;
    }
    return TRUE;
}
