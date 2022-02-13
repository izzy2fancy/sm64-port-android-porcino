https://user-images.githubusercontent.com/58743577/153742648-51787e63-eb45-4825-be48-cd1980f85442.mp4



# sm64ex Android Port
If you want to compile Super Mario 64 for Android on PC you'll probably want to clone [this repo](https://github.com/VDavid003/sm64-port-android-base) instead!
If you want to compile on Android using [Termux](https://play.google.com/store/apps/details?id=com.termux), follow these instructions in Termux:

**Install dependencies:**
```sh
pkg install git wget make python getconf zip apksigner clang
```

**Clone the repository:**
```sh
git clone https://github.com/VDavid003/sm64-port-android --branch ex/nightly
cd sm64-port-android
```

**Copy in your baserom:**

Do this using your default file manager (on AOSP, you can slide on the left and there will be a "Termux" option there), or using Termux
```sh
termux-setup-storage
cp /sdcard/path/to/your/baserom.z64 ./baserom.us.z64
```

**Get SDL includes:**
```sh
./getSDL.sh
```

**Build:**
```sh
# if you have more cores available, you can increase the --jobs parameter
make --jobs 4
```

**Enjoy your apk:**
```sh
ls -al build/us_pc/sm64.us.f3dex2e.apk
```

# sm64ex
Fork of [sm64-port/sm64-port](https://github.com/sm64-port/sm64-port) with additional features. 

Feel free to report bugs and contribute, but remember, there must be **no upload of any copyrighted asset**. 
Run `./extract_assets.py --clean && make clean` or `make distclean` to remove ROM-originated content.

Please contribute **first** to the [nightly branch](https://github.com/sm64pc/sm64ex/tree/nightly/). New functionality will be merged to master once they're considered to be well-tested.

*Read this in other languages: [Español](README_es_ES.md), [Português](README_pt_BR.md) or [简体中文](README_zh_CN.md).*

## New features

 * Options menu with various settings, including button remapping.
 * Optional external data loading (so far only textures and assembled soundbanks), providing support for custom texture packs.
 * Optional analog camera and mouse look (using [Puppycam](https://github.com/FazanaJ/puppycam)).
 * Optional OpenGL1.3-based renderer for older machines, as well as the original GL2.1, D3D11 and D3D12 renderers from Emill's [n64-fast3d-engine](https://github.com/Emill/n64-fast3d-engine/).
 * Option to disable drawing distances.
 * Optional model and texture fixes (e.g. the smoke texture).
 * Skip introductory Peach & Lakitu cutscenes with the `--skip-intro` CLI option
 * Cheats menu in Options (activate with `--cheats` or by pressing L thrice in the pause menu).
 * Support for both little-endian and big-endian save files (meaning you can use save files from both sm64-port and most emulators), as well as an optional text-based save format.

Recent changes in Nightly have moved the save and configuration file path to `%HOMEPATH%\AppData\Roaming\sm64ex` on Windows and `$HOME/.local/share/sm64ex` on Linux. This behaviour can be changed with the `--savepath` CLI option.
For example `--savepath .` will read saves from the current directory (which not always matches the exe directory, but most of the time it does);
   `--savepath '!'` will read saves from the executable directory.

## Building
For building instructions, please refer to the [wiki](https://github.com/sm64pc/sm64ex/wiki).

**Make sure you have MXE first before attempting to compile for Windows on Linux and WSL. Follow the guide on the wiki.**
