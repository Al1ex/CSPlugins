# About

A sample "Beacon Object File" (COFF, really) created with the Mingw-W64 compiler (partially cause I mostly work from a Unix based environment) to escalate from NetworkService or lower privilege to SYSTEM by abusing the issue described by the brilliant James Forshaw [here](https://www.tiraniddo.dev/2020/04/sharing-logon-session-little-too-much.html).

## Credits
  - [OffSecGinger](https://twitter.com/offsec_ginger) for his random_string() function in network.cna
  - [Alex Ionescu](https://github.com/ionescu007): I ripped some of his code from FaxHell to implement the functions in token.c. As such, he deserves credit as I did not know how to properly implement them without some crazy code.
