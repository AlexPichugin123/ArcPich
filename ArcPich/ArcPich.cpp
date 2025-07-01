// #define vv1 включает первую версию проги
#ifdef vv1

#include <windows.h>
#include <vector>

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// —труктура блока
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};

// √лобальные переменные
HBRUSH hBlockBrush; // кисть дл€ блоков
HBRUSH hPaddleBrush; // кисть дл€ ракетки
HBRUSH hBallBrush;   // кисть дл€ м€ча

// ќбъ€вление функций
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szClassName;
    wc.lpfnWndProc = WndProc;

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        szClassName,
        L"јрканойд с HBRUSH",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) return -1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// »нициализаци€ игровых объектов
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // —оздаем кисти
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // красные блоки
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // син€€ ракетка
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // желтый м€ч

    // —оздаем блоки
    int blockRows = 5;
    int blockCols = 10;
    int blockWidth = 70;
    int blockHeight = 20;

    for (int i = 0; i < blockRows; ++i) {
        for (int j = 0; j < blockCols; ++j) {
            Block b;
            b.rect.left = j * (blockWidth + 5) + 10;
            b.rect.top = i * (blockHeight + 5) + 50;
            b.rect.right = b.rect.left + blockWidth;
            b.rect.bottom = b.rect.top + blockHeight;
            b.brush = hBlockBrush;
            b.destroyed = false;
            blocks.push_back(b);
        }
    }

    // —оздаем ракетку
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;

    // —оздаем м€ч
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;

}

// ќбработка сообщений окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static std::vector<Block> blocks;
    static RECT paddleRect;
    static RECT ballRect;

    static bool isLeftPressed = false;

    switch (msg) {
    case WM_CREATE:
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, 16, NULL); // таймер дл€ обновлени€ игры (~60 fps)
        return 0;

    case WM_TIMER:
        // ќбновление положени€ м€ча и проверка столкновений

    {
        int dx = 4 * (ballRect.left <= (paddleRect.left + 2) ? 1 : (ballRect.right >= paddleRect.right ? -1 : 1));
        int dy = -4;

        // ѕередвижение м€ча по X и Y
        OffsetRect(&ballRect, dx / abs(dx), dy / abs(dy));
        // ѕроверка столкновени€ с границами окна
        if (ballRect.left <= 0 && ballRect.right >= 800)
            OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // отражение по X

        if (ballRect.top <= 0)
            OffsetRect(&ballRect, 0, -dy / abs(dy)); // отражение по Y

        if (ballRect.bottom >= 600) {
            // ћ€ч упал вниз - сбросим позицию
            SetRect(&ballRect, 390, 530, 410, 550);
        }

        // —толкновение с ракеткой
        if (IntersectRect(NULL, &ballRect, &paddleRect)) {
            OffsetRect(&ballRect, 0, -(ballRect.bottom - paddleRect.top));
        }

        // ѕроверка столкновени€ с блоками
        for (auto& block : blocks) {
            if (!block.destroyed && IntersectRect(NULL, &ballRect, &block.rect)) {
                block.destroyed = true;
                OffsetRect(&ballRect, 0, -(ballRect.bottom - block.rect.top));
                break;
            }
        }
    }

    InvalidateRgn(hwnd, NULL, FALSE); // перерисовать окно
    return 0;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) isLeftPressed = true;
        if (wParam == VK_RIGHT) isLeftPressed = true;
        return 0;

    case WM_KEYUP:
        if (wParam == VK_LEFT  && wParam == VK_RIGHT)
            isLeftPressed = false;
        return 0;

    case WM_MOUSEMOVE:
    {
        POINT pt = { (short)LOWORD(lParam),(short)HIWORD(lParam) };
        if (pt.x > paddleRect.left && pt.x < paddleRect.right) {
            SetRect(&paddleRect, pt.x - 50, paddleRect.top, pt.x + 50, paddleRect.bottom);
            InvalidateRgn(hwnd, NULL, FALSE);
        }
    }
    return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // —оздаем кисть дл€ рамки
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // черна€ лини€

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }

        // ќтрисовка блоков
        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }
        DeleteObject(hPen);
        // ќтрисовка ракетки
        {
            HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            FillRgn(hdc, hPaddleRegion, hPaddleBrush);
            DeleteObject(hPaddleRegion);

            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPaddleBrush);
            Rectangle(hdc, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            SelectObject(hdc, hOldBrush);
        }

        // ќтрисовка м€ча
        {
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBallBrush);
            Ellipse(hdc, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
            SelectObject(hdc, hOldBrush);
        }

        EndPaint(hwnd, &ps);

        return 0;
    }

    case WM_DESTROY:
        DeleteObject(hBlockBrush);
        DeleteObject(hPaddleBrush);
        DeleteObject(hBallBrush);

        KillTimer(hwnd, 1);

        PostQuitMessage(0);

        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
#endif // vv1

#include <windows.h>
#include <vector>

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// —труктура блока
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};

// √лобальные переменные
HBRUSH hBlockBrush; // кисть дл€ блоков
HBRUSH hPaddleBrush; // кисть дл€ ракетки
HBRUSH hBallBrush;   // кисть дл€ м€ча

// ќбъ€вление функций
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szClassName;
    wc.lpfnWndProc = WndProc;

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        szClassName,
        L"јрканойд с HBRUSH",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) return -1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// »нициализаци€ игровых объектов
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // —оздаем кисти
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // красные блоки
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // син€€ ракетка
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // желтый м€ч

    // —оздаем блоки
    int blockRows = 5;
    int blockCols = 10;
    int blockWidth = 70;
    int blockHeight = 20;

    for (int i = 0; i < blockRows; ++i) {
        for (int j = 0; j < blockCols; ++j) {
            Block b;
            b.rect.left = j * (blockWidth + 5) + 10;
            b.rect.top = i * (blockHeight + 5) + 50;
            b.rect.right = b.rect.left + blockWidth;
            b.rect.bottom = b.rect.top + blockHeight;
            b.brush = hBlockBrush;
            b.destroyed = false;
            blocks.push_back(b);
        }
    }

    // —оздаем ракетку
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;

    // —оздаем м€ч
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;

}
HDC buffer2;
// ќбработка сообщений окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static std::vector<Block> blocks;
    static RECT paddleRect;
    static RECT ballRect;

    static bool isLeftPressed = false;

    switch (msg) {
    case WM_CREATE:
    {
        RECT r;
        GetClientRect(hwnd, &r);
        auto window_width = r.right - r.left;//определ€ем размеры и сохран€ем
        auto window_height = r.bottom - r.top;
        buffer2 = CreateCompatibleDC(GetDC(hwnd));//второй буфер
        SelectObject(GetDC(hwnd), CreateCompatibleBitmap(buffer2, window_width, window_height));
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, 16, NULL); // таймер дл€ обновлени€ игры (~60 fps)
        return 0;
    }

    case WM_TIMER:

        // ќбновление положени€ м€ча и проверка столкновений

    {
        int dx = 4 * (ballRect.left <= (paddleRect.left + 2) ? 1 : (ballRect.right >= paddleRect.right ? -1 : 1));
        int dy = -4;

        // ѕередвижение м€ча по X и Y
        OffsetRect(&ballRect, dx / abs(dx), dy / abs(dy));

        // ѕроверка столкновени€ с границами окна
        if (ballRect.left <= 0 && ballRect.right >= 800)
            OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // отражение по X

        if (ballRect.top <= 0)
            OffsetRect(&ballRect, 0, -dy / abs(dy)); // отражение по Y

        if (ballRect.bottom >= 600) {
            // ћ€ч упал вниз - сбросим позицию
            SetRect(&ballRect, 390, 530, 410, 550);
        }
        // —толкновение с ракеткой
        if (IntersectRect(NULL, &ballRect, &paddleRect)) {
            OffsetRect(&ballRect, 0, -(ballRect.bottom - paddleRect.top));
        }

        // ѕроверка столкновени€ с блоками
        for (auto& block : blocks) {
            if (!block.destroyed && IntersectRect(NULL, &ballRect, &block.rect)) {
                block.destroyed = true;
                OffsetRect(&ballRect, 0, -(ballRect.bottom - block.rect.top));
                break;
            }
        }
    }

    InvalidateRgn(hwnd, NULL, FALSE); // перерисовать окно
    return 0;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) isLeftPressed = true;
        if (wParam == VK_RIGHT) isLeftPressed = true;
        return 0;

    case WM_KEYUP:
        if (wParam == VK_LEFT && wParam == VK_RIGHT)
            isLeftPressed = false;
        return 0;

    case WM_MOUSEMOVE:
    {
        POINT pt = { (short)LOWORD(lParam),(short)HIWORD(lParam) };
        if (pt.x > paddleRect.left && pt.x < paddleRect.right) {
            SetRect(&paddleRect, pt.x - 50, paddleRect.top, pt.x + 50, paddleRect.bottom);
            InvalidateRgn(hwnd, NULL, FALSE);
        }
    }
    return 0;

    case WM_ERASEBKGND:
        return (LRESULT)1; // Say we handled it.

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        //HDC hdcMem;
        //HBITMAP hbmMem, hbmOld;
        //HBRUSH hbrBkGnd;
        //HFONT hfntOld;
        RECT rc;
        //GetClientRect(hwnd, &rc);
       // hdc = CreateCompatibleDC(hdc);
       // hbmMem = CreateCompatibleBitmap(hdc,
        //    rc.right - rc.left,
       //     rc.bottom - rc.top);
       // hbmOld = SelectObject(hdc, hbmMem);

        RECT rect;
        GetClientRect(hwnd, &rect);
        auto whiteBrush = CreateSolidBrush(RGB(0, 0, 255));
        FillRect(buffer2, &rect, whiteBrush);
        DeleteObject(whiteBrush);


        // —оздаем кисть дл€ рамки
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // черна€ лини€

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(buffer2, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }

        // ќтрисовка блоков
        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(buffer2, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }
        DeleteObject(hPen);
        // ќтрисовка ракетки
        {
            HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            FillRgn(buffer2, hPaddleRegion, hPaddleBrush);
            DeleteObject(hPaddleRegion);

            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPaddleBrush);
            Rectangle(buffer2, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            SelectObject(buffer2, hOldBrush);
        }

        // ќтрисовка м€ча
        {
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBallBrush);
            Ellipse(buffer2, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
            SelectObject(buffer2, hOldBrush);
        }

        BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, buffer2, 0, 0, SRCCOPY);//копируем буфер в окно
        EndPaint(hwnd, &ps);

        return 0;
    }

    case WM_DESTROY:
        DeleteObject(hBlockBrush);
        DeleteObject(hPaddleBrush);
        DeleteObject(hBallBrush);

        KillTimer(hwnd, 1);

        PostQuitMessage(0);

        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }
    return NULL;
}