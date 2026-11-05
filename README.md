# Password Generator

A random password generator for Deepin Linux built with Qt6 and DTK6 (Deepin Tool Kit).

## Features

- Generate random passwords with adjustable length (0-64 characters)
- Selectable character types: lowercase, uppercase, numbers and symbols
- Spanish-specific characters included (`ñ`, `Ñ`)
- Copy to clipboard with one click
- Native Deepin desktop look and feel
- Resizable window with flexible layout
- Real-time length indicator

## Screenshot

![Password Generator](passwordgenerator.svg)

## Requirements

- Deepin 25 or any Debian-based system with DTK6
- Qt 6.4+
- DTK6 (Deepin Tool Kit): `libdtk6widget`, `libdtk6core`

## Build from source

### Prerequisites

```bash
sudo apt install qt6-base-dev qmake6 libdtk6widget-dev pkg-config build-essential
```

### Compile

```bash
export QT_SELECT=5
qmake6 app.pro PREFIX=/usr
make -j$(nproc)
```

### Run tests

```bash
qmake6 testlib.pro
make -j$(nproc)
export QT_QPA_PLATFORM=offscreen
./test_widget
```

## Install via .deb package

Build the package:

```bash
fakeroot debian/rules binary
```

Install:

```bash
sudo dpkg -i ../password-generator_*.deb
```

## License

AGPLv3 — see [LICENSE](LICENSE)

## Authors

- alvarosamudio <alvarosamudio@protonmail.com>
- Deepin Latin Code developers