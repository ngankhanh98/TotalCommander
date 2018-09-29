// TotalCommander.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TotalCommander.h"
#include <windowsx.h>
#include <CommCtrl.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL				OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void				OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void				OnDestroy(HWND hwnd);
LRESULT				OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);

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
	LoadStringW(hInstance, IDC_TOTALCOMMANDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOTALCOMMANDER));

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

	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOTALCOMMANDER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TOTALCOMMANDER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
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

HWND hTreeView;
HWND hListView;
HTREEITEM hThisPC;
BOOL loaded = false;
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	hTreeView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,	// TVS_HASBUTTONS: Displays plus (+) and minus (-) buttons next to parent items. Gợi ý bởi Nguyễn Hồng Khang
		0, 0, 300, 540, hwnd,																		// TVS_HASLINES (TVS_LINESATROOT) : Uses lines to show the hierarchy of items (at the root of the tree-view control). Gợi ý bởi Nguyễn Hồng Khang
		(HMENU)IDC_TREEVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(hTreeView, hFont, TRUE);
	hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
		WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL,
		300, 0, 500, 540, hwnd,
		(HMENU)IDC_LISTVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(hListView, hFont, TRUE);

	// Thêm nút gốc This PC vào TreeView
	TV_INSERTSTRUCT tvInsert;										// Cấu trúc dữ liệu dùng để chèn một node vào TreeView
	tvInsert.item.cChildren = 1;									// Nếu có nút con sẽ tự động được vẽ nút để bấm vào sổ xuống
	tvInsert.item.mask = TVIF_CHILDREN | TVIF_TEXT;// | TVIF_PARAM;	// Chèn thêm text và dữ liệu bên dưới - param
	tvInsert.hParent = NULL;										// Không có cha, hiển nhiên!
	tvInsert.hInsertAfter = TVI_ROOT;								// Chèn ngay đầu
	tvInsert.item.pszText = (LPWSTR) L"This PC";					// Text hiển thị của node này
	tvInsert.item.lParam = (LPARAM) L"Root";						// Dữ liệu tại node để xử lí
	hThisPC = TreeView_InsertItem(hTreeView, &tvInsert);

	// Lấy chuỗi chứa các ổ đĩa
	const int BUFFER_SIZE = 26 * 4; // 26 kí tự từ A - Z, 
	 /*Mỗi ổ đĩa cần chuỗi kích thước 4 ở dạng C:\*/
	WCHAR drives[BUFFER_SIZE];
	GetLogicalDriveStrings(BUFFER_SIZE, drives);

	WCHAR* name = drives; // Con trỏ trỏ đến vị trí đầu chuỗi
	do
	{
		tvInsert.hParent = hThisPC;
		tvInsert.hInsertAfter = TVI_LAST; // Cứ chèn từ trên xuống dưới
		tvInsert.item.pszText = name;
		tvInsert.item.lParam = (LPARAM)name;
		TreeView_InsertItem(hTreeView, &tvInsert);
		int i = 0;
		while (name[i] != '\0') i++; // Trong khi chưa gặp kí tự \0 thì tăng 1
		// Dừng lại thì i đang ở kí tự \0
		name = &name[i + 1]; // Bỏ qua kí tự \0 hiện tại và đi tới kí tự kế
	} while (name[0] != '\0'); // Chưa gặp 2 \0 liên tiếp

	//Mặc định cho My Computer expand và select luôn
	TreeView_Expand(hTreeView, hThisPC, TVE_EXPAND);
	TreeView_SelectItem(hTreeView, hThisPC);
	loaded = true;
	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR * pnm)
{
	if (loaded) { // Đã nạp xong dữ liệu ban đầu rồi
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;

		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			MessageBox(0, 0, 0, 0);
			break;
		}
	}
	return LRESULT();
}
