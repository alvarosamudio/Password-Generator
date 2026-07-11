# Password Generator

A random password generator for Deepin Linux built with Qt5 and DTK (Deepin Tool Kit).

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

- Deepin Linux or any Debian-based system with DTK5
- Qt 5.15+
- DTK5 (Deepin Tool Kit): `libdtkwidget5`, `libdtkcore5`

## Build from source

### Prerequisites

```bash
sudo apt install qtbase5-dev qt5-qmake libdtkwidget-dev pkg-config build-essential
```

### Compile

```bash
export QT_SELECT=5
qmake app.pro PREFIX=/usr
make -j$(nproc)
```

### Run tests

```bash
qmake testlib.pro
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