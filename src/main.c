#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

// -------------------- Window --------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND CreateMainWindow(HINSTANCE hInstance, int width, int height, const char* title) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SimpleWindow";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, SW_SHOW);
    return hwnd;
}

// -------------------- Rendering --------------------
void ClearScreen(HDC hdc, HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20));
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
}

// -------------------- FPS --------------------
typedef struct {
    LARGE_INTEGER freq;
    LARGE_INTEGER prevTime;
    int frameCount;
    double fps;
} FPSCounter;

void InitFPS(FPSCounter* counter) {
    QueryPerformanceFrequency(&counter->freq);
    QueryPerformanceCounter(&counter->prevTime);
    counter->frameCount = 0;
    counter->fps = 0;
}

void UpdateFPS(FPSCounter* counter, HWND hwnd) {
    counter->frameCount++;
    LARGE_INTEGER currTime;
    QueryPerformanceCounter(&currTime);
    double elapsed = (double)(currTime.QuadPart - counter->prevTime.QuadPart) / counter->freq.QuadPart;

    if (elapsed >= 1.0) {
        counter->fps = counter->frameCount / elapsed;
        counter->frameCount = 0;
        counter->prevTime = currTime;

        char title[128];
        sprintf(title, "Voxel Engine - Dark Frame [FPS: %.2f]", counter->fps);
        SetWindowText(hwnd, title);
    }
}

// -------------------- Main Loop --------------------
void GameLoop(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    FPSCounter fps;
    InitFPS(&fps);

    const double targetFrameTime = 1.0 / 60.0; // 60 FPS target

    MSG msg = {0};
    while (1) {
        LARGE_INTEGER frameStart;
        QueryPerformanceCounter(&frameStart);

        // Handle window messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update & render
        ClearScreen(hdc, hwnd);
        UpdateFPS(&fps, hwnd);

        // Wait until next frame to cap FPS
        LARGE_INTEGER frameEnd;
        QueryPerformanceCounter(&frameEnd);
        double frameTime = (double)(frameEnd.QuadPart - frameStart.QuadPart) / fps.freq.QuadPart;
        double sleepTime = targetFrameTime - frameTime;
        if (sleepTime > 0) {
            Sleep((DWORD)(sleepTime * 1000)); // convert seconds to ms
        }
    }

    ReleaseDC(hwnd, hdc);
}

// -------------------- Entry Point --------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd = CreateMainWindow(hInstance, 800, 600, "Voxel Engine");
    GameLoop(hwnd);
    return 0;
}
