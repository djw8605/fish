#ifndef FISHSPLASH_H
#define FISHSPLASH_H

#ifdef WIN32
#include <windows.h>
#endif

#include "aquarium.h"

class FishSplash: public Runnable{

public:
	FishSplash();
#ifdef WIN32
	FishSplash(const char* className, HINSTANCE hInstance);
	HWND getSplashHwnd() { return _splashHwnd; };
#endif
	void Display();
	void Close();
	void Start();
private:
#ifdef WIN32
	HANDLE _mainThread;
	HWND _splashHwnd;
	HINSTANCE _hInstance;
	const char* _className;
	BOOL _exiting;
#endif
	int _windowWidth, _windowHeight;











};

#endif



