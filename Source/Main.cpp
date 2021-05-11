// Include Directories
// ========================================================
#include <iostream>
#include <qapplication.h>
#include "MainWindow.h"
#include <WinInet.h>
#include <Windows.h>

int main(int argc, char* argv[])
{
	// Ensure that date/time is okay
	// SOFTWARE IS VOID AFTER JAN 2020
	HINTERNET hInternetSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hInternetFile = InternetOpenUrl(hInternetSession,
		"http://www.google.com.au", 0, 0,
		INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);

	typedef struct _SYSTEMTIME {
		WORD wYear;
		WORD wMonth;
		WORD wDayOfWeek;
		WORD wDay;
		WORD wHour;
		WORD wMinute;
		WORD wSecond;
		WORD wMilliseconds;
	} SYSTEMTIME, *PSYSTEMTIME;

	SYSTEMTIME sysTime;
	SecureZeroMemory(&sysTime, sizeof(SYSTEMTIME));

	DWORD dwSize = sizeof(SYSTEMTIME);
	if (!HttpQueryInfo(hInternetFile, HTTP_QUERY_DATE |
		HTTP_QUERY_FLAG_SYSTEMTIME, &sysTime, &dwSize, NULL))
	{
		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetFile);

		/*for (int i = 0; i < 100; i++)
		{
			cout << ">>SOFTWARE MUST ALWAYS BE USED ONLINE<<\n";
			cout << "---------------------------------------\n";
		}
		for (int i = 0; i < 15; i++)
		{
			Beep(523, 750);
			Sleep(200);
		}

		return 0;*/
	}

	if (sysTime.wYear >= 2020 && sysTime.wMonth >= 1)
	{
		/*for (int i = 0; i < 100; i++)
		{
			cout << ">>SOFTWARE LICENSE EXPIRED. CONTACT SAM HISLOP-LYNCH FOR RENEWAL<<\n";
			cout << "------------------------------------------------------------------\n";
		}
		for (int i = 0; i < 15; i++)
		{
			Beep(523, 750);
			Sleep(200);
		}
		return 0;*/
	}

	// Start the GUI
	QApplication app(argc, argv);
	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
