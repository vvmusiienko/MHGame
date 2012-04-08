//============================================================================
// Name        : main.cpp
// Author      : Musienko Volodimir aka MuHAOS
// Copyright   : Musienko Volodimir 2009
// Description : Platform based start point
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "Game.h"
#include "WindowsOSAdapter.h"
#include "BuildSettings.h"
#include "GL/gl.h"

#define		SCREEN_WIDTH	480
#define		SCREEN_HEIGHT	348


HGLRC hGLRC; // OpenGL rendering context 
HDC hDC; // Windows device context
HWND hWnd; 

__int64 perfFreq, perfLast, perfCurrent, perfDiff;

Game* game;
bool perfFreqDetermined = false;

bool setupPixelFormat(HWND hWnd)
{
	GLuint PixelFormat; 
	static PIXELFORMATDESCRIPTOR pfd= 
	{ 
		sizeof(PIXELFORMATDESCRIPTOR), 
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		1, 1, 1, 1,
		32,
		0,
		0,
		PFD_MAIN_PLANE, 
		0,
		0, 0, 0
	}; 
	hDC = GetDC(hWnd);
	
	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (!PixelFormat)
	{ 
		return false;
	} 
	
	SetPixelFormat (hDC, PixelFormat, &pfd);
	if (!SetPixelFormat (hDC, PixelFormat, &pfd))
	{ 
		return false; 
	} 
	
	hGLRC = wglCreateContext (hDC);
	if (!hGLRC)
	{ 
		return false; 
	} 
	
	wglMakeCurrent(hDC, hGLRC);
	if (!wglMakeCurrent(hDC, hGLRC))
	{ 
		return false; 
	}
	
	// switch of frame limiter
	
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

	const char *extensions = (const char *)glGetString( GL_EXTENSIONS );
	
	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
		return false; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	else
	{
	  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	
	  if( wglSwapIntervalEXT )
	    wglSwapIntervalEXT(0);
	}
	
	return true;
}



LRESULT CALLBACK WindowFunc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{ 

	switch(message)
	{ 
		case WM_CREATE:
			break; 
	
		case WM_DESTROY:
		case WM_CLOSE:
			game->stopGame();
			ChangeDisplaySettings (NULL, 0);
			if(hDC) wglMakeCurrent(hDC, NULL);
			if(hGLRC) wglDeleteContext(hGLRC);
			if(hWnd) ReleaseDC(hWnd, hDC);
			break; 
		
		case WM_SIZE: 
			break; 

		case WM_TIMER: { // computing actual freq
			/*static unsigned long int previosValue = 0;
			static unsigned long int currentValue;
			if (previosValue != 0) {
				QueryPerformanceCounter((LARGE_INTEGER*)&currentValue);
				perfFreq = currentValue - previosValue;
				perfFreqDetermined = true;
			}
			QueryPerformanceCounter((LARGE_INTEGER*)&previosValue);*/
			break; }
		
        case WM_MBUTTONDOWN:
            game->cursorDown(MIDDLE);
            break;
        case WM_MBUTTONUP:
            game->cursorUp(MIDDLE);
            break;

        case WM_RBUTTONDOWN:
            game->cursorDown(RIGHT);
            break;
        case WM_RBUTTONUP:
            game->cursorUp(RIGHT);
            break;
        case WM_LBUTTONDOWN:
            game->cursorDown(LEFT);
            break;
        case WM_LBUTTONUP:
            game->cursorUp(LEFT);
            break;
            
        case WM_MOUSEMOVE:
            game->cursorMoved((float)(WORD)lParam, (float)(lParam>>16));
            /*mouse.dx = mouse.x - mouse.bx;
            mouse.dy = mouse.y - mouse.by;
            mouse.bx = mouse.x;
            mouse.by = mouse.y;*/
            break;            
			
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			game->keyDown(wParam);
			break;

		case WM_CHAR:
			game->charEntered(wParam);
			break;
		case WM_SYSKEYUP: 
		case WM_KEYUP:
			game->keyUp(wParam);
			break; 
		
		default:
			return DefWindowProc(hWnd,message,wParam,lParam);
	}
	
	return 0;
}


bool createWindow(HINSTANCE hInstance, int nWinMode) {
	WNDCLASS wc; 
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	wc.lpfnWndProc = WindowFunc;
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = hInstance; 
	wc.hIcon = NULL; 
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = NULL; 
	wc.lpszMenuName = NULL; 
	wc.lpszClassName = "OpenGLWinClass"; 

	if (!RegisterClass(&wc))
	{ 
		return false; 
	} 

	if(FULL_SCREEN)
	{ 
		hWnd = CreateWindow( 
		"OpenGLWinClass", 
		"3D Game - by MuHAOS",
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL); 
	} 
	else 
	{ 
		hWnd = CreateWindow( 
		"OpenGLWinClass",
		"3D Game - by MuHAOS",
		WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE | WS_SIZEBOX,
		0, 0, 
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL); 
	} 
	
	if (!hWnd)
	{ 
		return false; 
	} 

	if(FULL_SCREEN)
	{ 
		DEVMODE dmScreenSettings;
		memset (&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = SCREEN_WIDTH;
		dmScreenSettings.dmPelsHeight = SCREEN_HEIGHT;
		dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN);
	} 

	// set 1 second timer
	SetTimer(hWnd, 0, 1000, NULL);
	
	ShowWindow(hWnd,nWinMode);
	UpdateWindow(hWnd); 
	SetFocus(hWnd);

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR str,int nWinMode) 
{ 
	MSG msg; 
	
	if (!createWindow(hInstance, nWinMode)) {
		MessageBox(0, "Error while creating window","Error!", MB_OK | MB_ICONERROR); 
		return EXIT_FAILURE;
	}
	
	if (!setupPixelFormat(hWnd)) {
		MessageBox(0, "Error while setup pixel fromat","Error!", MB_OK | MB_ICONERROR); 
		return EXIT_FAILURE;
	}
	
	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq) )
	{
		MessageBox(0, "Error while QueryPerformanceFrequency","Error!", MB_OK | MB_ICONERROR); 
		return EXIT_FAILURE;
	}
	
	game = new Game(new WindowsOSAdapter(hDC, hInstance));
	game->initialize();
	game->luaVM->doString("dofile(\"./scripts/game.lua\")");
	
	unsigned long int currentT;
	unsigned long int lastT;


	while (!game->needStop())
	{ 
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
		{ 
			if (GetMessage(&msg, NULL, 0, 0))
			{ 
				TranslateMessage (&msg); 
				DispatchMessage (&msg); 
			} 
			else return EXIT_FAILURE; 
		} 
		
		
		QueryPerformanceCounter((LARGE_INTEGER*)&perfCurrent);
		currentT = GetTickCount();

		static float c = 1000000/30;
		
		game->iterate(perfDiff / c /*lastT / 33.3*/ );
		
		lastT =  (GetTickCount() - currentT);
		QueryPerformanceCounter((LARGE_INTEGER*)&perfLast);
		perfDiff = ((perfLast - perfCurrent) * 1000000) / perfFreq;
	
	}
	game->finalize();
	
	return EXIT_SUCCESS;
}







