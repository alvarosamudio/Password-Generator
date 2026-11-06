# Password Generator

A random password generator for Deepin Linux built with Qt6 and DTK6 (Deepin Tool Kit).

## Features

- Generates a password as soon as it opens and every time an option changes
- Passwords come from the operating system's cryptographic random generator
- Passphrase mode: random common Spanish words (2685-word list, no accents),
  with separator, capitalized initials and an optional number
- Length from 4 to 128 characters (slider or number box)
- Character types: lowercase, uppercase, numbers and symbols (web-safe ASCII)
- Optional `ñ`/`Ñ`, option to avoid look-alike characters (`I l 1 O 0 o`) and
  to require at least one character of each selected type
- Strength meter with the estimated entropy in bits and the time a powerful
  computer would need to guess it
- Numbers and symbols are colored to tell them apart from letters
- Copy with one click or Ctrl+C; the password is wiped from the clipboard
  after 30 seconds (or when the app closes) if it is still there
- Remembers your options (never the password)
- Native Deepin (DTK6) look and feel

## Screenshot

![Password Generator](data/passwordgenerator.svg)

## Requirements

- Deepin 25 or any Debian-based system with DTK6
- Qt 6.4+
- DTK6 (Deepin Tool Kit): `libdtk6widget`, `libdtk6core`

## Build from source

### Prerequisites

```bash
sudo apt install qt6-base-dev qmake6 qt6-l10n-tools libdtk6widget-dev pkg-config build-essential
```

### Compile

```bash
mkdir build && cd build
qmake6 ../passwordgenerator.pro PREFIX=/usr
make -j$(nproc)
./src/passwordgenerator
```

### Run tests

```bash
cd build
QT_QPA_PLATFORM=offscreen make check
```

## Project layout

| Path | Contents |
| --- | --- |
| `passwordgenerator.pro` | Top-level project (builds `src` and `tests`) |
| `src/` | Application code: generator logic (`generador.*`), window (`widget.*`, `mainwindow.*`) |
| `tests/` | Qt Test suite |
| `data/` | Icon, `.desktop` file, logo, passphrase word list and the Qt resource file |
| `translations/` | Qt translations (en, pt, fr, it, zh_CN); the source language is Spanish |
| `debian/` | Debian packaging |

## Install via .deb package

Build the package:

```bash
dpkg-buildpackage -us -uc -b
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