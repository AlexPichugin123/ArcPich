//#define vv1 �������� ������ ������ �����
#ifdef vv1

#include <windows.h>
#include <vector>

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// ��������� �����
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};

// ���������� ����������
HBRUSH hBlockBrush; // ����� ��� ������
HBRUSH hPaddleBrush; // ����� ��� �������
HBRUSH hBallBrush;   // ����� ��� ����

// ���������� �������
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
        L"�������� � HBRUSH",
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

// ������������� ������� ��������
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // ������� �����
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // ������� �����
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // ����� �������
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // ������ ���

    // ������� �����
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

    // ������� �������
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;

    // ������� ���
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;

}

// ��������� ��������� ����
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static std::vector<Block> blocks;
    static RECT paddleRect;
    static RECT ballRect;

    static bool isLeftPressed = false;

    switch (msg) {
    case WM_CREATE:
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, 16, NULL); // ������ ��� ���������� ���� (~60 fps)
        return 0;

    case WM_TIMER:
        // ���������� ��������� ���� � �������� ������������

    {
        int dx = 4 * (ballRect.left <= (paddleRect.left + 2) ? 1 : (ballRect.right >= paddleRect.right ? -1 : 1));
        int dy = -4;

        // ������������ ���� �� X � Y
        OffsetRect(&ballRect, dx / abs(dx), dy / abs(dy));
        // �������� ������������ � ��������� ����
        if (ballRect.left <= 0 && ballRect.right >= 800)
            OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // ��������� �� X

        if (ballRect.top <= 0)
            OffsetRect(&ballRect, 0, -dy / abs(dy)); // ��������� �� Y

        if (ballRect.bottom >= 600) {
            // ��� ���� ���� - ������� �������
            SetRect(&ballRect, 390, 530, 410, 550);
        }

        // ������������ � ��������
        if (IntersectRect(NULL, &ballRect, &paddleRect)) {
            OffsetRect(&ballRect, 0, -(ballRect.bottom - paddleRect.top));
        }

        // �������� ������������ � �������
        for (auto& block : blocks) {
            if (!block.destroyed && IntersectRect(NULL, &ballRect, &block.rect)) {
                block.destroyed = true;
                OffsetRect(&ballRect, 0, -(ballRect.bottom - block.rect.top));
                break;
            }
        }
    }

    InvalidateRgn(hwnd, NULL, FALSE); // ������������ ����
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

        // ������� ����� ��� �����
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // ������ �����

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }

        // ��������� ������
        for (const auto& block : blocks) {
            if (!block.destroyed) {
                HRGN hRegion = CreateRectRgn(block.rect.left, block.rect.top, block.rect.right, block.rect.bottom);
                FillRgn(hdc, hRegion, hBlockBrush);
                DeleteObject(hRegion);
            }
        }
        DeleteObject(hPen);
        // ��������� �������
        {
            HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            FillRgn(hdc, hPaddleRegion, hPaddleBrush);
            DeleteObject(hPaddleRegion);

            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPaddleBrush);
            Rectangle(hdc, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
            SelectObject(hdc, hOldBrush);
        }

        // ��������� ����
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

const wchar_t szClassName[] = L"ArkanoidHBRUSH";

// ��������� �����
struct Block {
    RECT rect;
    HBRUSH brush;
    bool destroyed;
};

// ���������� ����������
HBRUSH hBlockBrush; // ����� ��� ������
HBRUSH hPaddleBrush; // ����� ��� �������
HBRUSH hBallBrush;   // ����� ��� ����
float dx = 2, dy = -4;
static std::vector<Block> blocks;
static RECT paddleRect;
static RECT ballRect;
static bool isLeftPressed = false, isRightPressed = false;

// ���������� �������
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
        L"�������� � HBRUSH",
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

// ������������� ������� ��������
void InitGameObjects(std::vector<Block>& blocks, RECT& paddleRect, RECT& ballRect) {
    // ������� �����
    hBlockBrush = CreateSolidBrush(RGB(200, 50, 50));   // ������� �����
    hPaddleBrush = CreateSolidBrush(RGB(50, 50, 200)); // ����� �������
    hBallBrush = CreateSolidBrush(RGB(255, 255, 0));   // ������ ���

    // ������� �����
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

    // ������� �������
    paddleRect.left = 350; paddleRect.top = 550; paddleRect.right = 450; paddleRect.bottom = 570;
    

    // ������� ���
    ballRect.left = 390; ballRect.top = 530; ballRect.right = 410; ballRect.bottom = 550;
    

}

int ballsize = ballRect.right - ballRect.left;
//��������������� ���������
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
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);//��� � ��������������� ����� ������ ������
    

    //������ ���
    hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdcMem, &rc, hbrBkGnd);
    DeleteObject(hbrBkGnd);


    // ��������� �������
    {
        HRGN hPaddleRegion = CreateRectRgn(paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        FillRgn(hdcMem, hPaddleRegion, hPaddleBrush);
        DeleteObject(hPaddleRegion);

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hPaddleBrush);
        Rectangle(hdcMem, paddleRect.left, paddleRect.top, paddleRect.right, paddleRect.bottom);
        SelectObject(hdcMem, hOldBrush);
    }

    // ��������� ����
    {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBallBrush);
        Ellipse(hdcMem, ballRect.left, ballRect.top, ballRect.right, ballRect.bottom);
        SelectObject(hdcMem, hOldBrush);
    }
    
    // ��������� ������
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

//��������
bool LineIntersectsRect(const POINT& p1, const POINT& p2, const RECT& rect, double& t)
{
    // ������� ��� �������� ����������� �����
    auto LineSegmentsIntersect = [](double x1, double y1, double x2, double y2,
        double x3, double y3, double x4, double y4,
        double& t)
        {
            // ��������� ���������
            double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (fabs(denom) < 1e-8)
                return false; // �����������

            double t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
            t = t_num / denom;

            double u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
            double u = u_num / denom;

            if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
                return true;
            return false;
        };

    // ���������� ������� ��������������
    POINT sides[4][2] = {
        { {rect.left, rect.top}, {rect.right, rect.top} },
        { {rect.right, rect.top}, {rect.right, rect.bottom} },
        { {rect.right, rect.bottom}, {rect.left, rect.bottom} },
        { {rect.left, rect.bottom}, {rect.left, rect.top} }
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
            if (tTemp < minT)
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

//�������� �������
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    
    int TimPer = 200;
    switch (msg)
    {
    case WM_CREATE:
    {
        
        RECT r;
        GetClientRect(hwnd, &r);
        auto window_width = r.right - r.left;//���������� ������� � ���������
        auto window_height = r.bottom - r.top;
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, TimPer, NULL); // 1000/2�� ��������� = ���, ������ ��� ���������� ���� (~60 fps)

        break;
    }

    case WM_TIMER:
    {
        // ���������� ��������� ���� � �������� ������������
        // ����� ����
        const double deltaTime = 1/(1000/TimPer);

        // ��������� ������� ������ ����
        POINT startPos = { ballRect.left + (ballsize) / 2 , ballRect.top + (ballsize) / 2 };

        // �������� ������� �� ���� ���
        POINT endPos = { startPos.x + dx * deltaTime * 1000 , startPos.y + dy * deltaTime * 1000 };

            // dx/dy ��� ������� �� ����
            endPos.x = startPos.x + dx;
            endPos.y = startPos.y + dy;

        bool collisionDetected = false;
        double collisionT;

        // �������� ������������ � �������
        for (auto it = blocks.begin(); it != blocks.end(); )
        {
            RECT blockRect = it->rect;
            if (LineIntersectsRect(startPos, endPos, blockRect, collisionT))
            {
                // ��������� ������� �� ����� ������������
                int newX = static_cast<int>(startPos.x + dx * collisionT);
                int newY = static_cast<int>(startPos.y + dy * collisionT);

                // ��������� ������� ����
                ballRect.left = newX - (ballsize) / 2;
                ballRect.top = newY - (ballsize) / 2;

                // ������ ����������� �� ��� X ��� Y � ����������� �� ������� ������������
                // ��� �������� ����������� ��������� �� ����� ����:
                dx = -dx;
                dy = -dy;

                // ������� ����
                it = blocks.erase(it);

                collisionDetected = true;
                //break; // ����� ���������� �������� ����� ��������� ������� ������������
            }
            else
                ++it;
        }

        if (!collisionDetected)
        {
            // �������� ������������ �� ������� ����
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);

            if (endPos.x <= clientRect.left + (ballsize) / 2 || endPos.x >= clientRect.right - (ballsize) / 2)
                dx = -dx;

            if (endPos.y <= clientRect.top + (ballsize) / 2)
                dy = -dy;

            if (endPos.y >= clientRect.bottom - (ballsize) / 2)
            {
                // ��� ���� ���� � �������� ������� ��� ���������� ��������
                SetRect(&ballRect, 390, 530, 410, 550);
                dy = -4;
                break;
            }

            // ��������� ������� ���� ����� ���� ��������
            startPos.x += dx;
            startPos.y += dy;

            ballRect.left = static_cast<int>(startPos.x) - (ballsize) / 2;
            ballRect.top = static_cast<int>(startPos.y) - (ballsize) / 2;

        }

        // ��������� ������� ���� � ���������
        OffsetRect(&ballRect, dx, dy);

        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }
        /* �������� ������������ � ��������� ����
            if (ballRect.left <= 0 || ballRect.right >= 800)
            {
                //OffsetRect(&ballRect, -dx / abs(dx) * 2, 0); // ��������� �� X
                dx = -dx;
                
            }
            
            if (ballRect.top <= 0)
            {
                //OffsetRect(&ballRect, 0, -dy / abs(dy)); // ��������� �� Y
                dy = -dy;
                
            }
            
        if (ballRect.bottom >= 650)
        {
            // ��� ���� ���� - ������� �������
            SetRect(&ballRect, 390, 530, 410, 550);
            dy = -4;
        }
        
        // ������������ � ��������
        /////
        const float M_PI = 3.14159265358979323846f;
        const float MAX_ANGLE = 75 * M_PI / 180; // ��������� � �������
        float ballX = (ballRect.right - ballRect.left) / 2;
        float ballY = (ballRect.bottom - ballRect.top) / 2;
        float ballrad = ballX;
        float paddleX = (paddleRect.right - paddleRect.left) / 2;
        float paddleY = (paddleRect.bottom - paddleRect.top) / 2;
        float paddleW = paddleX * 2;

         if (ballRect.bottom >= paddleRect.top)
            {

                // ������ ����� ����� �� �������
             float hitPos = ((ballRect.right - ballrad) - (paddleRect.right - paddleX)) / (paddleW);

                if (hitPos < 0) hitPos = 0;
                if (hitPos > 1) hitPos = 1;

                // ���� ����������
                float angle = (hitPos - 0.5f) * MAX_ANGLE;

                // ������� �������� ����
                float speed = sqrt(dx * dx + dy * dy);

                // ���������� �������� ����
                dx = speed * sin(angle);
                dy = -abs(speed * cos(angle));
            }
        /////
        if (ballRect.right >= paddleRect.left && ballRect.left <= paddleRect.right &&
            ballRect.bottom >= paddleRect.top && ballRect.top <= paddleRect.bottom)
        {
            dy = -abs(dy);
        }
        
        
        // �������� ������������ � �������
        for (auto& block : blocks) 
        {
            if (!block.destroyed && ballRect.right >= block.rect.left && ballRect.left <= block.rect.right &&
                ballRect.bottom >= block.rect.top && ballRect.top <= block.rect.bottom) //intersectrect �� ������������ ������������
            {
                block.destroyed = true;
                //OffsetRect(&ballRect, 0, -(ballRect.bottom - block.rect.top));
                dy = -dy;
                break;
            }
        }
        // ���������� ������� ����
        OffsetRect(&ballRect, dx, dy);

        if (isLeftPressed) {
            OffsetRect(&paddleRect, -5, 0);
            // �������� ������
            if (paddleRect.left < 0)
                SetRect(&paddleRect, 0, paddleRect.top, 100, paddleRect.bottom);
        }

        if (isRightPressed) {
            OffsetRect(&paddleRect, 5, 0);
            // �������� ������
            if (paddleRect.right > 800)
                SetRect(&paddleRect, 700, paddleRect.top, 800, paddleRect.bottom);
        }

        InvalidateRgn(hwnd, NULL, FALSE); // ������������ ����
        
        break;
        
    } */

    
    

    case WM_KEYDOWN:
    {
        if (wParam == VK_LEFT) isLeftPressed = true;
        if (wParam == VK_RIGHT) isRightPressed = true;
        break;
    }

    case WM_KEYUP:
        if (wParam == VK_LEFT) isLeftPressed = false;
        if (wParam == VK_RIGHT) isRightPressed = false;
        break;
    

    case WM_MOUSEMOVE:
    {
        POINT pt = { (short)LOWORD(lParam),(short)HIWORD(lParam) };
        if (pt.x > paddleRect.left && pt.x < paddleRect.right) {
            SetRect(&paddleRect, pt.x - 50, paddleRect.top, pt.x + 50, paddleRect.bottom);
            InvalidateRgn(hwnd, NULL, FALSE);
        }
        break;
    }
    ;

    case WM_ERASEBKGND:
        return (LRESULT)1; // Say we handled it.

    case WM_PAINT:
    {

        //HDC hdc = BeginPaint(hwnd, &ps);
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
//#define vv3
#ifdef vv3
#include <windows.h>
#include <vector>
#include <algorithm>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int BALL_SIZE = 20;
double dx = 5.0; // �������� �� X
double dy = -3.0; // �������� �� Y

RECT ballRect;
std::vector<RECT> blocks;

HWND hwndMain;
UINT_PTR timerId;

// ������� ��� ��������� ������� ���� �� ������
void SetBallPositionFromCenter(const POINT& center)
{
    ballRect.left = static_cast<int>(center.x) - BALL_SIZE / 2;
    ballRect.top = static_cast<int>(center.y) - BALL_SIZE / 2;
    ballRect.right = static_cast<int>(center.x) + BALL_SIZE / 2;
    ballRect.bottom = static_cast<int>(center.y) + BALL_SIZE / 2;
}

// �������� ����������� ����� �������� p1-p2 � ��������������� rect
bool LineIntersectsRect(const POINT& p1, const POINT& p2, const RECT& rect, double& t)
{
    auto LineSegmentsIntersect = [](double x1, double y1, double x2, double y2,
        double x3, double y3, double x4, double y4,
        double& t)
        {
            double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (fabs(denom) < 1e-8)
                return false; // �����������

            double t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
            t = t_num / denom;

            double u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
            double u = u_num / denom;

            if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
                return true;
            return false;
        };

    POINT sides[4][2] = {
        { {rect.left, rect.top}, {rect.right, rect.top} },
        { {rect.right, rect.top}, {rect.right, rect.bottom} },
        { {rect.right, rect.bottom}, {rect.left, rect.bottom} },
        { {rect.left, rect.bottom}, {rect.left, rect.top} }
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
            if (tTemp < minT)
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

// ��������� ������������ � ��������� ����
void CheckWallCollisions(POINT& pos)
{
    RECT clientRect;
    GetClientRect(hwndMain, &clientRect);

    if (pos.x <= clientRect.left + BALL_SIZE / 2)
    {
        pos.x = clientRect.left + BALL_SIZE / 2;
        dx = -dx;
    }
    if (pos.x >= clientRect.right - BALL_SIZE / 2)
    {
        pos.x = clientRect.right - BALL_SIZE / 2;
        dx = -dx;
    }
    if (pos.y <= clientRect.top + BALL_SIZE / 2)
    {
        pos.y = clientRect.top + BALL_SIZE / 2;
        dy = -dy;
    }
}

// ��������� ������������ � �������
void CheckBlockCollisions(const POINT& startPos, POINT& endPos)
{
    for (auto it = blocks.begin(); it != blocks.end(); )
    {
        RECT blockRect = *it;

        double t = 0.0;
        if (LineIntersectsRect(startPos, endPos, blockRect, t))
        {
            // ��������� ������� �� ����� ������������
            endPos.x = startPos.x + dx * t;
            endPos.y = startPos.y + dy * t;

            // ��������� �������� �� ����� ����
            dx = -dx;
            dy = -dy;

            // ������� ����
            it = blocks.erase(it);
            break; // ����� ���������� �������� ����� ������� ������������
        }
        else
            ++it;
    }
}

// ���������� ������� � �������� ����
void OnTimer()
{
    // ������� ������� ������ ����
    POINT startPos = { ballRect.left + BALL_SIZE / 2 , ballRect.top + BALL_SIZE / 2 };

    // �������������� ����� ��������� �� ���� ����
    POINT endPos = { startPos.x + dx , startPos.y + dy };

    // �������� ������������ � �������
    CheckBlockCollisions(startPos, endPos);

    // �������� ������������ �� �������
    CheckWallCollisions(endPos);

    // ��������� ������� ���� �� ������
    SetBallPositionFromCenter(endPos);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        hwndMain = hwnd;

        // ������������� ���� � ������ ����
        POINT center = { WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2 };
        SetBallPositionFromCenter(center);

        // ������� ����� � ��������, ��������� ��������������� � ������ ����� ����
        int blockWidth = 60, blockHeight = 20;
        for (int i = 0; i < 10; ++i)
        {
            RECT r = { 10 + i * 70 , WINDOW_HEIGHT - 50 ,10 + i * 70 + blockWidth , WINDOW_HEIGHT - 50 + blockHeight };
            blocks.push_back(r);
        }

        timerId = SetTimer(hwnd, NULL, 16, NULL); // �������� 60 fps
    }
    break;

    case WM_TIMER:
        OnTimer();
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // ������ ���
        HBRUSH brushBall = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdc, &ballRect, brushBall);
        DeleteObject(brushBall);

        // ������ �����
        HBRUSH brushBlock = CreateSolidBrush(RGB(0, 255, 0));
        for (const auto& r : blocks)
            FillRect(hdc, &r, brushBlock);
        DeleteObject(brushBlock);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        if (timerId != 0)
            KillTimer(hwnd, timerId);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = (LPCWSTR)"MyWindowClass";

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, L"��� � �����", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
#endif //vv3