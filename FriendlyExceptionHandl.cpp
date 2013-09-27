#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

BOOL WINAPI MyMiniDumpCallback(
								 PVOID                            pParam, 
								 const PMINIDUMP_CALLBACK_INPUT   pInput, 
								 PMINIDUMP_CALLBACK_OUTPUT        pOutput 
								 ) 
{
	BOOL bRet = FALSE; 


	// Check parameters 

	if( pInput == 0 ) 
		return FALSE; 

	if( pOutput == 0 ) 
		return FALSE; 


	switch( pInput->CallbackType ) 
	{
	case IncludeModuleCallback: 
			bRet = TRUE; 
		break; 
	case IncludeThreadCallback: 
			bRet = TRUE; 
		break; 
	case ModuleCallback: 
			bRet = TRUE; 
		break; 
	case ThreadCallback: 
			// Include all thread information into the minidump 
			bRet = TRUE;  
		break; 
	case ThreadExCallback: 
			bRet = TRUE;  
		break; 
	case MemoryCallback: 
			bRet = FALSE; 
		break; 
	}
	return bRet; 
}


void GetDumpFileName(LPTSTR buff, DWORD len)
{
	SYSTEMTIME st;
	TCHAR username[64];
	TCHAR temppath[64];

	DWORD cbSize = 63;
	GetUserName(username, &cbSize);

	cbSize = 63;
	//GetTempPath(cbSize, temppath);
	_tcscpy(temppath, _T(".\\dump"));

	GetSystemTime(&st);
	_sntprintf(buff, len, 
		_T("%s\\%s_%02d%02d%02d_%02d%02d%02d.dmp"),
		temppath,
		username, 
		st.wYear, st.wMonth, st.wDay, 
		st.wHour + 8, st.wMinute, st.wSecond);
}

LONG hadCrashed = 0;
TCHAR workdir[MAX_PATH];


LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{

	if (hadCrashed != 0)
		return EXCEPTION_EXECUTE_HANDLER;

	InterlockedExchange(&hadCrashed, 1);

	
	TCHAR dumpName[MAX_PATH * 2];
	//TCHAR cmdLine[MAX_PATH * 2];
	//TCHAR iniName[MAX_PATH], value[MAX_PATH],iniNameTemp[MAX_PATH];

	HANDLE hFile = INVALID_HANDLE_VALUE;
	LPCTSTR msg = NULL;
	MINIDUMP_EXCEPTION_INFORMATION mei = {GetCurrentThreadId(), pExceptionInfo, TRUE};
	MINIDUMP_CALLBACK_INFORMATION mci = {MyMiniDumpCallback, 0}; 
	MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(
		MiniDumpWithPrivateReadWriteMemory | 
		MiniDumpWithFullMemory |
		MiniDumpWithDataSegs | 
		MiniDumpWithHandleData |
		MiniDumpWithUnloadedModules ); 




	GetDumpFileName(dumpName, MAX_PATH * 2);

	hFile = CreateFile(dumpName, GENERIC_ALL, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}

	MiniDumpWriteDump(
		GetCurrentProcess(), 
		GetCurrentProcessId(),
		hFile,
		MiniDumpNormal,
		&mei,
		NULL, 
		&mci);
	CloseHandle(hFile);

	return EXCEPTION_EXECUTE_HANDLER;
}


void EnableFriendllyExceptionHandle(BOOL bEnable)
{
	GetCurrentDirectory(MAX_PATH, workdir);
	LPTOP_LEVEL_EXCEPTION_FILTER pHandler = 
		bEnable ? MyUnhandledExceptionFilter : NULL;
	SetUnhandledExceptionFilter(pHandler);
}
#endif
