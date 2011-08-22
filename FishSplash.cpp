#include "FishSplash.h"

#ifdef WIN32
#include <windows.h>
#endif


#ifdef WIN32

LRESULT CALLBACK SplashWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	HDC    hDC;
    int height, width;
	static int a = 0;

    // dispatch messages
    switch (uMsg)
    {   
    case WM_CREATE:         // window creation
	//	if(fishSplash && hWnd != fishSplash->getSplashHwnd()) {



		
	//	}
        break;

    case WM_DESTROY:            // window destroy
		PostQuitMessage(0);
		break;
    case WM_QUIT:
		break;

    case WM_CLOSE:                  // windows is closing

        // deselect rendering context and delete it
		DestroyWindow(hWnd);

        // send WM_QUIT to message queue
	//	if(fishSplash && hWnd != fishSplash->getSplashHwnd())
			//PostQuitMessage(0);
		
        break;

    case WM_SIZE:
        height = HIWORD(lParam);        // retrieve width and height
        width = LOWORD(lParam);



        break;

    case WM_ACTIVATEAPP:        // activate app
        break;

    case WM_PAINT:              // paint
//		if(a)
//			display();
//		else
//			a++;
//		SwapBuffers(hDC);
//		ValidateRect(hWnd, 0);
		
		    hDC = GetDC(hWnd);
		for(int i = 0; i < 100; i++)
			for(int a = 0; a < 100; a++)
				SetPixel(hDC, i, a, RGB(255, 0, 0));
		
/*        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
  */    break;
	

	default:
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);

}


FishSplash::FishSplash(const char* className, HINSTANCE hInstance) {

	_className = className;
	_hInstance = hInstance;
	_windowHeight = 100;
	_windowWidth = 100;
	
#ifdef WIN32
	_mainThread = CreateThread(0, 0, RunThread, this, 0, 0);
#endif
	
	//COLORREF color;
	
	

	
}

void FishSplash::Start() {

	WNDCLASSEX windowClass;     // window class

	windowClass.cbSize          = sizeof(WNDCLASSEX);
    windowClass.style           = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc     = SplashWindowProc;
    windowClass.cbClsExtra      = 0;
    windowClass.cbWndExtra      = 0;
    windowClass.hInstance       = _hInstance;
    windowClass.hIcon           = 0;//LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON1));  // default icon
    windowClass.hCursor         = LoadCursor(_hInstance, IDC_ARROW);//LoadCursor(hInstance, MAKEINTRESOURCE(ID_CURSOR1));      // default arrow
    windowClass.hbrBackground   = NULL;                             // don't need background
    windowClass.lpszMenuName    = NULL;                             // no menu
    windowClass.lpszClassName   = "Splash";
    windowClass.hIconSm         = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon

	RegisterClassEx(&windowClass);

	_className = "Splash";
	HWND desktopWindow = GetDesktopWindow();
	RECT desktopRect;
	GetWindowRect(desktopWindow, &desktopRect);
	RECT windowRect;
	windowRect.left = (desktopRect.right/2)-_windowWidth;
	windowRect.top = (desktopRect.bottom/2)-_windowHeight;

	_splashHwnd = CreateWindowEx(NULL,                                 // extended style
        _className,                          // class name
        "Fish Splash",      // app name
        WS_POPUP | WS_CLIPCHILDREN |
        WS_CLIPSIBLINGS,
		windowRect.left, windowRect.top,                               // x,y coordinate
        _windowWidth,
        _windowHeight, // width, height
        NULL,                               // handle to parent
        NULL,                               // handle to menu
        _hInstance,                          // application instance
        NULL);                              // no extra params


	MSG        msg;             // message
	_exiting = FALSE;
	ShowWindow(_splashHwnd, SW_SHOW);
	UpdateWindow(_splashHwnd);
	while(!_exiting) {
	while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage (&msg, NULL, 0, 0))
            {
                _exiting = TRUE;
                break;
            }
			
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
	Sleep(100);
	}
	

}


#endif


void FishSplash::Display() {
#ifdef WIN32



//	SetWindowLong(_splashHwnd, GWL_STYLE,	WS_POPUP);



#else
//todo linux splash code


#endif



}


void FishSplash::Close() {

#ifdef WIN32
//	DestroyWindow(_splashHwnd);
//	CloseWindow(_splashHwnd);
//	CloseHandle(_splashHwnd);
	PostMessage(_splashHwnd, WM_CLOSE, 0, 0);
#endif

}


