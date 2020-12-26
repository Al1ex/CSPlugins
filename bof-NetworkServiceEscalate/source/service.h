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
 * @ brief : Opens the target service and tries to
 * query its underlying process id. On success, it
 * will return the active process ID.
 *
 * @ arg LPCSTR lpServiceName : String containing the
 * full service name.
 *
 * @ ret : A process ID of the service. If it fails,
 * it will returns 0.
 *
**/
DWORD GetServiceProcessId( IN LPCSTR lpServiceName );
