#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

// -------------------- Window --------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP hBackground = NULL; // offscreen static background

    switch (uMsg) {
        case WM_CREATE: {
            // Create a static dark background bitmap
            HDC hdc = GetDC(hwnd);
            HDC memDC = CreateCompatibleDC(hdc);
            RECT rect;
            GetClientRect(hwnd, &rect);

            hBackground = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HGDIOBJ oldBmp = SelectObject(memDC, hBackground);

            HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20)); // dark gray
            FillRect(memDC, &rect, brush);
            DeleteObject(brush);

            SelectObject(memDC, oldBmp);
            DeleteDC(memDC);
            ReleaseDC(hwnd, hdc);
        } return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Blit the static background
            if (hBackground) {
                HDC memDC = CreateCompatibleDC(hdc);
                HGDIOBJ oldBmp = SelectObject(memDC, hBackground);

                BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, memDC, 0, 0, SRCCOPY);

                SelectObject(memDC, oldBmp);
                DeleteDC(memDC);
            }

            EndPaint(hwnd, &ps);
        } return 0;

        case WM_DESTROY:
            if (hBackground) DeleteObject(hBackground);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
        sprintf(title, "Voxel Engine [FPS: %.2f]", counter->fps);
        SetWindowText(hwnd, title);
    }
}

// -------------------- Main Loop --------------------
void GameLoop(HWND hwnd) {
    FPSCounter fps;
    InitFPS(&fps);

    MSG msg = {0};
    while (1) {
        // Handle window messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update FPS only; background is static
        UpdateFPS(&fps, hwnd);

        Sleep(1); // small sleep to yield CPU
    }
}

// -------------------- Window Creation --------------------
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

// -------------------- Entry Point --------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd = CreateMainWindow(hInstance, 800, 600, "Voxel Engine");
    GameLoop(hwnd);
    return 0;
}
