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
#include <chrono>

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
float dx = 3, dy = -50, dy1=dy; //dy1 = скорость после сброса
int TimPer = 200;
const int steps = 300; // число подшагов, увеличить для большей точности
double stepDx = dx / steps;
double stepDy = dy / steps;
int newX;
int newY;
static std::vector<Block> blocks;
static RECT paddleRect;
static RECT ballRect;
static bool isLeftPressed = false, isRightPressed = false;
std::chrono::steady_clock::time_point prevTime;
double deltaTime = 0.0; // Время между кадрами в секундах

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
    HFONT hfntOld, hfnt;

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

//коллизия
bool LineIntersectsRect(const POINT& p1, const POINT& p2, const RECT& rect, double& t)
{
    // Функции для проверки пересечения линий
    auto LineSegmentsIntersect = [](double x1, double y1, double x2, double y2,
        double x3, double y3, double x4, double y4,
        double& t)
        {
            // Вычисляем параметры
            double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (fabs(denom) < 1e-8)
                return false; // параллельны

            double t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
            t = t_num / denom;

            double u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
            double u = u_num / denom;

            if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
                return true;
            return false;
        };

    // Определяем стороны прямоугольника
    POINT sides[4][2] = {
        { {rect.left, rect.top}, {rect.right, rect.top} },//top
        { {rect.right, rect.top}, {rect.right, rect.bottom} },//right
        { {rect.right, rect.bottom}, {rect.left, rect.bottom} },//bottom
        { {rect.left, rect.bottom}, {rect.left, rect.top} }//left
    };

    bool hit = false;
    double minT = 1.0;

    for (int i = 0; i < 4; ++i)
    {
        double tTemp;
        if (LineSegmentsIntersect(p1.x, p1.y, p2.x, p2.y,
            sides[i][0].x, sides[i][0].y, sides[i][1].x, sides[i][1].y,
            tTemp))
        {
            if (tTemp <= minT)
            {
                minT = tTemp;
                hit = true;
            }
        }
    }

    if (hit)
    {
        t = minT;
        return true;
    }
    return false;
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
        //auto window_width = r.right - r.left;//определяем размеры и сохраняем
        //auto window_height = r.bottom - r.top;
        InitGameObjects(blocks, paddleRect, ballRect);
        prevTime = std::chrono::steady_clock::now();
        SetTimer(hwnd, 1, TimPer, NULL); // 1000/2ой параматер = фпс, таймер для обновления игры

        break;
    }

    case WM_TIMER:
    {
        int ballsize = ballRect.right - ballRect.left;
        // Начальные и конечные позиции центра мяча
        POINT startPos = { ballRect.left + ballsize / 2 , ballRect.top + ballsize / 2 };
        POINT endPos = { startPos.x + dx, startPos.y + dy};
        POINT currentPos = startPos;
        bool collisionDetected = false;
        double collisionT = 1.0; // по умолчанию — полного пути

        for (int i = 0; i < steps; ++i)
        {
            POINT nextPos = { static_cast<int>(currentPos.x + stepDx), static_cast<int>(currentPos.y + stepDy) };


            // Проверка столкновений с блоками
            for (auto& block : blocks)
            {
                if (!block.destroyed)
                {
                    RECT blockRect = block.rect;
                    double tTemp;
                    if (LineIntersectsRect({ currentPos.x, currentPos.y }, { nextPos.x, nextPos.y }, blockRect, tTemp))
                    {
                        // Обновляем позицию до точки столкновения
                        int collideX = static_cast<int>(currentPos.x + stepDx * tTemp);
                        int collideY = static_cast<int>(currentPos.y + stepDy * tTemp);
                       
                        // Определяем сторону столкновения
                        if (collideX < blockRect.right || collideX > blockRect.left) 
                        {
                            // Столкновение с боковой стороной
                            dy = -dy; // Отразить по X
                        }
                        else 
                        {
                            // Столкновение с верхней или нижней стороной
                            dx = -dx; // Отразить по Y
                        }

                        // Удаляем блок или помечаем как уничтоженный
                        block.destroyed = true;

                        // Обновляем позицию мяча
                       /*int ballsize = ballRect.right - ballRect.left;
                       SetRect(&ballRect,
                           collideX - ballsize / 2,
                           collideY - ballsize / 2,
                           collideX + ballsize / 2,
                           collideY + ballsize / 2);

                        InvalidateRect(hwnd, NULL, TRUE);*/
                        collisionDetected = true;
                        //break; // Можно продолжить или прервать по необходимости
                    }
                }
            }
            if (collisionDetected)
                break;
            currentPos = nextPos;
        }
            if (!collisionDetected)
            {
                int ballsize = ballRect.right - ballRect.left;
                // Обновляем позицию мяча на основе полного перемещения
                    newX = startPos.x + static_cast<int>(dx);
                    newY = startPos.y + static_cast<int>(dy);

                // Проверка ракетки (если мяч касается или ниже верхней части ракетки)
                if (newY + ballsize / 2 >= paddleRect.top &&
                    newX >= paddleRect.left && newX <= paddleRect.right)
                {
                    dy = -abs(dy); // отскок вверх
                    newY = paddleRect.top - ballsize / 2; // корректируем позицию
                }

                // Проверка границ окна
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);

                if (newX - ballsize / 2 <= clientRect.left || newX + ballsize / 2 >= clientRect.right)
                    dx = -dx;

                if (newY - ballsize / 2 <= clientRect.top)
                    dy = -dy;

                if (newY + ballsize / 2 >= clientRect.bottom)
                {
                    // Мяч упал вниз — сбросить позицию или обработать проигрыш
                    SetRect(&ballRect, 390, 530, 410, 550);
                    dy = dy1; // восстановить скорость по Y или задать новую
                    break;
                }

                // Обновляем позицию мяча только после всех проверок
                SetRect(&ballRect,
                    newX - ballsize / 2,
                    newY - ballsize / 2,
                    newX + ballsize / 2,
                    newY + ballsize / 2);
            }

            

            // Обработка управления ракеткой и перерисовка...
            if (isLeftPressed) 
            {
                OffsetRect(&paddleRect, -5, 0);
                // Проверка границ
                if (paddleRect.left < 0)
                    SetRect(&paddleRect, 0, paddleRect.top, 100, paddleRect.bottom);
            }

            if (isRightPressed) 
            {
                OffsetRect(&paddleRect, 5, 0);
                // Проверка границ
                if (paddleRect.right > 800)
                    SetRect(&paddleRect, 700, paddleRect.top, 800, paddleRect.bottom);
            }

            InvalidateRect(hwnd, NULL, TRUE); // перерисовать окно

            break;
        }   //старая проверка
        /* Проверка столкновения с границами окна
            if (ballRect.left <= 0 || ballRect.right >= 800)
            {
                //OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // отражение по X
                dx = -dx;

            }

            if (ballRect.top <= 0)
            {
                //OffsetRect(&ballRect, 0, -dy / abs(dy)); // отражение по Y
                dy = -dy;

            }

        if (ballRect.bottom >= 650)
        {
            // Мяч упал вниз - сбросим позицию
            SetRect(&ballRect, 390, 530, 410, 550);
            dy = -4;
        }

        // Столкновение с ракеткой
        /////
        const float M_PI = 3.14159265358979323846f;
        const float MAX_ANGLE = 75 * M_PI / 180; // переводим в радианы
        float ballX = (ballRect.right - ballRect.left) / 2;
        float ballY = (ballRect.bottom - ballRect.top) / 2;
        float ballrad = ballX;
        float paddleX = (paddleRect.right - paddleRect.left) / 2;
        float paddleY = (paddleRect.bottom - paddleRect.top) / 2;
        float paddleW = paddleX * 2;

         if (ballRect.bottom >= paddleRect.top)
            {

                // Расчет точки удара по ракетке
             float hitPos = ((ballRect.right - ballrad) - (paddleRect.right - paddleX)) / (paddleW);

                if (hitPos < 0) hitPos = 0;
                if (hitPos > 1) hitPos = 1;

                // Угол отклонения
                float angle = (hitPos - 0.5f) * MAX_ANGLE;

                // Текущая скорость мяча
                float speed = sqrt(dx * dx + dy * dy);

                // Обновление скорости мяча
                dx = speed * sin(angle);
                dy = -abs(speed * cos(angle));
            }
        /////
        if (ballRect.right >= paddleRect.left && ballRect.left <= paddleRect.right &&
            ballRect.bottom >= paddleRect.top && ballRect.top <= paddleRect.bottom)
        {
            dy = -abs(dy);
        }


        // Проверка столкновения с блоками
        for (auto& block : blocks)
        {
            if (!block.destroyed && ballRect.right >= block.rect.left && ballRect.left <= block.rect.right &&
                ballRect.bottom >= block.rect.top && ballRect.top <= block.rect.bottom) //intersectrect не регистрирует столкновения
            {
                block.destroyed = true;
                //OffsetRect(&ballRect, 0, -(ballRect.bottom - block.rect.top));
                dy = -dy;
                break;
            }
        }

        // Обновление позиции мяча
        OffsetRect(&ballRect, dx, dy);

        if (isLeftPressed) 
        {
            OffsetRect(&paddleRect, -5, 0);
            // Проверка границ
            if (paddleRect.left < 0)
                SetRect(&paddleRect, 0, paddleRect.top, 100, paddleRect.bottom);
        }

        if (isRightPressed) 
        {
            OffsetRect(&paddleRect, 5, 0);
            // Проверка границ
            if (paddleRect.right > 800)
                SetRect(&paddleRect, 700, paddleRect.top, 800, paddleRect.bottom);
        }

        InvalidateRgn(hwnd, NULL, FALSE); // перерисовать окно

        break;

    } */

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
