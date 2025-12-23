import Foundation

struct Glyph: Identifiable, Hashable {
    let id: UUID
    var character: Character?
    var pixels: [[Bool]]

    init(id: UUID = UUID(), character: Character? = nil, pixels: [[Bool]]) {
        self.id = id
        self.character = character
        self.pixels = pixels
    }

    static func empty(width: Int, height: Int, character: Character? = nil) -> Glyph {
        let pixels = Array(
            repeating: Array(repeating: false, count: width),
            count: height
        )
        return Glyph(character: character, pixels: pixels)
    }

    var width: Int { pixels.first?.count ?? 0 }
    var height: Int { pixels.count }
}
