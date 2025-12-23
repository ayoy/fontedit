import SwiftUI

struct GlyphCanvasView: View {
    let glyph: Glyph?
    var pixelSize: CGFloat = 22
    var onToggle: (_ row: Int, _ column: Int) -> Void

    var body: some View {
        let width = glyph?.width ?? 0
        let height = glyph?.height ?? 0

        VStack(spacing: 1) {
            ForEach(0..<height, id: \.self) { row in
                HStack(spacing: 1) {
                    ForEach(0..<width, id: \.self) { column in
                        PixelView(
                            isOn: glyph?.pixels[row][column] ?? false,
                            size: pixelSize
                        )
                        .onTapGesture {
                            onToggle(row, column)
                        }
                    }
                }
            }
        }
        .padding(6)
        .background(.thinMaterial)
        .clipShape(RoundedRectangle(cornerRadius: 8))
        .overlay(
            RoundedRectangle(cornerRadius: 8)
                .stroke(.quaternary, lineWidth: 1)
        )
    }
}

private struct PixelView: View {
    let isOn: Bool
    let size: CGFloat

    var body: some View {
        RoundedRectangle(cornerRadius: size * 0.12, style: .continuous)
            .fill(isOn ? Color.primary : Color(white: 0.92))
            .frame(width: size, height: size)
            .overlay(
                RoundedRectangle(cornerRadius: size * 0.12, style: .continuous)
                    .stroke(Color.primary.opacity(0.12), lineWidth: 0.8)
            )
    }
}
