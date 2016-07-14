// NtLoadDriver-C++.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <Windows.h>
#include <ntsecapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <iostream>
#include <ntdef.h>



#define REGISTRY_PATH_PREFIX L"System\\CurrentControlSet\\Services\\"


 // https://github.com/iceboy-sjtu/ntdrvldr/blob/master/main.c
bool EnablePrivilege(
	LPCWSTR lpPrivilegeName
	)
{
	TOKEN_PRIVILEGES Privilege;
	HANDLE hToken;
	DWORD dwErrorCode;

	Privilege.PrivilegeCount = 1;
	Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
		&Privilege.Privileges[0].Luid))
		return GetLastError();

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))
		return GetLastError();

	if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
		NULL, NULL)) {
		dwErrorCode = GetLastError();
		CloseHandle(hToken);
		return dwErrorCode;
	}

	CloseHandle(hToken);
	return TRUE;
}

bool CreateRegKey()
{
	// RegSetKey Values
	ULONG dwErrorCode;
	LPWSTR lpSubKey = L"System\\CurrentControlSet\\Services\\NPF2";
	HKEY hKey = HKEY_LOCAL_MACHINE;
	DWORD Reserved = 0;
	LPTSTR lpClass = NULL;
	DWORD dwOptions = 0;
	REGSAM samDesired = KEY_ALL_ACCESS;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	HKEY phkResult;
	// Set Value settings
	DWORD dwDisposition;
	DWORD dwServiceType = 1;
	DWORD dwServiceErrorControl = 1;
	DWORD dwServiceStart = 3;
	// RegSetValueExW values for image
	LPCTSTR lpValueImagePath = L"ImagePath";
	DWORD dwType = REG_EXPAND_SZ;
	LPWSTR ServiceImagePath = L"\\??\\npf.sys";
	SIZE_T ServiceImagePathSize;
	// RegSetValueExW values for Type
	LPCTSTR lpValueImageType = L"Type";
	DWORD dwTypeDWord = REG_DWORD;
	// RegSetValueExW values for Error
	LPCTSTR lpValueError = L"ErrorControl";
	// RegSetValueExW values for Start
	LPCTSTR lpValueStart = L"Start";

	dwErrorCode = RegCreateKeyExW(hKey,
								lpSubKey,
								Reserved,
								lpClass,
								dwOptions,
								samDesired,
								lpSecurityAttributes,
								&phkResult,
								&dwDisposition);
	if (dwErrorCode) {
		RegCloseKey(phkResult);
		std::cout << "Setting driver REG error code: " << dwErrorCode << std::endl;
		return FALSE;
	}
	// TODO : how to get byte size?
	ServiceImagePathSize = (lstrlenW(ServiceImagePath) + 1) * sizeof(WCHAR);
	dwErrorCode = RegSetValueExW(phkResult,
								lpValueImagePath,
								Reserved,
								dwType,
								(const BYTE *)ServiceImagePath,
								ServiceImagePathSize);
	if (dwErrorCode) {
		RegCloseKey(phkResult);
		return FALSE;
	}
	// Set the Type for key
	dwErrorCode = RegSetValueExW(phkResult,
								lpValueImageType,
								Reserved,
								dwTypeDWord,
								(const BYTE *)&dwServiceType,
								sizeof(DWORD));
	if (dwErrorCode) {
		RegCloseKey(phkResult);
		return FALSE;
	}
	// Set the ErrorControl key
	dwErrorCode = RegSetValueExW(phkResult,
								lpValueError,
								Reserved,
								dwTypeDWord,
								(const BYTE *)&dwServiceErrorControl,
								sizeof(DWORD));
	if (dwErrorCode) {
		RegCloseKey(phkResult);
		return FALSE;
	}
	// Set the Start key
	dwErrorCode = RegSetValueExW(phkResult,
								lpValueStart,
								Reserved,
								dwTypeDWord,
								(const BYTE *)&dwServiceStart,
								sizeof(DWORD));
	if (dwErrorCode) {
		RegCloseKey(phkResult);
		return FALSE;
	}
	RegCloseKey(phkResult);
	return TRUE;
}

int main()
{
	if (!EnablePrivilege(L"SeLoadDriverPrivilege"))
	{
		std::cout << "Setting driver privilege failed" << std::endl;
	}
	else {
		std::cout << "Setting driver privilege is GTG!" << std::endl;
		if (!CreateRegKey()) {
			std::cout << "Setting driver REG keys failed" << std::endl;
		}
		else {
			std::cout << "Setting REG keys was ok" << std::endl;
		}
	}

	std::cout << "Press Enter to Continue";
	getchar();
	return 0;
}
