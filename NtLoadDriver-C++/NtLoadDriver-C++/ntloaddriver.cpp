
#include "stdafx.h"
#include <windows.h>
#include <ntdef.h>
#include <stdio.h>
#include "ntdriver.h"
#include <Winternl.h>
#include <Ntsecapi.h>

typedef NTSTATUS(__stdcall *TFNNtTestAlert)();
// NTSTATUS is LONG, in case that isn't defined in the above headers

#define DRIVER_SERVICE_NAME_PREFIX L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"

NTSYSAPI
NTSTATUS
NtLoadDriver(
	PUNICODE_STRING DriverServiceName
	);


NTSYSAPI
ULONG
RtlNtStatusToDosError(
	NTSTATUS Status
	);

ULONG
LoadDriver(
	const LPWSTR DriverName
	)
{
	UNICODE_STRING DriverServiceName;
	RtlInitUnicodeString(&DriverServiceName, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\npf.sys");
	ULONG dwErrorCode;
	NTSTATUS Status;


	Status = NtLoadDriver(&DriverServiceName);
	if (!NT_SUCCESS(Status))
		return RtlNtStatusToDosError(Status);

	return 0;
}