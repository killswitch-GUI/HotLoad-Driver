/**********************************************************************
*  TEST_1
*  Dynamically Load a Driver
**********************************************************************/
#include <windows.h>
#include <stdio.h>
#include "base64.h"
#include <fstream>      // std::ofstream
#include <iostream>
//#include "stdafx.h"

bool writePacket2DLL();
bool writeWpcapDLL();
bool writeNpfSYS();
bool isWow64bit();
bool installDriver();
/*********************************************************
*   Main Function Entry
*********************************************************/

// tested on Windows 10 x64 using x86 complie
// tested on Windows 7 x86 using x86 compile

bool installDriver()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS ss;
	LPCSTR serviceModName;
	LPCSTR serviceDisplayName;
	LPCSTR dvrPath;
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	serviceModName = "MSKrnlSvc";
	serviceDisplayName = "MS Kernel Service";
	if (isWow64bit())
	{
		dvrPath = "C:\\Windows\\SysWOW64\\npf.sys";
	}
	else
	{
		dvrPath = "C:\\Windows\\System32\\npf.sys";
	}
	writePacket2DLL();
	writeWpcapDLL();
	writeNpfSYS();
	printf("[*] Loading Driver...\n");

	if (hSCManager)
	{
		printf("[+] SC Manager Opened\n");
		printf("[*] Creating Service...\n");
		hService = CreateService(hSCManager, serviceModName, serviceDisplayName, SERVICE_START | DELETE | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, dvrPath, NULL, NULL, NULL, NULL, NULL);

		if (!hService)
		{
			hService = OpenService(hSCManager, serviceModName, SERVICE_START | DELETE | SERVICE_STOP);
		} // end if
		if (hService == NULL) {
			printf("OpenService failed (%d) \n", GetLastError());
		} // end if
		else printf("[+] Service Created \n");

		if (hService)
		{
			StartService(hService, 0, NULL);
			printf("[+] Started Service: MSKrnlSvc \n");
			getchar();
			hService = OpenService(hSCManager, serviceModName, DELETE);
			ControlService(hService, SERVICE_CONTROL_STOP, &ss);

			if (CloseServiceHandle(hSCManager) == NULL) {
				printf("[-] Service Handle Close Failed... (%d) \n", GetLastError());

			} //end if
			else printf("[+] Handle to SC Manager closed \n");

			if (DeleteService(hService)) {
				printf("[+] Service Scheduled to be Deleted \n");
			} //end if
			else {
				printf("[-] Error Deleting Service \n");
			}
			if (CloseServiceHandle(hService)) {
				printf("[+] Service Handle Closed \n");
			}// end if
		} // end if	
	} // end if
	printf("Press Enter to close application & terminate Service \n");
	getchar();
	return 0;
}
bool isWow64bit()
{
	SYSTEM_INFO lpSystemInfo;
	GetNativeSystemInfo(&lpSystemInfo);
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		//memoryLog(0, "isWow64bit() AMD64 detected");
		return TRUE;
	}
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		//memoryLog(2, "isWow64bit() IA-64 detected");
		return TRUE;
	}
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		//memoryLog(0, "isWow64bit() x86 detected");
		return FALSE;
	}
	if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)
	{
		// WARNING: defualt to x86
		//memoryLog(2, "isWow64bit() UNKOWN arch detected");
		return FALSE;
	}
	return FALSE;

}
bool writePacket2DLL() {
	static const char Packet_2_dll[] = { 
		#include "src\Packet_dll_2.h"
	};
	unsigned int Packet_2_dll_len = 98040;
	std::string path;
	if (isWow64bit())
	{
		path = "C:\\Windows\\SysWOW64\\Packet.dll";
	}
	else
	{
		path = "C:\\Windows\\System32\\Packet.dll";
	}
	std::cout << "[+] Path being used: " << path << std::endl;
	std::ofstream outfile(path, std::ios_base::binary);
	outfile.write(Packet_2_dll, Packet_2_dll_len);
	std::cout << "[+] Packet.dll wrote" << std::endl;
	outfile.close();
	return TRUE;

}

bool writeWpcapDLL() {
	static const char wpcap_2_dll[] = {
		#include "src\wpcap_dll.h"
	};
	unsigned int wpcap_2_dll_len = 282360;
	std::string path;
	if (isWow64bit())
	{
		path = "C:\\Windows\\SysWOW64\\wpcap.dll";
	}
	else
	{
		path = "C:\\Windows\\System32\\wpcap.dll";
	}
	std::cout << "[+] Path being used: " << path << std::endl;
	std::ofstream outfile(path, std::ios_base::binary);
	outfile.write(wpcap_2_dll, wpcap_2_dll_len);
	std::cout << "[+] wpcap.dll wrote" << std::endl;
	outfile.close();
	return TRUE;

}

bool writeNpfSYS() {
	static const char npf_sys[] = {
#include "src\npf_sys.h"
	};
	unsigned int npf_sys_len = 36600;
	std::string path;
	if (isWow64bit())
	{
		path = "C:\\Windows\\SysWOW64\\nfp.sys";
	}
	else
	{
		path = "C:\\Windows\\System32\\npf.sys";
	}
	std::cout << "[+] Path being used: " << path << std::endl;
	std::ofstream outfile(path, std::ios_base::binary);
	outfile.write(npf_sys, npf_sys_len);
	std::cout << "[+] npf.syf wrote" << std::endl;
	outfile.close();
	return TRUE;

}