## FontEdit SwiftUI prototype

This directory contains a SwiftUI reimagining of the original Qt-based FontEdit
application. It focuses on recreating the core flows—browsing glyphs, editing a
bitmap canvas, and exporting the data as C/Arduino or Python source.

### Project layout

```
swiftui/FontEditSwiftUI
├── FontEditSwiftUIApp.swift      # Entry point for the macOS SwiftUI app
├── Models                        # Glyph + FontDocument data structures
├── ViewModels                    # Observable editing/export logic
└── Views                         # SwiftUI screens and controls
```

The app ships with a sample font document so it can be previewed immediately.
Glyphs can be added from single characters, edited by toggling pixels, reset to
blank, and exported using configurable bit order, inversion, and indentation.

### Running the app

Open `swiftui/FontEditSwiftUI` in Xcode 14+ on macOS 12 or newer and hit Run.
The app uses only SwiftUI, Foundation, and AppKit for pasteboard support—no
other dependencies are required.

### Notes on feature parity

* The exporter mirrors the original app’s byte ordering and inversion options,
  but does not yet include partial exports or UI for line spacing.
* Font importers are not implemented; the sample data demonstrates the flow and
  can be extended to load system fonts using CoreText in the future.
* The canvas uses a simple boolean bitmap; adopting Core Graphics would allow
  precise glyph rasterization later.
