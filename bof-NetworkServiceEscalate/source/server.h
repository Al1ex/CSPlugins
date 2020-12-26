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

/**
 *
 * @ brief : Connects to a named pipe server using 
 * the file API. On success, it will return a handle
 * to the pipe as an client.
 *
 * @ arg LPCSTR lpPipePath : Path to the pipe, in the
 * format of \\[host]\pipe\PipeStr.
 *
 * @ ret : Returns a handle to the server as a client
 * on success, else INVALID_HANDLE_VALUE on failure.
 *
**/
HANDLE NamedPipeFile( IN LPCSTR lpPipePath );

/**
 *
 * @ brief : Creates a named pipe using the kernel32
 * CreateNamedPipeA API. On success, it will return 
 * a handle to this pipe to be used by the operator.
 *
 * @ arg LPCSTR lpPipePath : Full path to a named 
 * pipe in the classic format \\.\pipe\PipeStr to be
 * opened.
 *
 * @ ret : Returns a pointer to NPFS pipe on success,
 * else returns INVALID_HANDLE_VALUE.
 *
**/
HANDLE NamedPipeOpen( IN LPCSTR lpPipePath );


/**
 *
 * @ brief : Disconnects an active named pipe client
 * from the listening server, and closes the server 
 * handle to avoid accepting connections.
 *
 * @ arg HANDLE lpNamePipe : Handle to a pipe created
 * with NamedPipeOpen().
 *
 * @ ret : If the server successfully disconnects the
 * client, and closes the handle, the function will 
 * return TRUE, else FALSE on failure.
 *
**/
BOOL NamedPipeFree( IN HANDLE lpNamePipe );

/**
 *
 * @ brief : Blocks the current thread and waits for 
 * a connection with a pipe created with PIPE_WAIT.
 * If a connection has been recieved before the wait
 * occurs, the function will return TRUE.
 *
 * @ arg HANDLE lpNamePipe : Pointer to a named pipe
 * handle.
 *
 * @ ret : Returns TRUE if a connection has been recieved
 * else FALSE on any error that may occur.
 *
**/
BOOL NamedPipeWait( IN HANDLE lpNamePipe );
