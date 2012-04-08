#include "WindowsOSAdapter.h"
#include <windows.h>

WindowsOSAdapter::WindowsOSAdapter(HDC _hDC, HINSTANCE _hInstance)
{
	hDC = _hDC;
	hInstance = _hInstance;
}

int WindowsOSAdapter::getScreenWidth() {
	return 480;
}


int WindowsOSAdapter::getScreenHeight() {
	return 320;
}


void WindowsOSAdapter::swapBuffers()
{
	SwapBuffers(hDC);
}


unsigned long int WindowsOSAdapter::getTickCount()
{
	return GetTickCount();	
}


string WindowsOSAdapter::getWorkingPath() {
	static char buff[1024];
	GetModuleFileName(hInstance, buff, 1024);

	for (int i = 1; i < 1024; i++) {
		if (buff[1024 - i] == '\\') {
			buff[1024 - i + 1] = 0;
			break;
		}
	}
	return string(buff);
}


string WindowsOSAdapter::getResourcePath() {
	return getWorkingPath();
}


void WindowsOSAdapter::terminate()
{
	exit(0);
}


void WindowsOSAdapter::showMessage(const string title, const string text)
{
	MessageBox(0, text.c_str(), title.c_str(), MB_OK);
}


WindowsOSAdapter::~WindowsOSAdapter()
{
}
