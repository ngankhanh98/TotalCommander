// TotalCommander.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TotalCommander.h"
#include <windowsx.h>
#include <CommCtrl.h>
#include <wchar.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

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
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
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
		0, 0, 300, 540, hwnd,																	// TVS_HASLINES (TVS_LINESATROOT) : Uses lines to show the hierarchy of items (at the root of the tree-view control). Gợi ý bởi Nguyễn Hồng Khang
		(HMENU)IDC_TREEVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(hTreeView, hFont, TRUE);

	hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT,
		300, 0, 500, 540, hwnd,
		(HMENU)IDC_LISTVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(hListView, hFont, TRUE);

	// Thêm nút gốc This PC vào TreeView
	TV_INSERTSTRUCT tvInsert;	// Cấu trúc dữ liệu dùng để chèn một node vào TreeView
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN; // Chèn thêm text và dữ liệu bên dưới - param
	tvInsert.hParent = NULL;			// Không có cha, hiển nhiên!
	tvInsert.hInsertAfter = TVI_ROOT;	// Chèn ngay đầu
	tvInsert.item.pszText = (LPWSTR)L"This PC"; // Text hiển thị của node này
	tvInsert.item.lParam = (LPARAM)L"Root";	 // Dữ liệu tại node để xử lí
	tvInsert.item.cChildren = 1; // Nếu có nút con sẽ tự động được vẽ nút để bấm vào sổ xuống
	hThisPC = TreeView_InsertItem(hTreeView, &tvInsert);

	// Lấy chuỗi chứa các ổ đĩa
	const int BUFFER_SIZE = 26 * 4; // 26 kí tự từ A - Z, 
	// Mỗi ổ đĩa cần chuỗi kích thước 4 ở dạng C:
	WCHAR drives[BUFFER_SIZE];
	GetLogicalDriveStrings(BUFFER_SIZE, drives);

	WCHAR* name = drives; // Con trỏ trỏ đến vị trí đầu chuỗi
	do
	{
		tvInsert.hParent = hThisPC;
		tvInsert.hInsertAfter = TVI_LAST; // Cứ chèn từ trên xuống dưới
		tvInsert.item.pszText = name;
		int len = wcslen(name);
		WCHAR* path = new WCHAR[len + 1]; // QUAN TRỌNG: RÒ RỈ BỘ NHỚ Ở ĐÂY
		wcscpy_s(path, len + 1, name);
		tvInsert.item.lParam = (LPARAM)path;
		TreeView_InsertItem(hTreeView, &tvInsert);
		int i = 0;
		while (name[i] != '\0') i++; // Trong khi chưa gặp kí tự \0 thì tăng 1
		// Dừng lại thì i đang ở kí tự \0
		name = &name[i + 1]; // Bỏ qua kí tự \0 hiện tại và đi tới kí tự kế
	} while (name[0] != '\0'); // Chưa gặp 2 \0 liên tiếp



	TreeView_Expand(hTreeView, hThisPC, TVE_EXPAND);
	TreeView_SelectItem(hTreeView, hThisPC);
	loaded = true;



	// Thông tin cột cần chèn
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;

	// Cột thứ nhất
	col.cx = 130;
	col.pszText = (LPWSTR)L"Tên";
	ListView_InsertColumn(hListView, 0, &col);

	// Cột thứ hai
	col.cx = 100;
	col.pszText = (LPWSTR)L"Loại";
	ListView_InsertColumn(hListView, 1, &col);

	// Cột thứ ba
	col.cx = 100;
	col.pszText = (LPWSTR)L"Kích thước";
	ListView_InsertColumn(hListView, 2, &col);

	// Cột thứ tư
	col.cx = 200;
	col.pszText = (LPWSTR)L"Lần truy cập cuối";
	ListView_InsertColumn(hListView, 3, &col);

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
		HTREEITEM node;

		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			node = lpnmTree->itemNew.hItem;
			if (node != hThisPC) { 
				// Không phải node gốc

				// Làm rỗng subitem của node
				HTREEITEM tvDelete;
				tvDelete = TreeView_GetChild(hTreeView, node);
				while (tvDelete != NULL)
				{
					TreeView_DeleteItem(hTreeView, tvDelete);
					tvDelete = TreeView_GetChild(hTreeView, node);
				}

				// Lấy đường dẫn ẩn ở bên dưới
				TVITEMEX tv;
				tv.mask = TVIF_PARAM;
				tv.hItem = node;
				TreeView_GetItem(hTreeView, &tv);
								
				// Tạo đường dẫn để tìm
				const int BUFFER_SIZE = 260;
				WCHAR buffer[BUFFER_SIZE];
				wcscpy_s(buffer, BUFFER_SIZE, (LPWSTR)tv.lParam);
				wcscat_s(buffer, L"\\*");
				// Duyệt qua đường dẫn lấy các thư mục con và thêm vào
				WIN32_FIND_DATA data;
				HANDLE hFile = FindFirstFile(buffer, &data); // Tìm tập tin đầu tiên
				do {
					if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
						!(data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
						!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
						(wcscmp(data.cFileName, L".") != 0) &&
						(wcscmp(data.cFileName, L"..") != 0))
					{
						TV_INSERTSTRUCT tvInsert;
						tvInsert.hParent = node;
						tvInsert.hInsertAfter = TVI_LAST;
						tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
						tvInsert.item.cChildren = 1;
						tvInsert.item.pszText = data.cFileName;
						// Tạo đường dẫn đầy đủ
						int len = wcslen((LPWSTR)tv.lParam)
							+ wcslen(L"\\") + wcslen(data.cFileName) + 1;
						WCHAR * path = new WCHAR[len];
						wcscpy_s(path, len, (LPWSTR)tv.lParam);
						wcscat_s(path, len, L"\\");
						wcscat_s(path, len, data.cFileName);
						tvInsert.item.lParam = (LPARAM)path; // QUAN TRỌNG: RÒ RỈ BỘ NHỚ!!
						TreeView_InsertItem(hTreeView, &tvInsert);
					}
				} while (FindNextFile(hFile, &data)); // Cho đến khi không còn tập tin kế
				FindClose(hFile);
			}
			break;

		case TVN_SELCHANGED:
			// Lấy node đang được select
			node = TreeView_GetNextItem(hTreeView, NULL, TVGN_CARET);
			if (node != hThisPC) {
				//  Lấy đường dẫn từ lParam của node
				TVITEMEX tv;
				tv.mask = TVIF_PARAM;
				tv.hItem = node;
				TreeView_GetItem(hTreeView, &tv);

				// Xóa trống ListView lại từ đầu
				ListView_DeleteAllItems(hListView);

				// Tạo đường dẫn để tìm
				const int BUFFER_SIZE = 260;
				WCHAR buffer[BUFFER_SIZE];
				wcscpy_s(buffer, BUFFER_SIZE, (LPWSTR)tv.lParam);
				wcscat_s(buffer, L"\\*");

				// Duyệt qua đường dẫn lấy các thư mục con và thêm vào
				WIN32_FIND_DATA data;
				HANDLE hFile = FindFirstFile(buffer, &data); // Tìm tập tin đầu tiên
				int itemIndex = 0; // Dòng đầu tiên

				do {
					if (/*(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&*/
						!(data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
						!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
						(wcscmp(data.cFileName, L".") != 0) &&
						(wcscmp(data.cFileName, L"..") != 0))
					{
						LV_ITEM row;
						row.mask = LVIF_TEXT | LVIF_PARAM;
						row.iItem = itemIndex;
						row.iSubItem = 0; // Cột đầu tiên - Tên
						row.pszText = data.cFileName;
						row.lParam = (LPARAM)data.cFileName;
						// Kết hợp đường dẫn với tên thư mục là sẽ ra path ngay
						ListView_InsertItem(hListView, &row);

						// cột ngày tháng năm
						SYSTEMTIME stUTC, stLocal;
						FileTimeToSystemTime(&data.ftLastAccessTime, &stUTC);
						SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
						WCHAR buffer[BUFFER_SIZE];
						swprintf(buffer, 255, L"%02d/%02d/%d", stLocal.wDay, stLocal.wMonth, stLocal.wYear);
						ListView_SetItemText(hListView, itemIndex, 3, buffer);

						if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						{// cột dung lượng
							LARGE_INTEGER size;
							size.HighPart = data.nFileSizeHigh;
							size.LowPart = data.nFileSizeLow;
							WCHAR unit[] = L"KB";
							int level = log2(size.QuadPart) / 10;
							level = level * 10;
							if (level >= 40)
								unit[0] = 'T';
							else if (level >= 30)
								unit[0] = 'G';
							else if (level >= 20)
								unit[0] = 'M';

							swprintf(buffer, 255, L"%0.1f %s", size.QuadPart*1.0 / pow(2, level), unit);
							ListView_SetItemText(hListView, itemIndex, 2, buffer);
						}

						itemIndex++; // Qua dòng kế
					}
				} while (FindNextFile(hFile, &data)); // Cho đến khi không còn tập tin kế

				FindClose(hFile);
			}
			break;


		}


	}
	return 0;
}
