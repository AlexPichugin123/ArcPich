//#define vv1 включает первую версию проги
#ifdef vv1

#include <windows.h>
#include <vector>
#include <cmath>
#include <random>
#include "Parametrs.h"

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// Ñòðóêòóðà áëîêà
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};
// Ãëîáàëüíûå ïåðåìåííûå
HBRUSH hBlockBrush; // êèñòü äëÿ áëîêîâ
HBRUSH hPaddleBrush; // êèñòü äëÿ ðàêåòêè
HBRUSH hBallBrush;   // êèñòü äëÿ ìÿ÷à
//float dy = -45; //(rand() % 10 + 5);//ôîðìèðóåì âåêòîð ïîëåòà øàðèêà
//float dx = 2;//-(1 - dy);//ôîðìèðóåì âåêòîð ïîëåòà øàðèêà
//int /*dx=0, dy=-5,*/ dy1=-3; //dy1 = ñêîðîñòü ïîñëå ñáðîñà
//int TimPer = 500;
//int ballspeed=0;
//const int steps = sqrt(dy*dy+dx*dx); // ÷èñëî ïîäøàãîâ, ïîñòàâèòü ðàâíûì êîëè÷åñòâó ïèêñëåëåé ìåæäó íà÷ è êîí òî÷êîé äâèæåíèÿ øàðèêà çà 1 êàäð 
////àëãîðèòì áðåçåíõåìà äëÿ ïðîâåðêè êîëëèçèè
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

// Îáúÿâëåíèå ôóíêöèé
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
        L"Àðêàíîéä ñ HBRUSH",
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
// Èíèöèàëèçàöèÿ èãðîâûõ îáúåêòîâ
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // Ñîçäàåì êèñòè
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // êðàñíûå áëîêè
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // ñèíÿÿ ðàêåòêà
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // æåëòûé ìÿ÷
    // Ñîçäàåì áëîêè
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

    // Ñîçäàåì ðàêåòêó
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;

    // Ñîçäàåì ìÿ÷
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;

}
//íåïîñðåäòñâåííî îòðèñîâêà
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
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);//òóò ñ ïðåîáðàçîâàíèåì òèïîâ äàííûõ ïðèêîë

    //÷èñòèì ôîí
    hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rc, hbrBkGnd);
    DeleteObject(hbrBkGnd);


    // Îòðèñîâêà ðàêåòêè
    {
        HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        FillRgn(hdcMem, hPaddleRegion, hPaddleBrush);
        DeleteObject(hPaddleRegion);

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hPaddleBrush);
        Rectangle(hdcMem, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        SelectObject(hdcMem, hOldBrush);

    }
    // Îòðèñîâêà ìÿ÷à
    {

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBallBrush);
        Ellipse(hdcMem, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
        SelectObject(hdcMem, hOldBrush);
        //SetPixel(hdcMem, currentPos1.x, currentPos1.y, RGB(0, 250, 0)); //íàäî êàê-òî òðàñèðîâêó îòîáðàçèòü 


    }

    // Îòðèñîâêà áëîêîâ
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

// Óïðîùåííàÿ ïðîâåðêà êîëëèçèè ñ îïðåäåëåíèåì ñòîðîíû
bool CheckCollisionWithSide(const RECT& ball, const RECT& block, int& collisionSide)
{
    if (!(ball.right > block.left && ball.left < block.right &&
        ball.bottom > block.top && ball.top < block.bottom))
        return false;

    //// Îïðåäåëÿåì ñòîðîíó ñòîëêíîâåíèÿ
    int ballCenterX = (ball.left + ball.right) / 2;
    int ballCenterY = (ball.top + ball.bottom) / 2;
    int blockCenterX = (block.left + block.right) / 2;
    int blockCenterY = (block.top + block.bottom) / 2;

    // Âû÷èñëÿåì ïåðåêðûòèÿ ïî êàæäîé îñè
    int overlapLeft = ball.right - block.left;
    int overlapRight = block.right - ball.left;
    int overlapTop = ball.bottom - block.top;
    int overlapBottom = block.bottom - ball.top;

    // Íàõîäèì ìèíèìàëüíîå ïåðåêðûòèå
    int minOverlap = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;

    if (minOverlap == overlapTop) collisionSide = 0;    // Âåðõ
    else if (minOverlap == overlapRight) collisionSide = 1; // Ïðàâî
    else if (minOverlap == overlapBottom) collisionSide = 2; // Íèç
    else collisionSide = 3; // Ëåâî

    return true;
}

//îñíîâíàÿ ôóíêöèÿ
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
        SetTimer(hwnd, 1, TimPer, NULL); // 1000/2îé ïàðàìàòåð = ôïñ, òàéìåð äëÿ îáíîâëåíèÿ èãðû

        break;
    }

    case WM_TIMER:
    {
        // Òðàññèðîâêà äâèæåíèÿ ìÿ÷à ñ øàãàìè
        bool collisionDetected = false;
        RECT originalBallRect = ballRect;

        for (int i = 1; i <= steps && !collisionDetected; ++i)
        {
            // Ïðîìåæóòî÷íàÿ ïîçèöèÿ ìÿ÷à   
            RECT tempBallRect = originalBallRect;
            OffsetRect(&tempBallRect, dx * i / steps, dy * i / steps);

            // Ïðîâåðêà ñòîëêíîâåíèé ñ áëîêàìè
            for (auto& block : blocks)
            {
                if (!block.destroyed && CheckCollisionWithSide(tempBallRect, block.rect, side))
                {
                    collisionDetected = true;
                    block.destroyed = true;

                    // Êîððåêòèðóåì ïîçèöèþ ïåðåä îòñêîêîì
                    switch (side)
                    {
                    case 0: // Âåðõ áëîêà
                        tempBallRect.bottom = block.rect.top;
                        tempBallRect.top = tempBallRect.bottom - (originalBallRect.bottom - originalBallRect.top);
                        dy = -dy; // Ãàðàíòèðîâàííûé îòñêîê ââåðõ
                        break;

                    case 2: // Íèç áëîêà
                        tempBallRect.top = block.rect.bottom;
                        tempBallRect.bottom = tempBallRect.top + (originalBallRect.bottom - originalBallRect.top);
                        dy = -dy; // Ãàðàíòèðîâàííûé îòñêîê âíèç
                        break;

                    case 1: // Ïðàâî áëîêà
                        tempBallRect.left = block.rect.right;
                        tempBallRect.right = tempBallRect.left + (originalBallRect.right - originalBallRect.left);
                        dx = -dx; // Âïðàâî
                        break;

                    case 3: // Ëåâî áëîêà
                        tempBallRect.right = block.rect.left;
                        tempBallRect.left = tempBallRect.right - (originalBallRect.right - originalBallRect.left);
                        dx = -dx; // Âëåâî
                        break;
                    }


                    ballRect = tempBallRect;
                    //break;
                }
            }

            if (!collisionDetected)
            {
                // Ïðîâåðêà ðàêåòêè (ñ ãàðàíòèðîâàííûì îòñêîêîì ââåðõ)
                if (CheckCollisionWithSide(tempBallRect, paddleRect, side))
                {
                    collisionDetected = true;

                    // Êîððåêòèðîâêà ïîçèöèè
                    tempBallRect.bottom = paddleRect.top;
                    tempBallRect.top = tempBallRect.bottom - (originalBallRect.bottom - originalBallRect.top);

                    // Ôèçèêà îòñêîêà
                    int ballCenterX = (tempBallRect.left + tempBallRect.right) / 2;
                    int paddleCenterX = (paddleRect.left + paddleRect.right) / 2;
                    int hitOffset = (ballCenterX - paddleCenterX) / 10;

                    dy = -dy; // Ãàðàíòèðîâàííûé îòñêîê ââåðõ
                    dx += hitOffset;

                    ballRect = tempBallRect;
                }
            }

            if (!collisionDetected)
            {
                // Ïðîâåðêà ãðàíèö îêíà
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);

                if (tempBallRect.left <= clientRect.left)
                {
                    tempBallRect.left = clientRect.left;
                    tempBallRect.right = tempBallRect.left + (originalBallRect.right - originalBallRect.left);
                    dx = abs(dx); // Ãàðàíòèðîâàííûé îòñêîê âïðàâî
                    collisionDetected = true;
                }
                else if (tempBallRect.right >= clientRect.right)
                {
                    tempBallRect.right = clientRect.right;
                    tempBallRect.left = tempBallRect.right - (originalBallRect.right - originalBallRect.left);
                    dx = -abs(dx); // Ãàðàíòèðîâàííûé îòñêîê âëåâî
                    collisionDetected = true;
                }

                if (tempBallRect.top <= clientRect.top)
                {
                    tempBallRect.top = clientRect.top;
                    tempBallRect.bottom = tempBallRect.top + (originalBallRect.bottom - originalBallRect.top);
                    dy = abs(dy); // Ãàðàíòèðîâàííûé îòñêîê âíèç
                    collisionDetected = true;
                }
                else if (tempBallRect.bottom >= clientRect.bottom)
                {
                    // Ìÿ÷ óïàë - ñáðîñ ïîçèöèè
                    SetRect(&ballRect, 390, 530, 410, 550);
                    dy = -dy; // Ãàðàíòèðîâàííûé ñòàðò ââåðõ
                    collisionDetected = true;
                    break; // Ïðåðûâàåì öèêë òðàññèðîâêè
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

        // Óïðàâëåíèå ðàêåòêîé (áåç èçìåíåíèé)
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

// Упрощённая проверка коллизий
void CheckCollisions(HWND hwnd) {
    // Сохраняем начальную позицию мяча
    POINT startPos = { ballRect.left + ballsize / 2, ballRect.top + ballsize / 2 };
    POINT endPos = { startPos.x + dx, startPos.y + dy };

    // Трассировка (для предотвращения проскакивания)
    std::vector<POINT> trace;
    trace.push_back(startPos);

    // Разбиваем движение на подшаги
    bool collisionDetected = false;

    for (int i = 1; i <= steps; ++i) {
        // Промежуточная позиция
        POINT currentPos = {
            startPos.x + (dx * i) / steps,
            startPos.y + (dy * i) / steps
        };
        trace.push_back(currentPos);

        // Проверяем коллизию с блоками
        for (auto& block : blocks) {
            if (!block.destroyed &&
                currentPos.x >= block.rect.left - ballsize / 2 &&
                currentPos.x <= block.rect.right + ballsize / 2 &&
                currentPos.y >= block.rect.top - ballsize / 2 &&
                currentPos.y <= block.rect.bottom + ballsize / 2) {

                // Нашли коллизию - откатываем к предыдущей позиции
                currentPos = trace[trace.size() - 2];
                collisionDetected = true;

                // Определяем сторону столкновения
                bool hitVertical = (currentPos.y < block.rect.top || currentPos.y > block.rect.bottom);
                bool hitHorizontal = (currentPos.x < block.rect.left || currentPos.x > block.rect.right);

                // Отражаем мяч
                if (hitVertical && !hitHorizontal) dy = -dy;
                else if (hitHorizontal && !hitVertical) dx = -dx;
                else {
                    // Угловое столкновение
                    dx = -dx;
                    dy = -dy;
                }

                block.destroyed = true;
                break;
            }
        }

        if (collisionDetected) break;
    }

    // Проверка ракетки
    POINT ballCenter = trace.back();
    if (ballCenter.y + ballsize / 2 >= paddleRect.top &&
        ballCenter.x >= paddleRect.left - ballsize / 2 &&
        ballCenter.x <= paddleRect.right + ballsize / 2) {
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
            FillRgn(hdcMem, hRegion, hBlockBrush);
            DeleteObject(hRegion);
        }
    }

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
            OffsetRect(&paddleRect, -5, 0);
            if (paddleRect.left < 0)
                SetRect(&paddleRect, 0, paddleRect.top, 100, paddleRect.bottom);
        }

        if (isRightPressed) {
            OffsetRect(&paddleRect, 5, 0);
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
#endif // vv2
