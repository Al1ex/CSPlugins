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

#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include "../external/beacon.h"

#include "api.h"
#include "server.h"
#include "service.h"
#include "token.h"

typedef datap BEACON;

#define _ERROR(x) BeaconPrintf(CALLBACK_ERROR, "ERROR %s(): 0x%x", \
		x, KERNEL32$GetLastError())
