// crashtestdummy.cpp : main source file for crashtestdummy.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "CrashRpt.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h> 

CAppModule _Module;


FILE* g_hLog = NULL; // Global handle to the application log file

// Define the callback function that will be called on crash
int CALLBACK CrashCallback(CR_CRASH_CALLBACK_INFO* pInfo)
{  
	// The application has crashed!

	// Close the log file here
	// to ensure CrashRpt is able to include it into error report
	if(g_hLog!=NULL)
	{
		fclose(g_hLog);
		g_hLog = NULL;// Clean up handle
	}

	// Return CR_CB_DODEFAULT to generate error report
	return CR_CB_DODEFAULT;
}

// The following function writes an entry to the log file
void log_write(LPCTSTR szFormat)
{
  if (g_hLog == NULL) 
    return; // Log file seems to be closed

  va_list args; 
  va_start(args,szFormat);
  _vftprintf_s(g_hLog, szFormat, args);
  fflush(g_hLog);
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	// Define CrashRpt configuration parameters
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("crashtestdummy");
	info.pszAppVersion = _T("1.0.0");  
	info.pszEmailSubject = _T("Error Report");  
	info.pszEmailTo = _T("travis@levion.com");
	info.uPriorities[CR_HTTP] = 3;  // First try send report over HTTP 
	info.uPriorities[CR_SMTP] = 2;  // Second try send report over SMTP  
	info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI    
	// Install all available exception handlers
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	// Restart the app on crash 
	info.dwFlags |= CR_INST_APP_RESTART; 
	info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS; 
	info.pszRestartCmdLine = _T("/restart");
	// Define the Privacy Policy URL 
	info.pszPrivacyPolicyURL = _T("http://myapp.com/privacypolicy.html"); 

	// Install crash reporting
	int nResult = crInstall(&info);    
	if(nResult!=0)  
	{    
		// Something goes wrong. Get error message.
		TCHAR szErrorMsg[512] = _T("");        
		crGetLastErrorMsg(szErrorMsg, 512);    
		_tprintf_s(_T("%s\n"), szErrorMsg);    
		return 1;
	} 

	// Set crash callback function
	crSetCrashCallback(CrashCallback, NULL);

	// Add our log file to the error report
	crAddFile2(_T("log.txt"), NULL, _T("Log File"), CR_AF_MAKE_FILE_COPY);    

	// We want the screenshot of the entire desktop is to be added on crash
	crAddScreenshot2(CR_AS_VIRTUAL_SCREEN, 0);   

	// Add a named property that means what graphics adapter is
	// installed on user's machine
	crAddProperty(_T("VideoCard"), _T("nVidia GeForce 8600 GTS"));







	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();






	errno_t err = _tfopen_s(&g_hLog, _T("log.txt"), _T("wt"));
  if(err!=0 || g_hLog==NULL)
  {
    _tprintf_s(_T("Error opening log.txt\n"));
    return 1; // Couldn't open log file
  }

  log_write(_T("Started successfully\n"));

  
  // There is a hidden error in the main() function
  // Call of _tprintf_s with NULL parameter
  TCHAR* szFormatString = NULL;
  _tprintf_s(szFormatString);


  // Close the log file
  if(g_hLog!=NULL)
  {
    fclose(g_hLog);
    g_hLog = NULL;// Clean up handle
  }

  // Uninitialize CrashRpt before exiting the main function
  crUninstall();






	return nRet;
}
