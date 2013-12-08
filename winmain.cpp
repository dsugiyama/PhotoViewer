#include <Windows.h>
#include "Application.h"

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, INT nCmdShow)
{
	Application app;
	app.Run();

	return 0;
}
