// WindowsProject1.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "WindowsProject1.h"
#include <string>
#include "headers.h"
#include <thread>
#include <unordered_set>
#define SECOND_TIMER 100
#define TEST_MODE 0

std::string tf;

#define MAX_LOADSTRING 100
#define IDB_BALL 101
#define FPWH 98
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HBITMAP g_hbmBall = NULL;
HWND hWndExample = NULL;

HWND hWnd = NULL;

PAINTSTRUCT ps;
std::thread* MT = nullptr;
std::thread* MT2 = nullptr;
std::thread* MT2_ = nullptr;
HDC hdcMem1;
HBITMAP hbitmap2;
#include <map>

std::map<COLORREF, int> hist;

void BmpSetImageData(SBmpImage* bmp, const std::vector<uint8>& data, uint32 width, uint32 height)
{
    bmp->data.resize(data.size());

    for (uint32 y = 0; y < height; y++)
    {
        const uint32 yy = (height - y - 1) * width;

        for (uint32 x = 0; x < width; x++)
        {
            const uint32 xy = yy + x;

            bmp->data[xy].r = data[y * width + x];
            bmp->data[xy].g = data[y * width + x];
            bmp->data[xy].b = data[y * width + x];
            bmp->data[xy].a = 255;
        }
    }

    bmp->dataPaddingSize = sizeof(uint32) - ((width * NBmp::BMP_24B_COLORS) % sizeof(uint32));
    if (bmp->dataPaddingSize == sizeof(uint32))
        bmp->dataPaddingSize = 0;

    bmp->signature = NBmp::BMP_HEADER_SIGNATURE;
    bmp->fileSize = NBmp::BMP_HEADER_FILE_DATA_OFFSET + bmp->colorTable.size() * NBmp::BMP_32B_COLORS + (width * NBmp::BMP_24B_COLORS + bmp->dataPaddingSize) * height;
    bmp->reserved = NBmp::BMP_HEADER_RESERVED;
    bmp->dataOffset = NBmp::BMP_HEADER_FILE_DATA_OFFSET + bmp->colorTable.size() * NBmp::BMP_32B_COLORS;
    bmp->infoHeader.headerSize = NBmp::BMP_INFO_HEADER_SIZE;
    bmp->infoHeader.width = width;
    bmp->infoHeader.height = height;
    bmp->infoHeader.planes = NBmp::BMP_INFO_HEADER_PLANES;
    bmp->infoHeader.bitsPerPixel = NBmp::BMP_INFO_HEADER_BITS_PER_PIXEL_24;
    bmp->infoHeader.compression = NBmp::BMP_INFO_HEADER_COMPRESSION;
    bmp->infoHeader.imageSize = (width * NBmp::BMP_24B_COLORS + bmp->dataPaddingSize) * height;
    bmp->infoHeader.pixelPerMeterX = NBmp::BMP_INFO_HEADER_PIXEL_PER_METER_X;
    bmp->infoHeader.pixelPerMeterY = NBmp::BMP_INFO_HEADER_PIXEL_PER_METER_Y;
    bmp->infoHeader.colors = NBmp::BMP_INFO_HEADER_COLORS;
    bmp->infoHeader.usedColors = NBmp::BMP_INFO_HEADER_USED_COLORS;
}

void BmpSave2(const SBmpImage* bmp, std::string filename)
{
    const uint32 dataPadding = 0;

    CFile file(filename);

    file.write(&bmp->signature[0], sizeof(char) * NBmp::BMP_HEADER_SIGNATURE_LENGHT);
    file.write(&bmp->fileSize, sizeof(uint32));
    file.write(&bmp->reserved, sizeof(uint32));
    file.write(&bmp->dataOffset, sizeof(uint32));
    file.write(&bmp->infoHeader.headerSize, sizeof(uint32));
    file.write(&bmp->infoHeader.width, sizeof(uint32));
    file.write(&bmp->infoHeader.height, sizeof(uint32));
    file.write(&bmp->infoHeader.planes, sizeof(uint16));
    file.write(&bmp->infoHeader.bitsPerPixel, sizeof(uint16));
    file.write(&bmp->infoHeader.compression, sizeof(uint32));
    file.write(&bmp->infoHeader.imageSize, sizeof(uint32));
    file.write(&bmp->infoHeader.pixelPerMeterX, sizeof(uint32));
    file.write(&bmp->infoHeader.pixelPerMeterY, sizeof(uint32));
    file.write(&bmp->infoHeader.colors, sizeof(uint32));
    file.write(&bmp->infoHeader.usedColors, sizeof(uint32));

    for (uint32 i = 0; i < bmp->colorTable.size(); i++)
    {
        file.write(&bmp->colorTable[i].r, sizeof(uint8));
        file.write(&bmp->colorTable[i].g, sizeof(uint8));
        file.write(&bmp->colorTable[i].b, sizeof(uint8));
        file.write(&bmp->colorTable[i].a, sizeof(uint8));
    }
    float SCALE = 43;


    for (uint32 y = 0; y < bmp->infoHeader.height; y++)
    {
        const uint32 yy = (bmp->infoHeader.height - y - 1) * bmp->infoHeader.width;

        for (uint32 x = 0; x < bmp->infoHeader.width; x++)
        {
            const uint32 xy = yy + x;
            int f = 0;

            file.write(&bmp->data[xy].b, sizeof(uint8));
            file.write(&bmp->data[xy].g, sizeof(uint8));
            file.write(&bmp->data[xy].r, sizeof(uint8));

        }
        file.write(&dataPadding, sizeof(uint8) * bmp->dataPaddingSize);
    }
    file.close();
}

void BmpSave(HDC hdc, SBmpImage* bmp)
{
    tf = "";
    int V = 0;
    uint8 r = 255, g = 255, b = 255;
    uint8 xl = 0;
    int prev = 0;

    std::vector<double> A;
    std::vector<double> B;
    for (uint32 x = 0; x < bmp->infoHeader.height; x++)
    {
        const uint32 xx = (bmp->infoHeader.height - x - 1) * bmp->infoHeader.width;

        for (uint32 y = 0; y < bmp->infoHeader.width; y++)
        {
            const uint32 xy = xx + y;


            if ((bmp->data[xy].r) <= 16) {
                xl += 1;
            }

            A.push_back(xl);
        }
        xl = 0;
    }
    V = 0;
    for (uint32 x = 0; x < bmp->infoHeader.width; x++)
    {
        for (uint32 y = 0; y < bmp->infoHeader.height; y++)
        {
            const uint32 yy = (bmp->infoHeader.height - y - 1) * bmp->infoHeader.width;


            const uint32 xy = yy + x;

            if (bmp->data[xy].r <= 16) {
                V += 1;
            }

            B.push_back(V);
        }
        V = 0;
    }

    float MINVx = 10000;
    float MAXVx = -10000;

    float MINVy = 10000;
    float MAXVy = -10000;

    for (int i = 0; i < A.size(); i++) {
        if (A[i] > MAXVx)
            MAXVx = A[i];

        if (B[i] > MAXVy)
            MAXVy = B[i];

        if (A[i] < MINVx)
            MINVx = A[i];

        if (B[i] < MINVy)
            MINVy = B[i];
    }

    float SCALE = 32;

   // std::cout << "[";
    std::string v = "";

    for (uint32 y = 0; y < bmp->infoHeader.height; y++)
    {
        const uint32 yy = (bmp->infoHeader.height - y - 1) * bmp->infoHeader.width;
        uint8_t xva, yva = 0;

        for (uint32 x = 0; x < bmp->infoHeader.width; x++)
        {
            const uint32 xy = yy + x;
            const float XV = ((float)A[xy]) *SCALE + 40;
            const float YV = ((float)B[xy]) *SCALE + 40;
            const float CV = (MINVx + MAXVx + MINVx + MAXVy + x + y) / 6;
            
            bmp->data[xy].a = bmp->data[xy].r;
            xva = (XV - 40)/SCALE; yva = (YV - 40)/SCALE;

            if (XV > YV) {
                if (XV > CV) {
                    SetPixelV(hdc, x, y, RGB(XV, YV, CV));
                    if (hist.find(RGB(XV, YV, CV)) == hist.end())
                        hist.insert(std::pair<COLORREF, int>(RGB(XV, YV, CV), 1));
                    else
                        hist[RGB(XV, YV, CV)]++;

                    bmp->data[xy].r = XV;
                    bmp->data[xy].g = YV;
                    bmp->data[xy].b -= CV;
                }
                else {
                    SetPixelV(hdc, x, y, RGB(CV, YV, XV));
                    if (hist.find(RGB(CV, YV, XV)) == hist.end())
                        hist.insert(std::pair<COLORREF, int>(RGB(CV, YV, XV), 1));
                    else
                        hist[RGB(CV, YV, XV)]++;


                    bmp->data[xy].r = CV;
                    bmp->data[xy].g = YV;
                    bmp->data[xy].b -= XV;
                }
            }
            else {
                if (YV > CV) {
                    SetPixelV(hdc, x, y, RGB(YV, XV, CV));
                    if (hist.find(RGB(YV, XV, CV)) == hist.end())
                        hist.insert(std::pair<COLORREF, int>(RGB(YV, XV, CV), 1));
                    else
                        hist[RGB(YV, XV, CV)]++;

                    bmp->data[xy].r = YV;
                    bmp->data[xy].g = XV;
                    bmp->data[xy].b -= CV;
                }
                else {
                    SetPixelV(hdc, x, y, RGB(CV, XV, YV));
                    if (hist.find(RGB(CV, XV, YV)) == hist.end())
                        hist.insert(std::pair<COLORREF, int>(RGB(CV, XV, YV), 1));
                    else
                        hist[RGB(CV, XV, YV)]++;

                    bmp->data[xy].r = CV;
                    bmp->data[xy].g = XV;
                    bmp->data[xy].b -= YV;
                }
            }




            //    if (bmp->data[xy].b <= 16 && x % 8 == 0) {
                //    std::cout << (char)((xy % 33) + 33);
               //     v += (char)((xy % 33) + 33);
             //   }
        }

       // std::cout << xva << "," << yva << ",";
        tf = std::to_string(xva + yva);
    }

    for (auto i = hist.begin(); i != hist.end(); i++) {
     //   std::cout << i->first << " - " << i->second << std::endl;
      std::cout << (char)((i->second % 33) + 43);
      v += (char)((i->second % 33) + 43);
    }

    if (OpenClipboard(hWnd)) {
        EmptyClipboard();

        HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
            (v.length() + 1) * sizeof(TCHAR));
        if (hglbCopy != NULL)
        {
            // Lock the handle and copy the text to the buffer. 

            LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy);
            memcpy(lptstrCopy, v.c_str(),
               (v.length()+1)*sizeof(char));
            lptstrCopy[v.length()] = (TCHAR)0;    // null character 
            GlobalUnlock(hglbCopy);

            // Place the handle on the clipboard. 

            SetClipboardData(CF_TEXT, hglbCopy);
        }

        CloseClipboard();
    }

    std::cout << "\n";//"]\n";
}


SBmpImage bmp;
bool ct = 0;
HRESULT CaptureSample(HDC* hdc)
{
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_REJECT_DETAIL rejectDetail = 0;
    PWINBIO_BIR sample = NULL;
    SIZE_T sampleSize = 0;

    // Connect to the system pool. 
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,    // Service provider
        WINBIO_POOL_SYSTEM,         // Pool type
        WINBIO_FLAG_RAW,            // Access: Capture raw data
        NULL,                       // Array of biometric unit IDs
        0,                          // Count of biometric unit IDs
        WINBIO_DB_DEFAULT,          // Default database
        &sessionHandle              // [out] Session handle
    );

    if (FAILED(hr))
    {
        std::cout << "WinBioOpenSession failed. hr = 0x" << std::hex << hr << std::dec << "\n";

        if (sample != NULL)
        {
            WinBioFree(sample);
            sample = NULL;
        }

        if (sessionHandle != NULL)
        {
            WinBioCloseSession(sessionHandle);
            sessionHandle = NULL;
        }

        return hr;
    }

    // Capture a biometric sample.
    std::cout << "Calling WinBioCaptureSample - Swipe sensor...\n";

    hr = WinBioCaptureSample(
        sessionHandle,
        WINBIO_NO_PURPOSE_AVAILABLE,
        WINBIO_DATA_FLAG_RAW,
        &unitId,
        &sample,
        &sampleSize,
        &rejectDetail
    );

    if (FAILED(hr))
    {
        if (hr == WINBIO_E_BAD_CAPTURE)
            std::cout << "Bad capture; reason: " << rejectDetail << "\n";
        else
            std::cout << "WinBioCaptureSample failed.hr = 0x" << std::hex << hr << std::dec << "\n";

        if (sample != NULL)
        {
            WinBioFree(sample);
            sample = NULL;
        }

        if (sessionHandle != NULL)
        {
            WinBioCloseSession(sessionHandle);
            sessionHandle = NULL;
        }

        return hr;
    }

    std::cout << "Swipe processed - Unit ID: " << unitId << "\n";
    std::cout << "Captured " << sampleSize << " bytes.\n";

    if (sample != NULL)
    {
        PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset);
        PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset);
        PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER));

        DWORD width = AnsiBdbRecord->HorizontalLineLength; // Width of image in pixels
        DWORD height = AnsiBdbRecord->VerticalLineLength; // Height of image in pixels

        std::cout << "Image resolution: " << width << " x " << height << "\n";

        PBYTE firstPixel = (PBYTE)((PBYTE)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD);

        std::vector<uint8> data(width * height);
        memcpy(&data[0], firstPixel, width * height);

        SYSTEMTIME st;
        GetSystemTime(&st);
        std::stringstream s;
        s << st.wYear << "." << st.wMonth << "." << st.wDay << "." << st.wHour << "." << st.wMinute << "." << st.wSecond << "." << st.wMilliseconds;

        if (ct == 0)
            BmpSetImageData(&bmp, data, width, height);

        if (TEST_MODE) {
            ct = 1;
        }

        if (hdc != nullptr)
            BmpSave(*hdc, &bmp);

        std::string bmpFile = "data/" + tf  + ".bmp"; //+ s.str()

        BmpSave2(&bmp, bmpFile + ".alt.bmp");

        WinBioFree(sample);
        sample = NULL;
    }

    if (sessionHandle != NULL)
    {
        WinBioCloseSession(sessionHandle);
        sessionHandle = NULL;
    }

    return hr;
}

void drawBP(HDC hdcMem) {//HWND hWnd, PAINTSTRUCT ps) {

    int x = 1;
    while (1) {
        SelectObject(hdcMem, hbitmap2);

        CaptureSample(&hdcMem);

        std::string test = "FeatV" + std::to_string(x++);
        std::wstring stemp = std::wstring(test.begin(), test.end());
        LPCWSTR sw = stemp.c_str();
     //   SetWindowText(hWnd, sw);

        //    BitBlt(hdc, 0, 0, FPWH, FPWH, hdcMem, 0, 0, SRCCOPY);
    }
}

void drawBKA() {
    CaptureSample(&hdcMem1);
}

void drawBK() {
    int x = 0;
    while (true) {

        if (MT2 != nullptr)
            MT2_ = new std::thread(drawBKA);
    }

    
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

int main() {
  //  ShowWindow(::GetConsoleWindow(), SW_HIDE);
    CreateDirectoryA("data", NULL);
    while (1) {
        if (MT == nullptr)
            MT = new std::thread(drawBP, nullptr);
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, L"Feature Visualizer and Generator", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, FPWH*5, FPWH*5, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   CreateDirectoryA("data", NULL);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: 
    {
        SetTimer(hWnd, SECOND_TIMER, SECOND_TIMER, NULL);
        //other initialisation stuff
        break;
    }
    case WM_TIMER:
        if (wParam == SECOND_TIMER)
        {
            InvalidateRect(hWnd, NULL, FALSE);   // invalidate whole window
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {

        BITMAP bm;
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hWnd, &ps);

        if (MT == nullptr) {
            hdcMem1 = CreateCompatibleDC(hdc);
            hbitmap2 = CreateCompatibleBitmap(hdc, FPWH, FPWH);
            MT = new std::thread(drawBP, hdcMem1);
        }

        StretchBlt(hdc, 0, 0, FPWH * 5, FPWH * 5, hdcMem1, 0, 0, FPWH, FPWH, SRCCOPY);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
