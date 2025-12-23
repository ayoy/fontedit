import SwiftUI

struct ContentView: View {
    @EnvironmentObject private var document: FontDocumentViewModel
    @State private var newGlyphCharacter: String = ""

    var body: some View {
        NavigationSplitView {
            VStack(alignment: .leading, spacing: 12) {
                Text("Glyphs")
                    .font(.headline)
                    .padding(.top, 8)

                List(selection: $document.selectedGlyphID) {
                    ForEach(document.document.glyphs) { glyph in
                        GlyphRow(glyph: glyph)
                            .tag(glyph.id)
                            .onTapGesture { document.select(glyph) }
                    }
                    .onDelete(perform: document.removeGlyph)
                }
                .listStyle(.inset)

                HStack {
                    TextField("Add glyph (character)", text: $newGlyphCharacter)
                        .textFieldStyle(.roundedBorder)
                        .frame(minWidth: 120)
                        .onSubmit(addGlyphIfNeeded)
                    Button("Add", action: addGlyphIfNeeded)
                        .buttonStyle(.borderedProminent)
                        .disabled(newGlyphCharacter.count != 1)
                }
            }
            .padding()
            .frame(minWidth: 280)
        } detail: {
            VStack(spacing: 16) {
                HStack {
                    Text(document.selectedGlyph?.character.map(String.init) ?? "Custom glyph")
                        .font(.title2)
                    Spacer()
                    Button("Reset glyph", action: document.resetGlyph)
                        .disabled(document.selectedGlyph == nil)
                }

                GlyphCanvasView(
                    glyph: document.selectedGlyph,
                    pixelSize: 18,
                    onToggle: document.togglePixel(row:column:)
                )
                .padding(.bottom, 8)

                ExportPanel()
            }
            .padding()
            .frame(minWidth: 600, minHeight: 520)
        }
    }

    private func addGlyphIfNeeded() {
        guard let character = newGlyphCharacter.first else { return }
        document.addGlyph(from: character)
        newGlyphCharacter.removeAll()
    }
}

private struct GlyphRow: View {
    let glyph: Glyph

    var body: some View {
        HStack {
            GlyphCanvasView(glyph: glyph, pixelSize: 6, onToggle: { _, _ in })
                .frame(width: 80, height: 60)
            VStack(alignment: .leading) {
                Text(glyph.character.map(String.init) ?? "Custom")
                Text("\(glyph.width)x\(glyph.height)")
                    .foregroundStyle(.secondary)
                    .font(.footnote)
            }
        }
    }
}
