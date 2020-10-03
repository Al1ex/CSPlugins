
## PowerView Aggressor Script for CobaltStrike 

#### A user menu for [@harmjoy's](https://twitter.com/harmj0y) [PowerView](https://github.com/PowerShellMafia/PowerSploit/blob/dev/Recon/PowerView.ps1)
#### and [SharpView](https://github.com/tevora-threat/SharpView)

#### Very similar to the PowerView v2 [aggressor script](https://github.com/tevora-threat/aggressor-powerview)

All functions listed in the PowerView about page are included in this with all arguments for each function.

![powerview1](https://user-images.githubusercontent.com/9096315/43167904-31311bf4-8f50-11e8-9c89-1b86fb69e5ff.png)

Has the ability to accept credentials (format: domain\user password to parse correctly) 

![powerview2](https://user-images.githubusercontent.com/9096315/43167890-2d9f74e0-8f50-11e8-80e9-c994c54eb56b.png)

With SharpView now has the ability to leverage execute-assembly and like before PowerPick and PowerShell

![powerview3](https://user-images.githubusercontent.com/9096315/43167896-30078ca4-8f50-11e8-9a6c-dcd333ddae1b.png)


Now has a help menu that provides the description and Parameters from the PowerView.ps1 source

![powerview4](https://user-images.githubusercontent.com/9096315/43167905-31f7ae72-8f50-11e8-93b0-edb1df5b16f7.png)


The script does not automatically do powershell-import for PowerView you must manually do that first. Additionally, depending on your placement of SharpView you may need to change the location in the $sharpviewlocation variable.

Please note this requires PowerView 3.0 (current dev branch) and SharpView (only version)

Blog [Here](https://threat.tevora.com/a-sharpview-and-more-aggressor)