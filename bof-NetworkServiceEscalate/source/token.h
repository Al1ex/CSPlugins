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
 * @ brief : Locates an impersonation token for the
 * NT AUTHORITY\SYSTEM user within the target 
 * process. 
 *
 * @ arg HANDLE hProcess : Handle to an opened process
 * to enum for process handles.
 *
 * @ ret : Returns a pointer to a useable handle to
 * impersonate. The target must close the handle 
 * when they are done.
 *
**/
HANDLE TokenGetSystemTokenFromProcess( IN HANDLE hProcess );
