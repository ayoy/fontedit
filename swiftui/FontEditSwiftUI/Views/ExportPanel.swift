import SwiftUI
#if os(macOS)
import AppKit
#endif

struct ExportPanel: View {
    @EnvironmentObject private var document: FontDocumentViewModel
    @State private var exportMode: ExportMode = .c
    @State private var copiedFeedback: Bool = false

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack {
                Text("Export")
                    .font(.headline)
                Spacer()
                Picker("Mode", selection: $exportMode) {
                    ForEach(ExportMode.allCases) { mode in
                        Text(mode.title).tag(mode)
                    }
                }
                .pickerStyle(.segmented)
                Button {
                    copyToPasteboard()
                } label: {
                    Label(copiedFeedback ? "Copied!" : "Copy", systemImage: "doc.on.doc")
                        .labelStyle(.iconOnly)
                }
                .keyboardShortcut("c", modifiers: [.command, .shift])
                .help("Copy exported code")
            }

            VStack(alignment: .leading, spacing: 8) {
                Toggle("MSB first", isOn: $document.exportOptions.msbFirst)
                Toggle("Invert bits", isOn: $document.exportOptions.invertBits)
                Toggle("Include line spacing metadata", isOn: $document.exportOptions.includeLineSpacing)
                Stepper(value: $document.exportOptions.tabSize, in: 2...8) {
                    Text("Tab size: \(document.exportOptions.tabSize)")
                }
                .help("Controls indentation in exported code")
            }

            TextEditor(text: .constant(exportedText))
                .font(.system(.body, design: .monospaced))
                .frame(minHeight: 220)
                .overlay(
                    RoundedRectangle(cornerRadius: 10)
                        .stroke(Color.secondary.opacity(0.2), lineWidth: 1)
                )
        }
    }

    private var exportedText: String {
        switch exportMode {
        case .c:
            return document.exportC()
        case .python:
            return document.exportPython()
        }
    }

    private func copyToPasteboard() {
#if os(macOS)
        let pasteboard = NSPasteboard.general
        pasteboard.clearContents()
        pasteboard.setString(exportedText, forType: .string)
        withAnimation(.easeInOut) {
            copiedFeedback = true
        }
        DispatchQueue.main.asyncAfter(deadline: .now() + 1.4) {
            withAnimation(.easeInOut) {
                copiedFeedback = false
            }
        }
#endif
    }
}

private enum ExportMode: String, CaseIterable, Identifiable {
    case c, python

    var id: String { rawValue }

    var title: String {
        switch self {
        case .c: return "C/Arduino"
        case .python: return "Python"
        }
    }
}
