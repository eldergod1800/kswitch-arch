# KSwitch Arch PKGBUILD

This repository contains an Arch Linux PKGBUILD for building and installing
[KSwitch](https://github.com/Zyren-Lab/KSwitch) from source.

KSwitch is developed and maintained by Zyren-Lab and licensed under GPLv3.
This repository only provides packaging files; the source code is fetched
directly from the upstream GitHub repository.

This will download the upstream KSwitch sources, build the package, and
install it on your system.
Requirements

    Arch Linux (or Arch-based distro)
    base-devel group installed
    git installed


## Build and install

```bash
git clone https://github.com/eldergod1800/kswitch-arch.git
cd kswitch-arch
makepkg -si


