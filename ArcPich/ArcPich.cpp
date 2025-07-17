//#define vv1 включает первую версию проги
#ifdef vv1

#include <windows.h>
#include <vector>

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// Структура блока
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};

// Глобальные переменные
HBRUSH hBlockBrush; // кисть для блоков
HBRUSH hPaddleBrush; // кисть для ракетки
HBRUSH hBallBrush;   // кисть для мяча

// Объявление функций
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
        L"Арканойд с HBRUSH",
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

// Инициализация игровых объектов
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // Создаем кисти
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // красные блоки
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // синяя ракетка
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // желтый мяч

    // Создаем блоки
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

    // Создаем ракетку
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;

    // Создаем мяч
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;

}

// Обработка сообщений окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static std::vector<Block> blocks;
    static RECT paddleRect;
    static RECT ballRect;

    static bool isLeftPressed = false;

    switch (msg) {
    case WM_CREATE:
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, 16, NULL); // таймер для обновления игры (~60 fps)
        return 0;

    case WM_TIMER:
        // Обновление положения мяча и проверка столкновений

    {
        int dx = 4 * (ballRect.left <= (paddleRect.left + 2) ? 1 : (ballRect.right >= paddleRect.right ? -1 : 1));
        int dy = -4;

        // Передвижение мяча по X и Y
        OffsetRect(&ballRect, dx / abs(dx), dy / abs(dy));
        // Проверка столкновения с границами окна
        if (ballRect.left <= 0 && ballRect.right >= 800)
            OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // отражение по X

        if (ballRect.top <= 0)
            OffsetRect(&ballRect, 0, -dy / abs(dy)); // отражение по Y

        if (ballRect.bottom >= 600) {
            // Мяч упал вниз - сбросим позицию
            SetRect(&ballRect, 390, 530, 410, 550);
        }

        // Столкновение с ракеткой
        if (IntersectRect(NULL, &ballRect, &paddleRect)) {
            OffsetRect(&ballRect, 0, -(ballRect.bottom - paddleRect.top));
        }

        // Проверка столкновения с блоками
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

        // Создаем кисть для рамки
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // черная линия

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }

        // Отрисовка блоков
        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }
        DeleteObject(hPen);
        // Отрисовка ракетки
        {
            HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            FillRgn(hdc, hPaddleRegion, hPaddleBrush);
            DeleteObject(hPaddleRegion);

            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPaddleBrush);
            Rectangle(hdc, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            SelectObject(hdc, hOldBrush);
        }

        // Отрисовка мяча
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
#define vv2
#ifdef vv2
#include <windows.h>
#include <vector>
#include <cmath>
#include <random>
#include "Parametrs.h"

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// Структура блока
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};
// Глобальные переменные
HBRUSH hBlockBrush; // кисть для блоков
HBRUSH hPaddleBrush; // кисть для ракетки
HBRUSH hBallBrush;   // кисть для мяча
//float dy = -45; //(rand() % 10 + 5);//формируем вектор полета шарика
//float dx = 2;//-(1 - dy);//формируем вектор полета шарика
//int /*dx=0, dy=-5,*/ dy1=-3; //dy1 = скорость после сброса
//int TimPer = 500;
//int ballspeed=0;
//const int steps = sqrt(dy*dy+dx*dx); // число подшагов, поставить равным количеству пикслелей между нач и кон точкой движения шарика за 1 кадр 
////алгоритм брезенхема для проверки коллизии
//double stepDx = dx / steps; 
//double stepDy = dy / steps; 
//int newX;
//int newY;
//int side=0;
//int ballsize = 20;
static std::vector<Block> blocks;
static RECT paddleRect;
static RECT ballRect;
static bool isLeftPressed = false, isRightPressed = false;
POINT currentPos1;

// Объявление функций
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect);

//main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
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
        L"Арканойд с HBRUSH",
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
// Инициализация игровых объектов
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // Создаем кисти
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // красные блоки
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // синяя ракетка
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // желтый мяч
    // Создаем блоки
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

    // Создаем ракетку
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;
    
    // Создаем мяч
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;
    
}
//непосредтсвенно отрисовка
static void Paint(HWND hwnd, LPPAINTSTRUCT lpPS)
{ 
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    HBRUSH hbrBkGnd;

    GetClientRect(hwnd, &rc);
    hdcMem = CreateCompatibleDC(lpPS->hdc);
    hbmMem = CreateCompatibleBitmap(lpPS->hdc,
        rc.right - rc.left,
        rc.bottom - rc.top);
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);//тут с преобразованием типов данных прикол
    
    //чистим фон
    hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rc, hbrBkGnd);
    DeleteObject(hbrBkGnd);


    // Отрисовка ракетки
    {
        HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        FillRgn(hdcMem, hPaddleRegion, hPaddleBrush);
        DeleteObject(hPaddleRegion);

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hPaddleBrush);
        Rectangle(hdcMem, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        SelectObject(hdcMem, hOldBrush);
        
    }
    // Отрисовка мяча
    {
       
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBallBrush);
        Ellipse(hdcMem, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
        SelectObject(hdcMem, hOldBrush);
        //SetPixel(hdcMem, currentPos1.x, currentPos1.y, RGB(0, 250, 0)); //надо как-то трасировку отобразить 
        
        
    }
    
    // Отрисовка блоков
    for (const auto& block : blocks)
    {
        if (!block.destroyed)
        {
            HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
            FillRgn(hdcMem, hRegion, hBlockBrush);
            DeleteObject(hRegion);
        }
    }
    
    //
    // Blt the changes to the screen DC.
    //
    BitBlt(lpPS->hdc,
        rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top,
        hdcMem,
        0, 0,
        SRCCOPY);
    //
    // Done with off-screen bitmap and DC.
    //
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

// Упрощенная проверка коллизии с определением стороны
bool CheckCollisionWithSide(const RECT& ball, const RECT& block, int& collisionSide)
{
    if (!(ball.right > block.left && ball.left < block.right &&
          ball.bottom > block.top && ball.top < block.bottom))
        return false;

    //// Определяем сторону столкновения
    //int ballCenterX = (ball.left + ball.right) / 2;
    //int ballCenterY = (ball.top + ball.bottom) / 2;
    //int blockCenterX = (block.left + block.right) / 2;
    //int blockCenterY = (block.top + block.bottom) / 2;

    // Вычисляем перекрытия по каждой оси
    int overlapLeft = ball.right - block.left;
    int overlapRight = block.right - ball.left;
    int overlapTop = ball.bottom - block.top;
    int overlapBottom = block.bottom - ball.top;

    // Находим минимальное перекрытие
    int minOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;

    if (minOverlap == overlapTop) collisionSide = 0;    // Верх
    else if (minOverlap == overlapRight) collisionSide = 1; // Право
    else if (minOverlap == overlapBottom) collisionSide = 2; // Низ
    else collisionSide = 3; // Лево

    return true;
}

//основная функция
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    switch (msg)
    {
    case WM_CREATE:
    {

        RECT r;
        GetClientRect(hwnd, &r);
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, TimPer, NULL); // 1000/2ой параматер = фпс, таймер для обновления игры

        break;
    }

    case WM_TIMER:
    {
        // Трассировка движения мяча с шагами
        bool collisionDetected = false;
        RECT originalBallRect = ballRect;

        for (int i = 1; i <= steps && !collisionDetected; ++i)
        {
            // Промежуточная позиция мяча
            RECT tempBallRect = originalBallRect;
            OffsetRect(&tempBallRect, dx * i / steps, dy * i / steps);

            // Проверка столкновений с блоками
            for (auto& block : blocks)
            {
                if (!block.destroyed && CheckCollisionWithSide(tempBallRect, block.rect, side))
                {
                    collisionDetected = true;
                    block.destroyed = true;

                    // Корректируем позицию перед отскоком
                    switch (side)
                    {
                    case 0: // Верх блока
                        tempBallRect.bottom = block.rect.top;
                        tempBallRect.top = tempBallRect.bottom - (originalBallRect.bottom - originalBallRect.top);
                        dy = -abs(dy); // Гарантированный отскок вверх
                        break;

                    case 2: // Низ блока
                        tempBallRect.top = block.rect.bottom;
                        tempBallRect.bottom = tempBallRect.top + (originalBallRect.bottom - originalBallRect.top);
                        dy = abs(dy); // Гарантированный отскок вниз
                        break;

                    case 1: // Право блока
                        tempBallRect.left = block.rect.right;
                        tempBallRect.right = tempBallRect.left + (originalBallRect.right - originalBallRect.left);
                        dx = abs(dx); // Вправо
                        break;

                    case 3: // Лево блока
                        tempBallRect.right = block.rect.left;
                        tempBallRect.left = tempBallRect.right - (originalBallRect.right - originalBallRect.left);
                        dx = -abs(dx); // Влево
                        break;
                    }

                    // Добавляем небольшую вариацию
                    dx += (rand() % 3 - 1) * 0.5f;

                    ballRect = tempBallRect;
                    break;
                }
            }

            if (!collisionDetected)
            {
                // Проверка ракетки (с гарантированным отскоком вверх)
                if (CheckCollisionWithSide(tempBallRect, paddleRect, side))
                {
                    collisionDetected = true;

                    // Корректировка позиции
                    tempBallRect.bottom = paddleRect.top;
                    tempBallRect.top = tempBallRect.bottom - (originalBallRect.bottom - originalBallRect.top);

                    // Физика отскока
                    int ballCenterX = (tempBallRect.left + tempBallRect.right) / 2;
                    int paddleCenterX = (paddleRect.left + paddleRect.right) / 2;
                    int hitOffset = (ballCenterX - paddleCenterX) / 10;

                    dy = -abs(dy + ballspeed); // Гарантированный отскок вверх
                    dx += hitOffset;

                    ballRect = tempBallRect;
                }
            }

            if (!collisionDetected)
            {
                // Проверка границ окна
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);

                if (tempBallRect.left <= clientRect.left)
                {
                    tempBallRect.left = clientRect.left;
                    tempBallRect.right = tempBallRect.left + (originalBallRect.right - originalBallRect.left);
                    dx = abs(dx); // Гарантированный отскок вправо
                    collisionDetected = true;
                }
                else if (tempBallRect.right >= clientRect.right)
                {
                    tempBallRect.right = clientRect.right;
                    tempBallRect.left = tempBallRect.right - (originalBallRect.right - originalBallRect.left);
                    dx = -abs(dx); // Гарантированный отскок влево
                    collisionDetected = true;
                }

                if (tempBallRect.top <= clientRect.top)
                {
                    tempBallRect.top = clientRect.top;
                    tempBallRect.bottom = tempBallRect.top + (originalBallRect.bottom - originalBallRect.top);
                    dy = abs(dy); // Гарантированный отскок вниз
                    collisionDetected = true;
                }
                else if (tempBallRect.bottom >= clientRect.bottom)
                {
                    // Мяч упал - сброс позиции
                    SetRect(&ballRect, 390, 530, 410, 550);
                    dy = -abs(dy); // Гарантированный старт вверх
                    collisionDetected = true;
                    break; // Прерываем цикл трассировки
                }

                if (collisionDetected)
                {
                    ballRect = tempBallRect;
                }
            }
        }

        if (!collisionDetected)
        {
            OffsetRect(&ballRect, dx, dy);
        }

        // Управление ракеткой (без изменений)
        if (isLeftPressed)
        {
            OffsetRect(&paddleRect, -5, 0);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            if (paddleRect.left < clientRect.left)
                OffsetRect(&paddleRect, clientRect.left - paddleRect.left, 0);
        }

        if (isRightPressed)
        {
            OffsetRect(&paddleRect, 5, 0);
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            if (paddleRect.right > clientRect.right)
                OffsetRect(&paddleRect, clientRect.right - paddleRect.right, 0);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    
    case WM_KEYDOWN:
    {
        if (wParam == VK_LEFT) isLeftPressed = true;
        if (wParam == VK_RIGHT) isRightPressed = true;
        break;
    }

    case WM_KEYUP:
    {
        if (wParam == VK_LEFT) isLeftPressed = false;
        if (wParam == VK_RIGHT) isRightPressed = false;
        break;
    }
   
    case WM_MOUSEMOVE:
    {
        POINT pt = { (short)LOWORD(lParam),(short)HIWORD(lParam) };
        if (pt.x > paddleRect.left && pt.x < paddleRect.right)
        {
            SetRect(&paddleRect, pt.x - 50, paddleRect.top, pt.x + 50, paddleRect.bottom);
            InvalidateRgn(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_ERASEBKGND:
        return (LRESULT)1; // Say we handled it.


    case WM_PAINT:
    {

        BeginPaint(hwnd, &ps);
        Paint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
    {
        DeleteObject(hBlockBrush);
        DeleteObject(hPaddleBrush);
        DeleteObject(hBallBrush);

        KillTimer(hwnd, 1);

        PostQuitMessage(0);

        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }
    }

    return NULL;
}

#endif // vv2
