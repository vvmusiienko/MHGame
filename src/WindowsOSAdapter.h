#ifndef WINDOWSOSADAPTER_H_
#define WINDOWSOSADAPTER_H_

#include "windows.h"
#include "IOSAdapter.h"
#include <string>

using namespace std;

class WindowsOSAdapter : public IOSAdapter
{
private:
	HDC hDC;
	HINSTANCE hInstance;
	
public:
	WindowsOSAdapter(HDC _hDC, HINSTANCE _hInstance);
	void swapBuffers();
	unsigned long int getTickCount();
	string getWorkingPath();
	string getResourcePath();
	void terminate();
	void showMessage(const string title, const string text);
	int getScreenWidth();
	int getScreenHeight();
	
	virtual ~WindowsOSAdapter();
};

#endif /*WINDOWSOSADAPTER_H_*/
