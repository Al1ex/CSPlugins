# Adapted from Powershell Empire (https://github.com/PowerShellEmpire/Empire)
# Supply a url, opens a hidden internet explorer on the url
# Use this for youtube sites
# I removed the volume up functionality because I couldn't get it to work
# @Und3rf10w

Function Open-HiddenInternetExplorer
{
    [CmdletBinding()]
    Param (
        [Parameter(Mandatory = $False, Position = 0)]
        [ValidateNotNullOrEmpty()]
        [String] $VideoURL = $null
    )
    if (!$VideoURL) 
    {
        Write-Host "Invalid URL supplied. Exiting..."
        Exit 1
    }
    #Create hidden IE Com Object
    $IEComObject = New-Object -com "InternetExplorer.Application"
    $IEComObject.visible = $False
    $IEComObject.navigate($VideoURL)
}