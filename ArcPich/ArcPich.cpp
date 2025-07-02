// ArcPich
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
//HDC buffer2;
static std::vector<Block> blocks;
static RECT paddleRect;
static RECT ballRect;
static bool isLeftPressed = false, isRightPressed = false;

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

static void Paint(HWND hwnd, LPPAINTSTRUCT lpPS)//��������������� ���������
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
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);//��� ��� ��������� � ��� ������ ���������� �������, ��� ������ hdc �������� �� hbmMem hdcMem
    
    /*
    RECT rect;
    GetClientRect(hwnd, &rect);
    auto whiteBrush = CreateSolidBrush(RGB(0, 0, 255));
    FillRect(buffer2, &rect, whiteBrush);
    DeleteObject(whiteBrush);
    */

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
int dx = 2, dy = -4;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    

    switch (msg)
    {
    case WM_CREATE:
    {
        RECT r;
        GetClientRect(hwnd, &r);
        auto window_width = r.right - r.left;//���������� ������� � ���������
        auto window_height = r.bottom - r.top;
        InitGameObjects(blocks, paddleRect, ballRect);
        SetTimer(hwnd, 1, 16, NULL); // ������ ��� ���������� ���� (~60 fps)

        break;
    }

    case WM_TIMER:
    {
        // ���������� ��������� ���� � �������� ������������

        // �������� ������������ � ��������� ����
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
        if (ballRect.right >= paddleRect.left && ballRect.left <= paddleRect.right &&
            ballRect.bottom >= paddleRect.top && ballRect.top <= paddleRect.bottom)
        {
            // ������������
            dy = -abs(dy);
        }
        //if (IntersectRect(0, &ballRect, &paddleRect)) 
        //{
            //OffsetRect(&ballRect, 0, -(ballRect.bottom - paddleRect.top));
         //   dy = -abs(dy);
        //}

        // �������� ������������ � �������
        for (auto& block : blocks) 
        {
            if (!block.destroyed && IntersectRect(NULL, &ballRect, &block.rect)) //intersectrect �� ����������� ������������ ���� ����� ���� ��������� ������������
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
    }

    
    

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