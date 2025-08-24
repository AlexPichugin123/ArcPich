
//#define qwer
#ifdef qwer
#endif

///////////////////////////////////
#define qwe
#ifdef qwe
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
static std::vector<Block> blocks;
static RECT paddleRect;
static RECT ballRect;
static bool isLeftPressed = false, isRightPressed = false;

// Объявление функций
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect);
void CheckCollisions(HWND hwnd);

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

// Добавляем функцию Брезенхема в начало (перед CheckCollisions)
//void BresenhamLine(POINT start, POINT end, std::vector<POINT>& points) {
//    int x1 = start.x, y1 = start.y;
//    int x2 = end.x, y2 = end.y;
//
//    const bool steep = (abs(y2 - y1) > abs(x2 - x1));
//    if (steep) {
//        std::swap(x1, y1);
//        std::swap(x2, y2);
//    }
//
//    if (x1 > x2) {
//        std::swap(x1, x2);
//        std::swap(y1, y2);
//    }
//
//    const int dx = x2 - x1;
//    const int dy = abs(y2 - y1);
//
//    int error = dx / 2;
//    const int ystep = (y1 < y2) ? 1 : -1;
//    int y = y1;
//
//    for (int x = x1; x <= x2; x++) {
//        points.push_back(steep ? POINT{ y, x } : POINT{ x, y });
//
//        error -= dy;
//        if (error < 0) {
//            y += ystep;
//            error += dx;
//        }
//    }
//}

void CalculatePredictedTrajectory(HWND hwnd) {
    predictedPoints.clear();

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // Создаем копии параметров для симуляции
    int simDx = dx;
    int simDy = dy;
    RECT simBall = ballRect;

    // Создаем копию блоков для симуляции
    std::vector<Block> simBlocks = blocks;

    for (int i = 0; i < PREDICTION_STEPS; i++) {
        // Сохраняем центр мяча
        POINT center = {
            (simBall.left + simBall.right) / 2,
            (simBall.top + simBall.bottom) / 2
        };

        // Добавляем точки по окружности
        for (float angle = 0; angle < 2 * PI; angle += PI / 8) {
            POINT p;
            p.x = center.x + static_cast<int>((ballsize / 2) * cos(angle));
            p.y = center.y + static_cast<int>((ballsize / 2) * sin(angle));
            predictedPoints.push_back(p);
        }

        // Двигаем мяч
        RECT prevBall = simBall;
        simBall.left += simDx;
        simBall.right += simDx;
        simBall.top += simDy;
        simBall.bottom += simDy;

        POINT newCenter = {
            (simBall.left + simBall.right) / 2,
            (simBall.top + simBall.bottom) / 2
        };

        // Проверяем столкновение с границами
        if (newCenter.x - ballsize / 2 <= clientRect.left ||
            newCenter.x + ballsize / 2 >= clientRect.right) {
            simDx = -simDx;
            simBall = prevBall;
            simBall.left += simDx;
            simBall.right += simDx;
        }

        if (newCenter.y - ballsize / 2 <= clientRect.top) {
            simDy = -simDy;
            simBall = prevBall;
            simBall.top += simDy;
            simBall.bottom += simDy;
        }

        // Если мяч упал вниз, прекращаем симуляцию
        if (newCenter.y + ballsize / 2 >= clientRect.bottom) {
            break;
        }

        // Проверяем столкновение с ракеткой
        POINT ballCenter = {
            (simBall.left + simBall.right) / 2,
            (simBall.top + simBall.bottom) / 2
        };

        if (ballCenter.y + ballsize / 2 >= paddleRect.top &&
            ballCenter.x >= paddleRect.left - ballsize / 2 &&
            ballCenter.x <= paddleRect.right + ballsize / 2) {
            simDy = -abs(simDy);
            simBall = prevBall;
            simBall.top += simDy;
            simBall.bottom += simDy;
        }

        // Проверяем столкновение с блоками
        bool blockCollision = false;
        for (auto& block : simBlocks) {
            if (!block.destroyed) {
                POINT ballCenter = {
                    (simBall.left + simBall.right) / 2,
                    (simBall.top + simBall.bottom) / 2
                };

                // Упрощенная проверка коллизии
                if (ballCenter.x >= block.rect.left - ballsize / 2 &&
                    ballCenter.x <= block.rect.right + ballsize / 2 &&
                    ballCenter.y >= block.rect.top - ballsize / 2 &&
                    ballCenter.y <= block.rect.bottom + ballsize / 2) {

                    // Определяем сторону столкновения
                    bool hitVertical = (ballCenter.y < block.rect.top || ballCenter.y > block.rect.bottom);
                    bool hitHorizontal = (ballCenter.x < block.rect.left || ballCenter.x > block.rect.right);

                    // Отражаем мяч
                    if (hitVertical && !hitHorizontal) {
                        simDy = -simDy;
                    }
                    else if (hitHorizontal && !hitVertical) {
                        simDx = -simDx;
                    }
                    else {
                        simDx = -simDx;
                        simDy = -simDy;
                    }

                    block.destroyed = true;
                    blockCollision = true;
                    simBall = prevBall;
                    simBall.left += simDx;
                    simBall.right += simDx;
                    simBall.top += simDy;
                    simBall.bottom += simDy;
                    break;
                }
            }
        }

        // Если была коллизия с блоком, продолжаем с новым направлением
        if (blockCollision) {
            continue;
        }
    }
}

// Упрощённая проверка коллизий
void CheckCollisions(HWND hwnd) 
{
    tracePoints.clear();
    // Сохраняем начальную позицию мяча
    POINT startPos = { ballRect.left + ballsize / 2, ballRect.top + ballsize / 2 };
    POINT endPos = { startPos.x + dx, startPos.y + dy };

    // Трассировка (для предотвращения проскакивания)
    std::vector<POINT> trace;
    trace.push_back(startPos);
    // Разбиваем движение на подшаги
    bool collisionDetected = false;
    int steps = sqrt(dy * dy + dx * dx);

    POINT NormalizeVector; 
    NormalizeVector.x = dx / steps, NormalizeVector.y = dy / steps; //вектор единичной длинны
    float Angle = atan2(NormalizeVector.y, NormalizeVector.x);// находим угол между вектором движения и осью Х
    POINT Sphere;
    float Predel;
    
    for (int i = 1; i <= steps; ++i) 
    {
        for (Predel = -PI / 4; Predel <= PI / 4; Predel += PI / 4)
        {
            // Промежуточная позиция
            POINT currentPos = 
            {
                startPos.x + (dx * i) / steps,
                startPos.y + (dy * i) / steps
            };
            trace.push_back(currentPos);
            tracePoints.push_back(Sphere);

            // Проверяем коллизию с блоками
            for (auto& block : blocks) 
            {
                Sphere.x = currentPos.x + ((ballsize / 2) * cos(Predel + Angle));
                Sphere.y = currentPos.y + ((ballsize / 2) * sin(Predel + Angle));

                  //надо этот цикл с пределом
                  if ( !block.destroyed &&

                      Sphere.x >= block.rect.left - ballsize / 2 &&
                      Sphere.x <= block.rect.right + ballsize / 2 &&
                      Sphere.y >= block.rect.top - ballsize / 2 &&
                      Sphere.y <= block.rect.bottom + ballsize / 2  )

                        /*currentPos.x >= block.rect.left - ballsize / 2 &&
                        currentPos.x <= block.rect.right + ballsize / 2 &&
                        currentPos.y >= block.rect.top - ballsize / 2 &&
                        currentPos.y <= block.rect.bottom + ballsize / 2)*/
                  {

                        // Нашли коллизию - откатываем к предыдущей позиции
                        currentPos = trace[trace.size()-2];
                        collisionDetected = true;

                        // Определяем сторону столкновения
                        bool hitVertical = (Sphere.y < block.rect.top || Sphere.y > block.rect.bottom);
                        bool hitHorizontal = (Sphere.x < block.rect.left || Sphere.x > block.rect.right);

                        // Отражаем мяч
                        if (hitVertical && !hitHorizontal) dy = -dy;
                        else if (hitHorizontal && !hitVertical) dx = -dx;
                        else /*if (Predel != -PI/2 || Predel != PI/2)*/ //убрал потому что при крайних значениях шарик должен проскакивать
                        {
                            // Угловое столкновение
                            dx = -dx;
                            dy = -dy;
                        }
                    

                        block.destroyed = true;
                        break;
                  }
            }
        }

        if (collisionDetected) break;
    }

    // Проверка ракетки
    POINT ballCenter = trace.back();
    if (ballCenter.y + ballsize / 2 >= paddleRect.top &&
        ballCenter.x >= paddleRect.left - ballsize / 2 &&
        ballCenter.x <= paddleRect.right + ballsize / 2) 
    {
      
        dy = -abs(dy);
        ballCenter.y = paddleRect.top - ballsize / 2;
    }

    // Проверка границ окна
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    if (ballCenter.x - ballsize / 2 <= clientRect.left ||
        ballCenter.x + ballsize / 2 >= clientRect.right) {
        dx = -dx;
    }

    if (ballCenter.y - ballsize / 2 <= clientRect.top) {
        dy = -dy;
    }

    // Если мяч упал вниз
    if (ballCenter.y + ballsize / 2 >= clientRect.bottom) {
        SetRect(&ballRect, 390, 530, 410, 550);
        dy = dy1;
        return;
    }
    CalculatePredictedTrajectory(hwnd);

    // Обновляем позицию мяча
    SetRect(&ballRect,
        ballCenter.x - ballsize / 2,
        ballCenter.y - ballsize / 2,
        ballCenter.x + ballsize / 2,
        ballCenter.y + ballsize / 2);

    
}



// Отрисовка
static void Paint(HWND hwnd, LPPAINTSTRUCT lpPS) {
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    HBRUSH hbrBkGnd;

    GetClientRect(hwnd, &rc);
    hdcMem = CreateCompatibleDC(lpPS->hdc);
    hbmMem = CreateCompatibleBitmap(lpPS->hdc, rc.right - rc.left, rc.bottom - rc.top);
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // Очищаем фон
    hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rc, hbrBkGnd);
    DeleteObject(hbrBkGnd);


    // Отрисовка ракетки
    {
        HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        FillRgn(hdcMem, hPaddleRegion, hPaddleBrush);
        DeleteObject(hPaddleRegion);
    }

    // Отрисовка мяча
    {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBallBrush);
        Ellipse(hdcMem, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
        SelectObject(hdcMem, hOldBrush);
    }

    // Отрисовка блоков
    for (const auto& block : blocks) {
        if (!block.destroyed) {
            HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
            //FillRect(hdcMem, &rc, hBlockBrush); 
            FillRgn(hdcMem, hRegion, hBlockBrush);
            DeleteObject(hRegion);
            
        }
    }

    // Отрисовка прогнозируемой траектории
    HBRUSH hPredictionBrush = CreateSolidBrush(RGB(0, 200, 0)); // Темно-зеленый
    HPEN hPredictionPen = CreatePen(PS_SOLID, 1, RGB(0, 150, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPredictionPen);

    // Рисуем точки траектории
    for (int i = 0; i < predictedPoints.size(); i++) {
        // Каждую 8-ю точку рисуем больше для лучшей видимости
        if (i % 8 == 0) {
            Ellipse(hdcMem,
                predictedPoints[i].x - 2,
                predictedPoints[i].y - 2,
                predictedPoints[i].x + 2,
                predictedPoints[i].y + 2);
        }
        else {
            Ellipse(hdcMem,
                predictedPoints[i].x - 1,
                predictedPoints[i].y - 1,
                predictedPoints[i].x + 1,
                predictedPoints[i].y + 1);
        }
    }

    SelectObject(hdcMem, hOldPen);
    DeleteObject(hPredictionPen);
    DeleteObject(hPredictionBrush);

    BitBlt(lpPS->hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

// Обработчик сообщений
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;

    switch (msg) {
    case WM_CREATE: {
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, TimPer, NULL);
        break;
    }

    case WM_TIMER: {
        CheckCollisions(hwnd);

        // Управление ракеткой
        if (isLeftPressed) {
            OffsetRect(&paddleRect, -10, 0);
            if (paddleRect.left < 0)
                SetRect(&paddleRect, 0, paddleRect.top, 100, paddleRect.bottom);
        }

        if (isRightPressed) {
            OffsetRect(&paddleRect, 10, 0);
            if (paddleRect.right > 800)
                SetRect(&paddleRect, 700, paddleRect.top, 800, paddleRect.bottom);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_KEYDOWN: {
        if (wParam == VK_LEFT) isLeftPressed = true;
        if (wParam == VK_RIGHT) isRightPressed = true;
        break;
    }

    case WM_KEYUP: {
        if (wParam == VK_LEFT) isLeftPressed = false;
        if (wParam == VK_RIGHT) isRightPressed = false;
        break;
    }

    case WM_MOUSEMOVE: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        if (pt.x > paddleRect.left && pt.x < paddleRect.right) {
            SetRect(&paddleRect, pt.x - 50, paddleRect.top, pt.x + 50, paddleRect.bottom);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_ERASEBKGND:
        return (LRESULT)1;

    case WM_PAINT: {
        BeginPaint(hwnd, &ps);
        Paint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY: {
        DeleteObject(hBlockBrush);
        DeleteObject(hPaddleBrush);
        DeleteObject(hBallBrush);
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return NULL;
}
#endif qwe
