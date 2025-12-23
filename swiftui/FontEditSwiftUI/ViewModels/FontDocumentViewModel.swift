import Foundation
import SwiftUI

final class FontDocumentViewModel: ObservableObject {
    @Published var document: FontDocument
    @Published var exportOptions = ExportOptions()
    @Published var selectedGlyphID: Glyph.ID?

    init(document: FontDocument = .sample()) {
        self.document = document
        self.selectedGlyphID = document.glyphs.first?.id
    }

    var selectedGlyph: Glyph? {
        guard let id = selectedGlyphID else { return nil }
        return document.glyphs.first(where: { $0.id == id })
    }

    func select(_ glyph: Glyph) {
        selectedGlyphID = glyph.id
    }

    func togglePixel(row: Int, column: Int) {
        guard let index = document.glyphs.firstIndex(where: { $0.id == selectedGlyphID }),
              row < document.glyphHeight, column < document.glyphWidth else {
            return
        }

        document.glyphs[index].pixels[row][column].toggle()
        objectWillChange.send()
    }

    func resetGlyph() {
        guard let index = document.glyphs.firstIndex(where: { $0.id == selectedGlyphID }) else { return }
        let character = document.glyphs[index].character
        document.glyphs[index] = .empty(width: document.glyphWidth, height: document.glyphHeight, character: character)
        objectWillChange.send()
    }

    func addGlyph(from character: Character) {
        var newGlyph = Glyph.empty(width: document.glyphWidth, height: document.glyphHeight, character: character)

        // Seed with a simple diagonal to give the user something to start with.
        for row in 0..<min(document.glyphHeight, document.glyphWidth) {
            newGlyph.pixels[row][row] = true
        }

        document.glyphs.append(newGlyph)
        selectedGlyphID = newGlyph.id
    }

    func removeGlyph(at offsets: IndexSet) {
        document.glyphs.remove(atOffsets: offsets)
        selectedGlyphID = document.glyphs.first?.id
    }

    func exportC() -> String {
        document.exportC(options: exportOptions)
    }

    func exportPython() -> String {
        document.exportPython(options: exportOptions)
    }
}
