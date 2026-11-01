# Notice

Generals-Modern is a modified version of the source code of Command & Conquer Generals and
Command & Conquer Generals Zero Hour that Electronic Arts Inc. released in February 2025 under the
GNU General Public License, version 3, with additional terms (see [LICENSE.md](LICENSE.md)). It is
not the original program and is not affiliated with, endorsed by or sponsored by Electronic Arts.
"Command & Conquer" and related marks belong to Electronic Arts Inc.; the licence grants no right to
use them. No game assets are in this repository: running the game needs a legitimately obtained copy
of Zero Hour.

## Zero Hour Reforged

Parts of this work are adapted from, or follow approaches taken by, **Zero Hour Reforged**
(https://github.com/olcayseygan/CnCGeneralsZH-Reforged) by **Olcay Seygan**, with its macOS and
Linux port by **İlyas Akın**, released under GPL-3.0-or-later as a modification of EA's source.
Every file or change that takes something from it says so where it happens. So far:

| Here | From Reforged |
|:--|:--|
| `GeneralsMD/Code/CMakeLists.txt`: configure-time pointer-size guard | the same guard, inverted (Reforged is x64-only), CMakeLists.txt:105-109 |
| `GeneralsMD/Code/CMakeLists.txt`: wwshade left header-only | Reforged's CMakeLists.txt does the same, for the same reason |
| `W3DVolumetricShadow.cpp`: shader-mesh shadow branch compiled out | Reforged's W3DVolumetricShadow.cpp |
| `GameEngine/Include/Common/SafeDisc/CdaPfn.h` | Reforged's stand-in of the same name |
| LZH-Light `_lz.h`: history buffer cleared | Reforged's `lzhl-clear-history.patch` |

The planned sound work ports Reforged's Miles-API implementation over miniaudio; it will be listed
here, with its authors, when it lands.

## Third-party code in this repository

| Component | Where | Version | Licence and copyright |
|:--|:--|:--|:--|
| bgfx | `GeneralsMD/Code/Libraries/Source/bgfx` | upstream `4d53b55ce2` (2025-05-17) | BSD-2-Clause, © 2010-2025 Branimir Karadžić (`LICENSE` in the folder) |
| bx | `GeneralsMD/Code/Libraries/Source/bx` | upstream `d4096a8446` (2025-04-26) | BSD-2-Clause, © 2010-2025 Branimir Karadžić |
| bimg | `GeneralsMD/Code/Libraries/Source/bimg` | closest upstream `446b9eb111` (2025-03-06); 4 third-party text files differ | BSD-2-Clause, © 2010-2025 Branimir Karadžić |
| zlib | `zlib-extract/zlib-1.3.1` | 1.3.1, unmodified | zlib licence, © 1995-2022 Jean-loup Gailly and Mark Adler (`LICENSE` in the folder) |
| LZH-Light | `GeneralsMD/Code/Libraries/Source/Compression/LZHCompress/CompLib*` | 1.0, from github.com/TheSuperHackers/lzhl-1.0 at `dfd96e2` | © 1998 Sergey Ignatchenko; permission to use, copy, modify, distribute and sell for any purpose, provided the notice is kept and altered versions are marked. **Altered:** `_lz.h` clears the history buffer (marked in the file). |

Each component's own licence notice is kept in its files. This table lists the components added or
recovered during the modernization so far; it is being completed as the rest of the tree is audited.
