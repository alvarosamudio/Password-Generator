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