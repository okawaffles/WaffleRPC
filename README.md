# WaffleRPC
A Discord RPC sysmodule that compiles + works on the latest devkitPro builds.

## Sysmodule Info
Sysmodule ID: `0x010000000000CAFE`

## Building
Clone repo, install devkitPro and switch development libraries, run `make` in the directory you wish to compile (sysmodule/GUI). I highly recommend using vscode and the included `c_cpp_properties.json` as it will allow Intellisense with devkitPro libraries. You may need to change the path/update the version number. `switch-sdl2` is required to build the GUI app.

## Credits
Heavily references [SwitchPresence-Rewritten](https://github.com/SunResearchInstitute/SwitchPresence-Rewritten) despite being built from the ground up. It also uses a modified Makefile taken from that repository.