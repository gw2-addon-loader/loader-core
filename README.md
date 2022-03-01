# loader-core

Core addon loading library for Guild wars 2

Original idea: https://github.com/Archomeda/gw2-addon-loader

[![Build status](https://ci.appveyor.com/api/projects/status/m356po5lxrq24vng?svg=true)](https://ci.appveyor.com/project/megai2/loader-core)

[![](https://img.shields.io/discord/384735285197537290.svg?logo=discord&logoColor=f0f0f0)](https://discord.gg/5epjQEj)

# Features

In short: loads addons on game start and provides basic addon API functionality.

## Addon DLL loading

On start loads compatible DLLs than conform to name "<GW2 directory>/addons/addon_name/gw2addon_addon_name.dll" and have addon exports. 
   
## Addon API

Addon API includes several parts:

1. Addon loading functions to load and unload addons on fly
2. Function registry to share functions between addons
3. Event registry to perform some sender/subscriber logic.

Details in <include/gw2al_api.h>

# Installation
 
1. Download and extract the archive `loader_core_*.zip` found in the [latest release](https://github.com/gw2-addon-loader/loader-core/releases/latest).
2. Place the contents (`bin64`, `addonLoader.dll`, `d3d11.dll`, `dxgi.dll`) in your game installation directory (with the default game install path, this would be `C:\Program Files\Guild Wars 2`).
