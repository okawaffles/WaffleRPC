# WaffleRPC
A Discord RPC sysmodule that compiles + works on the latest devkitPro builds. **If it doesn't compile/work, please open an issue!! I will fix it ASAP!**

## Sysmodule Info
Sysmodule ID: `0x010000000000CAFE`

## Building
Clone repo, install devkitPro and switch development libraries, run `make` in the directory you wish to compile (sysmodule/GUI). I highly recommend using vscode and the included `c_cpp_properties.json` as it will allow Intellisense with devkitPro libraries. You may need to change the path/update the version number.

## TODO
I would like to get a function which can toggle the sysmodule without rebooting the system. This won't gracefully shut down the sysmodule but it would be a nice touch.

## Credits
Heavily references [SwitchPresence-Rewritten](https://github.com/SunResearchInstitute/SwitchPresence-Rewritten) despite being built from the ground up. It also uses a modified Makefile taken from that repository.
