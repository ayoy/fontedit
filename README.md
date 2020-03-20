# FontEdit

FontEdit is a desktop application that allows you to convert general-purpose 
fixed-width desktop fonts to byte array representation that's suitable for
use in embedded systems displays.

It's written in C++ with Qt UI and was tested on Windows, Linux and MacOS.

Read more about it in [the blog post](https://kapusta.cc/2019/03/20/fontedit/).

![FontEdit](https://kapusta.cc/assets/fontedit/imported_font.png)

## Getting FontEdit

### Packages

The [Releases GitHub page](https://github.com/ayoy/fontedit/releases) contains
packages for:
* Ubuntu/Debian (amd64),
* Raspbian Buster (armhf),
* MacOS,
* Windows.

### Building from source

Prerequisites:

* Qt (tested with >= 5.9)
* cmake (3.9 or newer)
* C++ compiler that supports C++17

Follow these steps to build the app from the source code:

1. Clone the Git repository:

    ```
    $ git clone https://github.com/ayoy/fontedit
    $ cd fontedit
    ```

2. Check out Git submodules:

    ```
    $ git submodule update --init
    ```

3. Build with CMake:

    ```
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    $ make
    ```

4. (Optionally) Install on Linux with: `make install` or create a dmg
  image on MacOS with `make dmg`.

## Features

With FontEdit you can:

* import fonts from the operating system - to load a custom font, you should
  first register it in your OS,
* edit individual font glyphs after importing - automatic import is a best-effort
  operation and although the font should be usable right after importing, you
  might want to tweak it so that it looks better,
* add new glyphs to a font document - either by copying an existing glyph, starting 
  from scratch or adding a glyph from a character you input (useful for adding
  non-ASCII characters to your font),
* export the font as source code (in a form of byte array) suitable for Arduino,
  C/C++ or Python,
* save your progress to a file - the font document file is cross-platform so you can
  e.g. import and edit it on MacOS and then move to RPi and export the code from there.

### Font Editor

You can edit font glyphs with a minimal editor that's controlled with a mouse
and keyboard. Click and drag the mouse to set pixels (making them black), hold
Alt or Ctrl (⌘) to erase. Use touchpad scroll (mouse wheel) with Ctrl (⌘) to zoom
the editor canvas.

You can also reset the current glyph or the whole font to their initial state
(from latest save). The editor supports Undo/Redo for most operations.

### Source Code Export

The font data can be exported to:

* a C file (also suitable for use with C++),
* an Arduino-specific C file (using PROGMEM),
* a Python list or bytes object (both compatible with Python 2.x/3.x and MicroPython).

You can switch between MSB and LSB mode, invert all the bits, and conditionally include
line spacings in font definition (not recommended unless you have a very good reason
for it). The tab size can be configured.

## Bugs, ideas, improvements

Please report bugs and feature requests via [GitHub Issues](https://github.com/ayoy/fontedit/issues) or as a [pull request](https://github.com/ayoy/fontedit/pulls).

## License

© 2020 Dominik Kapusta

This app is distributed in accordance with GPL v3. See [LICENSE](https://github.com/ayoy/fontedit/blob/master/LICENSE) for details.
The app uses icons from [www.flaticon.com](https://www.flaticon.com) made by
[Smashicons](https://www.flaticon.com/authors/smashicons),
[Freepik](https://www.flaticon.com/authors/freepik) and
[Pixel perfect](https://www.flaticon.com/authors/pixel-perfect).
