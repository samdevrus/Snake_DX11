////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Инициализация и запуск.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Завершение и освобождение памяти.
	System->Shutdown();
	delete System;
	System = 0;
	return 0;
}